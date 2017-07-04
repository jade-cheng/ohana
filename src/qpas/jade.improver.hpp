/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_IMPROVER_HPP__
#define JADE_IMPROVER_HPP__

#include "jade.forced_grouping.hpp"
#include "jade.qpas.hpp"
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

        /// The QPAS type.
        typedef basic_qpas<value_type> qpas_type;

        ///
        /// \return A new-and-improved F matrix.
        ///
        static matrix_type improve_f(
                const genotype_matrix_type & g,   ///< The G matrix.
                const matrix_type &          q,   ///< The Q matrix.
                const matrix_type &          fa,  ///< The F matrix.
                const matrix_type &          fb,  ///< The 1-F matrix.
                const matrix_type &          qfa, ///< The Q*F matrix.
                const matrix_type &          qfb) ///< The Q*(1-F) matrix.
        {
            assert(verification_type::validate_gqf_sizes(g, q, fa));
            assert(verification_type::validate_gqf_sizes(g, q, fb));
            assert(verification_type::validate_q(q));
            assert(verification_type::validate_f(fa));

            const auto K = fa.get_height();
            const auto J = fa.get_width();

            matrix_type f_dst (K, J);

            static const std::vector<size_t> fixed_active_set;

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

                const auto coefficients_mat = _create_coefficients_mat(K, 0);
                const auto b_vec            = _create_b_vec(f_column, 0);

                std::vector<size_t> active_set { 0 };
                matrix_type delta_vec (K, 1);
                delta_vec[0] = -b_vec[0];

                qpas_type::loop_over_active_set(
                        b_vec,
                        coefficients_mat,
                        hessian_mat,
                        derivative_vec,
                        fixed_active_set,
                        active_set,
                        delta_vec);

                for (size_t k = 0; k < K; k++)
                    f_dst(k, j) = f_column[k] + delta_vec[k];
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

            const std::vector<size_t> fixed_active_set { K + K };

            matrix_type derivative_vec (K, 1);
            matrix_type hessian_mat (K, K);

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

                const auto coefficients_mat = _create_coefficients_mat(K, 1);

                auto b_vec = _create_b_vec(q_row, 1);
                if (nullptr != fg)
                {
                    for (size_t k = 0; k < K; k++)
                    {
                        b_vec[k + 0] -= fg->get_min(i, k);
                        b_vec[k + K] += fg->get_max(i, k) - value_type(1);
                    }
                }

                std::vector<size_t> active_set { 0 };
                matrix_type delta_vec (K, 1);
                delta_vec[0] = -b_vec[0];

                qpas_type::loop_over_active_set(
                        b_vec,
                        coefficients_mat,
                        hessian_mat,
                        derivative_vec,
                        fixed_active_set,
                        active_set,
                        delta_vec);

                for (size_t k = 0; k < K; k++)
                    q_dst(i, k) = q_row[k] + delta_vec[k];

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
        static matrix_type _create_b_vec(
                const matrix_type & current_values,
                const size_t        row_padding)
        {
            assert(current_values.is_vector());

            const auto K = current_values.get_length();

            matrix_type b_vec (K + K + row_padding, 1);

            for (size_t k = 0; k < K; k++)
            {
                b_vec[k] = current_values[k];
                b_vec[k + K] = value_type(1) - current_values[k];
            }

            for (size_t k = K + K; k < b_vec.get_height(); k++)
                b_vec[k] = value_type(0);

            return b_vec;
        }

        // --------------------------------------------------------------------
        static matrix_type _create_coefficients_mat(
                const size_t K,
                const size_t row_padding)
        {
            matrix_type c_mat (K + K + row_padding, K);

            for (size_t k = 0; k < K; k++)
            {
                c_mat(k,     k) = value_type(-1.0);
                c_mat(K + k, k) = value_type(+1.0);
            }

            for (size_t r = K + K; r < c_mat.get_height(); r++)
                for (size_t c = 0; c < c_mat.get_width(); c++)
                    c_mat(r, c) = value_type(1.0);

            return c_mat;
        }
    };
}

#endif // JADE_IMPROVER_HPP__
