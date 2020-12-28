/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_DISCRETE_GENOTYPE_MATRIX_HPP__
#define JADE_DISCRETE_GENOTYPE_MATRIX_HPP__

#include "jade.genotype.hpp"
#include "jade.verification.hpp"

namespace jade
{
    ///
    /// A template for a class implementing operations for a discrete genotype
    /// matrix.
    ///
    template <typename TValue>
    class basic_discrete_genotype_matrix
        : public basic_genotype_matrix<TValue>
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The genotype matrix type.
        typedef basic_matrix<genotype> genotype_matrix_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The verification type.
        typedef basic_verification<value_type> verification_type;

        /// The initializer list for the genotype matrix.
        typedef typename
                genotype_matrix_type::initializer_list_type
                initializer_list_type;

        ///
        /// Initializes a new instance of the class.
        ///
        inline basic_discrete_genotype_matrix()
            : _g ()
        {
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        inline explicit basic_discrete_genotype_matrix(
                char const * const path) ///< The path to the file.
            : _g (path)
        {
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        inline explicit basic_discrete_genotype_matrix(
                const std::string & path) ///< The path to the file.
            : _g (path)
        {
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified input stream.
        ///
        inline explicit basic_discrete_genotype_matrix(
                std::istream & in) ///< The input stream.
            : _g (in)
        {
        }

        ///
        /// Initializes a new instance of the class based on the specified
        /// values.
        ///
        /// \param values The two-dimensional values.
        ///
        inline explicit basic_discrete_genotype_matrix(
                const initializer_list_type & values)
            : _g (values)
        {
        }

        ///
        /// \return This instance.
        ///
        inline const basic_discrete_genotype_matrix * as_dgm() const override
        {
            return this;
        }

        ///
        /// \return This instance.
        ///
        inline basic_discrete_genotype_matrix * as_dgm() override
        {
            return this;
        }

        ///
        /// Computes the derivative vector and hessian matrix for a specified
        /// marker of the F matrix.
        ///
        virtual void compute_derivatives_f(
                const matrix_type & q,     ///< The Q matrix.
                const matrix_type & ,      ///< The F matrix.
                const matrix_type & ,      ///< The 1-F matrix.
                const matrix_type & qfa,   ///< The Q*F product.
                const matrix_type & qfb,   ///< The Q*(1-F) product.
                const size_t        j,     ///< The marker.
                matrix_type &       d_vec, ///< The derivative vector.
                matrix_type &       h_mat) ///< The hessian matrix.
                const override
        {
            const auto J = _g.get_width();
            const auto K = d_vec.get_height();

            #ifndef NDEBUG
            const auto I = _g.get_height();
            assert(q.is_size(I, K));
            assert(qfa.is_size(I, J));
            assert(qfb.is_size(I, J));
            assert(j < J);
            assert(d_vec.is_size(K, 1));
            assert(h_mat.is_size(K, K));
            #endif // NDEBUG

            h_mat.set_values(0);
            d_vec.set_values(0);

            //
            // for (size_t i = 0; i < I; i++)
            //   g_ij   --> g(i, j)
            //   q_i0   --> q(i, 0)
            //   qfa_ij --> qfa(i, j)
            //   qfb_ij --> qfb(i, j)
            //
            auto       g_ij_ptr   = _g.get_data() + j;
            auto       q_i0_ptr   = q.get_data();
            auto       qfa_ij_ptr = qfa.get_data() + j;
            auto       qfb_ij_ptr = qfb.get_data() + j;
            const auto g_ij_end   = g_ij_ptr + _g.get_length();
            const auto g_step     = J;
            const auto q_step     = K;
            const auto qf_step    = J;
            while (g_ij_ptr != g_ij_end)
            {
                auto is_evaluated = true;
                value_type g_ij;
                switch (*g_ij_ptr)
                {
                    case genotype_major_major: g_ij = 0; break;
                    case genotype_major_minor: g_ij = 1; break;
                    case genotype_minor_minor: g_ij = 2; break;
                    default: is_evaluated = false; break;
                }

                if (is_evaluated)
                {
                    const auto qfa_ij = *qfa_ij_ptr;
                    const auto qfb_ij = *qfb_ij_ptr;
                    const auto term1  = g_ij / qfa_ij;
                    const auto term2  = (2 - g_ij) / qfb_ij;
                    const auto term3  = term1 - term2;
                    const auto term4  = term1 / qfa_ij + term2 / qfb_ij;

                    //
                    // for (size_t k1 = 0; k1 < K; k1++)
                    //   d     --> d_vec[k1]
                    //   h     --> h_mat(k1, ...)
                    //   q_ik1 --> q(i, k1)
                    //
                    auto       d_ptr     = d_vec.get_data();
                    auto       h_ptr     = h_mat.get_data();
                    auto       q_ik1_ptr = q_i0_ptr;
                    const auto h_end     = h_ptr + h_mat.get_length();
                    while (h_ptr != h_end)
                    {
                        const auto q_ik1 = *q_ik1_ptr;

                        *d_ptr += term3 * q_ik1;

                        //
                        // for (size_t k2 = 0; k2 < K; k2++)
                        //   q_ik2 --> q(i, k2)
                        //
                        auto       q_ik2_ptr = q_i0_ptr;
                        const auto q_ik2_end = q_ik2_ptr + q.get_width();
                        while (q_ik2_ptr != q_ik2_end)
                        {
                            *h_ptr -= term4 * q_ik1 * *q_ik2_ptr;

                            h_ptr++;
                            q_ik2_ptr++;
                        }

                        q_ik1_ptr++;
                        d_ptr++;
                    }
                }

                g_ij_ptr   += g_step;
                qfa_ij_ptr += qf_step;
                qfb_ij_ptr += qf_step;
                q_i0_ptr   += q_step;
            }
        }

        ///
        /// Computes the derivative vector and hessian matrix for a specified
        /// individual of the Q matrix.
        ///
        virtual void compute_derivatives_q(
                const matrix_type & ,      ///< The Q matrix.
                const matrix_type & fa,    ///< The F matrix.
                const matrix_type & fb,    ///< The 1-F matrix.
                const matrix_type & qfa,   ///< The Q*F product.
                const matrix_type & qfb,   ///< The Q*(1-F) product.
                const size_t        i,     ///< The individual.
                matrix_type &       d_vec, ///< The derivative vector.
                matrix_type &       h_mat) ///< The hessian matrix.
                const override
        {
            const auto J = _g.get_width();

            #ifndef NDEBUG
            const auto I = _g.get_height();
            const auto K = d_vec.get_height();
            assert(fa.is_size(K, J));
            assert(fb.is_size(K, J));
            assert(qfa.is_size(I, J));
            assert(qfb.is_size(I, J));
            assert(i < I);
            assert(d_vec.is_size(K, 1));
            assert(h_mat.is_size(K, K));
            #endif // NDEBUG

            h_mat.set_values(0);
            d_vec.set_values(0);

            //
            // for (size_t j = 0; j < J; j++)
            //   g_ij    --> g(i, j)
            //   q_fa_ij --> q_fa(i, j)
            //   q_fb_ij --> q_fb(i, j)
            //   fa_0j   --> q_fa(0, j)
            //   fb_0j   --> q_fb(0, j)
            //
            auto       g_ij_ptr   = _g.get_data(i, 0);
            const auto g_ij_end   = g_ij_ptr + J;
            auto       qfa_ij_ptr = qfa.get_data(i, 0);
            auto       qfb_ij_ptr = qfb.get_data(i, 0);
            auto       fa_0j_ptr  = fa.get_data();
            auto       fb_0j_ptr  = fb.get_data();
            const auto f_step     = J;
            while (g_ij_ptr != g_ij_end)
            {
                bool is_evaluated = true;
                value_type g_ij;
                switch (*g_ij_ptr)
                {
                    case genotype_major_major: g_ij = 0; break;
                    case genotype_major_minor: g_ij = 1; break;
                    case genotype_minor_minor: g_ij = 2; break;
                    default: is_evaluated = false; break;
                }

                if (is_evaluated)
                {
                    const auto qfa_ij = *qfa_ij_ptr;
                    const auto qfb_ij = *qfb_ij_ptr;
                    const auto term1  = g_ij / qfa_ij;
                    const auto term2  = (2 - g_ij) / qfb_ij;
                    const auto term3  = term1 / qfa_ij;
                    const auto term4  = term2 / qfb_ij;

                    //
                    // for (size_t k1 = 0; k1 < K; k1++)
                    //   fa_k1j --> fa(k1, j)
                    //   fb_k1j --> fb(k1, j)
                    //   d      --> d_vec[k1]
                    //   h      --> h_mat(k1, ...)
                    //
                    auto       fa_k1j_ptr = fa_0j_ptr;
                    auto       fb_k1j_ptr = fb_0j_ptr;
                    auto       d_ptr      = d_vec.get_data();
                    auto       h_ptr      = h_mat.get_data();
                    const auto h_end      = h_ptr + h_mat.get_length();
                    while (h_ptr != h_end)
                    {
                        const auto fa_k1j = *fa_k1j_ptr;
                        const auto fb_k1j = *fb_k1j_ptr;

                        *d_ptr += term1 * fa_k1j + term2 * fb_k1j;

                        //
                        // for (size_t k2 = 0; k2 < K; k2++)
                        //   fa_k2j --> fa(k2, j)
                        //   fb_k2j --> fb(k2, j)
                        //
                        auto       fa_k2j_ptr  = fa_0j_ptr;
                        auto       fb_k2j_ptr  = fb_0j_ptr;
                        const auto fa_k2j_end  = fa_k2j_ptr + fa.get_length();
                        while (fa_k2j_ptr != fa_k2j_end)
                        {
                            const auto fa_k2j = *fa_k2j_ptr;
                            const auto fb_k2j = *fb_k2j_ptr;

                            *h_ptr -= (term3 * fa_k1j * fa_k2j)
                                    + (term4 * fb_k1j * fb_k2j);

                            fa_k2j_ptr += f_step;
                            fb_k2j_ptr += f_step;
                            h_ptr++;
                        }

                        fa_k1j_ptr += f_step;
                        fb_k1j_ptr += f_step;
                        d_ptr++;
                    }
                }

                g_ij_ptr++;
                qfa_ij_ptr++;
                qfb_ij_ptr++;
                fa_0j_ptr++;
                fb_0j_ptr++;
            }
        }

        ///
        /// \return The log of the likelihood function.
        ///
        virtual value_type compute_lle(
                const matrix_type & q,   ///< The Q matrix.
                const matrix_type & fa,  ///< The F matrix for major alleles.
                const matrix_type & fb,  ///< The F matrix for minor alleles.
                const matrix_type & ,    ///< The Q*Fa product.
                const matrix_type & )    ///< The Q*Fb product.
                const override
        {
            const auto & g = _g;

            assert(verification_type::validate_gqf_sizes(*this, q, fa));
            assert(verification_type::validate_gqf_sizes(*this, q, fb));

            const auto J        = g.get_width();
            const auto K        = q.get_width();
            auto       g_ij_ptr = g.get_data();
            const auto g_ij_end = g.get_data() + g.get_length();
            auto       q_i0_ptr = q.get_data();
            auto       sum_i    = value_type(0);

            while (g_ij_ptr != g_ij_end)
            {
                auto fa_0j_ptr = fa.get_data();
                auto fa_0j_end = fa.get_data() + J;
                auto fb_0j_ptr = fb.get_data();
                auto sum_j     = value_type(0);

                while (fa_0j_ptr != fa_0j_end)
                {
                    switch (*g_ij_ptr++)
                    {
                    case genotype_major_major:
                        {
                            auto       q_ik_ptr  = q_i0_ptr;
                            const auto q_ik_end  = q_i0_ptr + K;
                            auto       fb_kj_ptr = fb_0j_ptr;
                            auto       sum_rhs   = value_type(0);

                            while (q_ik_ptr != q_ik_end)
                            {
                                const auto q_ik  = *q_ik_ptr;
                                const auto fb_kj = *fb_kj_ptr;

                                sum_rhs += q_ik * fb_kj;

                                q_ik_ptr++;
                                fb_kj_ptr += J;
                            }

                            sum_j += value_type(2) * std::log(sum_rhs);
                            break;
                        }

                    case genotype_major_minor:
                        {
                            auto       q_ik_ptr  = q_i0_ptr;
                            const auto q_ik_end  = q_i0_ptr + K;
                            auto       fa_kj_ptr = fa_0j_ptr;
                            auto       fb_kj_ptr = fb_0j_ptr;
                            auto       sum_lhs   = value_type(0);
                            auto       sum_rhs   = value_type(0);

                            while (q_ik_ptr != q_ik_end)
                            {
                                const auto q_ik  = *q_ik_ptr;
                                const auto fa_kj = *fa_kj_ptr;
                                const auto fb_kj = *fb_kj_ptr;

                                sum_lhs += q_ik * fa_kj;
                                sum_rhs += q_ik * fb_kj;

                                q_ik_ptr++;
                                fa_kj_ptr += J;
                                fb_kj_ptr += J;
                            }

                            sum_j += std::log(sum_lhs * sum_rhs);
                            break;
                        }

                    case genotype_minor_minor:
                        {
                            auto       q_ik_ptr  = q_i0_ptr;
                            const auto q_ik_end  = q_i0_ptr + K;
                            auto       fa_kj_ptr = fa_0j_ptr;
                            auto       sum_lhs   = value_type(0);

                            while (q_ik_ptr != q_ik_end)
                            {
                                const auto q_ik  = *q_ik_ptr;
                                const auto fa_kj = *fa_kj_ptr;

                                sum_lhs += q_ik * fa_kj;

                                q_ik_ptr++;
                                fa_kj_ptr += J;
                            }

                            sum_j += value_type(2) * std::log(sum_lhs);
                            break;
                        }
                    }

                    fa_0j_ptr++;
                    fb_0j_ptr++;
                }

                sum_i += sum_j;

                q_i0_ptr += K;
            }

            return sum_i;
        }

        ///
        /// \return A new mu matrix.
        ///
        virtual matrix_type create_mu(
            const value_type f_epsilon) ///< The F matrix boundary epsilon.
            const override
        {
            const auto f_min = value_type(0.0) + f_epsilon;
            const auto f_max = value_type(1.0) - f_epsilon;

            const auto I = _g.get_height();
            const auto J = _g.get_width();

            assert(I > 0);

            matrix_type mu (J, 1);

            for (size_t j = 0; j < J; j++)
            {
                auto sum = value_type(0.0);

                for (size_t i = 0; i < I; i++)
                {
                    switch (_g(i, j))
                    {
                    case jade::genotype_major_major:
                        sum += value_type(2.0);
                        break;

                    case jade::genotype_major_minor:
                        sum += value_type(1.0);
                        break;

                    default:
                        break;
                    }
                }

                mu[j] = std::min(std::max(
                        f_min,
                        sum / (value_type(2.0) * value_type(I))),
                        f_max);
            }

            return mu;
        }

        ///
        /// \return The base matrix.
        ///
        inline const genotype_matrix_type & get_matrix() const
        {
            return _g;
        }

        ///
        /// \return The height of the matrix.
        ///
        inline virtual size_t get_height() const override
        {
            return _g.get_height();
        }

        ///
        /// \return The string representation of the size of the matrix.
        ///
        inline virtual std::string get_size_str() const override
        {
            return _g.get_size_str();
        }

        ///
        /// \return The width of the matrix.
        ///
        inline virtual size_t get_width() const override
        {
            return _g.get_width();
        }

        ///
        /// \return The string representation of the matrix.
        ///
        inline virtual std::string str() const override
        {
            return _g.str();
        }

    private:
        genotype_matrix_type _g;
    };
}

#endif // JADE_DISCRETE_GENOTYPE_MATRIX_HPP__
