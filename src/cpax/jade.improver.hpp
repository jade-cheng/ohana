/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_IMPROVER_HPP__
#define JADE_IMPROVER_HPP__

#include "jade.forced_grouping.hpp"
#include "jade.lemke.hpp"
#include "jade.verification.hpp"

namespace jade
{
    ///
    /// A template for a class that improves the Q and F matrices.
    ///
    template <typename TValue>
    class basic_improver
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The genotype matrix type.
        typedef basic_genotype_matrix<value_type> genotype_matrix_type;

        /// The forced grouping type.
        typedef basic_forced_grouping<value_type> forced_grouping_type;

        /// The verification type.
        typedef basic_verification<value_type> verification_type;

        /// The Lemke type.
        typedef basic_lemke<value_type> lemke_type;

        ///
        /// \return A new-and-improved F matrix.
        ///
        static matrix_type improve_f(
                const genotype_matrix_type & g,   ///< The G matrix.
                const matrix_type &          q,   ///< The Q matrix.
                const matrix_type &          fa,  ///< The F matrix.
                const matrix_type &          fb,  ///< The 1-F matrix.
                const matrix_type &          qfa, ///< The Q*F matrix.
                const matrix_type &          qfb, ///< The Q*(1-F) matrix.
                const matrix_type *          fif) ///< The Fin-force matrix.
        {
            assert(verification_type::validate_gqf_sizes(g, q, fa));
            assert(verification_type::validate_gqf_sizes(g, q, fb));
            assert(verification_type::validate_q(q));
            assert(verification_type::validate_f(fa));

            const auto K = fa.get_height();
            const auto J = fa.get_width();
            assert(nullptr == fif || verification_type::
                validate_fif_size(*fif, K, J));

            matrix_type f_dst (K, J);

            matrix_type shift_vec (K, 1);
            shift_vec.set_values(1);

            matrix_type derivative_vec (K, 1);
            matrix_type hessian_mat    (K, K);

            for (size_t j = 0; j < J; j++)
            {
                const auto f_column = fa.copy_column(j);

                g.compute_derivatives_f(
                        q,
                        fa,
                        fb,
                        qfa,
                        qfb,
                        j,
                        derivative_vec,
                        hessian_mat);

                const auto a_mat = _create_a_mat(K, false);

                auto b_vec = _create_b_vec(f_column, a_mat, shift_vec, false);
                if (nullptr != fif)
                {
                    for (size_t k = 0; k < fif->get_height(); k++)
                    {
                        b_vec[k + 0] = value_type(0);
                        b_vec[k + K] = value_type(0);
                    }
                }

                const auto sqp_q = -hessian_mat;
                const auto sqp_a = -a_mat;
                const auto sqp_c = (hessian_mat * shift_vec) - derivative_vec;
                const auto sqp_b = -b_vec;

                matrix_type shifted_delta_vec;
                if (lemke_type::solve(
                        shifted_delta_vec,
                        sqp_q,  // shifted QP's "Q" matrix
                        sqp_a,  // shifted QP's "A" matrix
                        sqp_c,  // shifted QP's "c" vector
                        sqp_b)) // shifted QP's "b" vector
                {
                    assert(shifted_delta_vec.get_height() == 3 * K);
                    for (size_t k = 0; k < K; k++)
                        f_dst(k, j) = f_column[k]
                                    + shifted_delta_vec[k]
                                    - value_type(1);
                }
                else
                {
                    for (size_t k = 0; k < K; k++)
                        f_dst(k, j) = f_column[k];
                }
            }

            f_dst.clamp(min, max);
            return f_dst;
        }

        ///
        /// \return A new-and-improved Q matrix.
        ///
        static matrix_type improve_q(
                const genotype_matrix_type & g,   ///< The G matrix.
                const matrix_type &          q,   ///< The Q matrix.
                const matrix_type &          fa,  ///< The F matrix.
                const matrix_type &          fb,  ///< The 1-F matrix.
                const matrix_type &          qfa, ///< The Q*F matrix.
                const matrix_type &          qfb, ///< The Q*(1-F) matrix.
                const forced_grouping_type * fg)  ///< The force-grouping.
        {
            assert(verification_type::validate_gqf_sizes(g, q, fa));
            assert(verification_type::validate_gqf_sizes(g, q, fb));
            assert(verification_type::validate_q(q));
            assert(verification_type::validate_f(fa));

            const auto I = q.get_height();
            const auto K = q.get_width();

            matrix_type q_dst (I, K);

            matrix_type shift_vec (K, 1);
            shift_vec.set_values(1);

            matrix_type derivative_vec (K, 1);
            matrix_type hessian_mat    (K, K);

            for (size_t i = 0; i < I; i++)
            {
                const auto q_row = q.copy_row(i);

                g.compute_derivatives_q(
                        q,
                        fa,
                        fb,
                        qfa,
                        qfb,
                        i,
                        derivative_vec,
                        hessian_mat);

                const auto a_mat = _create_a_mat(K, true);

                auto b_vec = _create_b_vec(q_row, a_mat, shift_vec, true);
                if (nullptr != fg)
                {
                    for (size_t k = 0; k < K; k++)
                    {
                        b_vec[k + 0] -= fg->get_min(i, k);
                        b_vec[k + K] += fg->get_max(i, k) - value_type(1);
                    }
                }

                const auto sqp_q = -hessian_mat;
                const auto sqp_a = -a_mat;
                const auto sqp_c = (hessian_mat * shift_vec) - derivative_vec;
                const auto sqp_b = -b_vec;

                matrix_type shifted_delta_vec;
                if (lemke_type::solve(
                        shifted_delta_vec,
                        sqp_q,  // shifted QP's "Q" matrix
                        sqp_a,  // shifted QP's "A" matrix
                        sqp_c,  // shifted QP's "c" vector
                        sqp_b)) // shifted QP's "b" vector
                {
                    assert(shifted_delta_vec.get_height() == 3 * K);
                    for (size_t k = 0; k < K; k++)
                        q_dst(i, k) = q_row[k]
                                    + shifted_delta_vec[k]
                                    - value_type(1);
                }
                else
                {
                    for (size_t k = 0; k < K; k++)
                        q_dst(i, k) = q_row[k];
                }

                q_dst.clamp_row(i, min, max);

                const auto sum = q_dst.get_row_sum(i);
                q_dst.multiply_row(i, value_type(1) / sum);
            }

            return q_dst;
        }

    private:
        static constexpr auto min = value_type(0.000001);
        static constexpr auto max = value_type(0.999999);

        // --------------------------------------------------------------------
        static matrix_type _create_a_mat(
                const size_t K,
                const bool   is_padded)
        {
            matrix_type c_mat (K + K + (is_padded ? 2 : 0), K);

            for (size_t k = 0; k < K; k++)
            {
                c_mat(k + 0, k) = value_type(-1);
                c_mat(K + k, k) = value_type(+1);
            }

            if (is_padded)
            {
                for (size_t k = 0; k < K; k++)
                {
                    c_mat(K + K + 0, k) = value_type(+1);
                    c_mat(K + K + 1, k) = value_type(-1);
                }
            }

            return c_mat;
        }

        // --------------------------------------------------------------------
        static matrix_type _create_b_vec(
                const matrix_type & current_values,
                const matrix_type & a_mat,
                const matrix_type & shift_vec,
                const bool          is_padded)
        {
            assert(current_values.is_vector());

            const auto K = current_values.get_length();

            matrix_type b_vec (K + K + (is_padded ? 2 : 0), 1);

            for (size_t k = 0; k < K; k++)
            {
                b_vec[k + 0] = value_type(0);
                b_vec[k + K] = value_type(1);
            }

            if (is_padded)
            {
                b_vec[K + K + 0] = value_type(+1);
                b_vec[K + K + 1] = value_type(-1);
            }

            b_vec -= a_mat * current_values.create_transpose();
            b_vec += a_mat * shift_vec;

            return b_vec;
        }
    };
}

#endif // JADE_IMPROVER_HPP__
