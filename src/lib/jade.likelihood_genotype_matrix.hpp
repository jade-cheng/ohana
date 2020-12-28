/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_LIKELIHOOD_GENOTYPE_MATRIX_HPP__
#define JADE_LIKELIHOOD_GENOTYPE_MATRIX_HPP__

#include "jade.genotype_matrix.hpp"

namespace jade
{
    ///
    /// A template class implementing operations for a likelihood genotype
    /// matrix type.
    ///
    template <typename TValue>
    class basic_likelihood_genotype_matrix
        : public basic_genotype_matrix<TValue>
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        ///
        /// Initializes a new instance of the class.
        ///
        basic_likelihood_genotype_matrix()
            : _g_aa ()
            , _g_Aa ()
            , _g_AA ()
        {
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified input stream.
        ///
        explicit basic_likelihood_genotype_matrix(
                std::istream & in) ///< The input stream.
            : _g_aa (in)
            , _g_Aa (in)
            , _g_AA (in)
        {
            _validate_sizes();
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        inline explicit basic_likelihood_genotype_matrix(
                const std::string & path) ///< The path to the file.
            : basic_likelihood_genotype_matrix(path.c_str())
        {
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        explicit basic_likelihood_genotype_matrix(
                char const * const path) ///< The path to the file.
            : basic_likelihood_genotype_matrix()
        {
            assert(path != nullptr);

            try
            {
                std::ifstream in (path);
                if (!in.good())
                    throw error("error reading file");

                basic_likelihood_genotype_matrix tmp (in);
                _g_aa.swap(tmp._g_aa);
                _g_Aa.swap(tmp._g_Aa);
                _g_AA.swap(tmp._g_AA);
            }
            catch (const std::exception & e)
            {
                throw error()
                    << "error reading likelihood genotype matrix '"
                    << path << "': " << e.what();
            }
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified matrices.
        ///
        explicit basic_likelihood_genotype_matrix(
                const matrix_type & g_aa, ///< The minor-minor matrix.
                const matrix_type & g_Aa, ///< The major-minor matrix.
                const matrix_type & g_AA) ///< The major-major matrix.
            : _g_aa (g_aa)
            , _g_Aa (g_Aa)
            , _g_AA (g_AA)
        {
            _validate_sizes();
        }

        ///
        /// \return This instance.
        ///
        inline const basic_likelihood_genotype_matrix * as_lgm() const override
        {
            return this;
        }

        ///
        /// \return This instance.
        ///
        inline basic_likelihood_genotype_matrix * as_lgm() override
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
            const auto J = get_width();
            const auto K = d_vec.get_height();

            #ifndef NDEBUG
            const auto I = get_height();
            assert(q.is_size(I, K));
            assert(qfa.is_size(I, J));
            assert(qfb.is_size(I, J));
            assert(j < J);
            assert(d_vec.is_size(K, 1));
            assert(h_mat.is_size(K, K));
            #endif // NDEBUG

            d_vec.set_values(0);
            h_mat.set_values(0);

            //
            // for (size_t i = 0; i < I; i++)
            //   g_ij   --> g(i, j)
            //   q_i0   --> q(i, 0)
            //   qfa_ij --> qfa(i, j)
            //   qfb_ij --> qfb(i, j)
            //
            auto       g_aa_ij_ptr = _g_aa.get_data() + j;
            auto       g_Aa_ij_ptr = _g_Aa.get_data() + j;
            auto       g_AA_ij_ptr = _g_AA.get_data() + j;
            auto       q_i0_ptr    = q.get_data();
            auto       qfa_ij_ptr  = qfa.get_data() + j;
            auto       qfb_ij_ptr  = qfb.get_data() + j;
            const auto g_aa_ij_end = g_aa_ij_ptr + _g_aa.get_length();
            const auto g_step      = J;
            const auto q_step      = K;
            const auto qf_step     = J;
            while (g_aa_ij_ptr != g_aa_ij_end)
            {
                const auto g_AA_ij = *g_AA_ij_ptr;
                const auto g_Aa_ij = *g_Aa_ij_ptr;
                const auto g_aa_ij = *g_aa_ij_ptr;
                const auto qfa_ij  = *qfa_ij_ptr;
                const auto qfb_ij  = *qfb_ij_ptr;

                const auto alpha = value_type(1) / (
                    g_AA_ij * qfa_ij * qfa_ij +
                    g_aa_ij * qfb_ij * qfb_ij +
                    g_Aa_ij * qfa_ij * qfb_ij * 2);

                const auto theta = 2 * (
                    g_AA_ij * qfa_ij -
                    g_aa_ij * qfb_ij +
                    g_Aa_ij * qfb_ij -
                    g_Aa_ij * qfa_ij);

                //
                // for (size_t k1 = 0; k1 < K; k1++)
                //   d     --> d_vec[k1]
                //   h     --> h_mat(k1, ...)
                //   q_ik1 --> q(i, k1)
                //
                auto       d_ptr     = d_vec.get_data();
                auto       h_ptr     = h_mat.get_data();
                auto       q_ik1_ptr = q_i0_ptr;
                const auto q_ik1_end = q_i0_ptr + K;
                const auto q_ik2_end = q_i0_ptr + K;
                while (q_ik1_ptr != q_ik1_end)
                {
                    const auto q_ik1 = *q_ik1_ptr;

                    *d_ptr += theta * alpha * q_ik1;

                    //
                    // for (size_t k2 = 0; k2 < K; k2++)
                    //   q_ik2 --> q(i, k2)
                    //
                    auto q_ik2_ptr = q_i0_ptr;
                    while (q_ik2_ptr != q_ik2_end)
                    {
                        const auto q_ik2 = *q_ik2_ptr;
                        const auto term = 2 * (g_AA_ij + g_aa_ij
                                       - (2 *  g_Aa_ij));

                        *h_ptr += alpha * q_ik1 * q_ik2 * (
                            term - (theta * theta * alpha));

                        h_ptr++;
                        q_ik2_ptr++;
                    }

                    d_ptr++;
                    q_ik1_ptr++;
                }

                g_AA_ij_ptr += g_step;
                g_Aa_ij_ptr += g_step;
                g_aa_ij_ptr += g_step;
                qfa_ij_ptr  += qf_step;
                qfb_ij_ptr  += qf_step;
                q_i0_ptr    += q_step;
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
            const auto J = get_width();

            #ifndef NDEBUG
            const auto I = get_height();
            const auto K = d_vec.get_height();
            assert(fa.is_size(K, J));
            assert(fb.is_size(K, J));
            assert(qfa.is_size(I, J));
            assert(qfb.is_size(I, J));
            assert(i < I);
            assert(d_vec.is_size(K, 1));
            assert(h_mat.is_size(K, K));
            #endif // NDEBUG

            d_vec.set_values(0);
            h_mat.set_values(0);

            //
            // for (size_t j = 0; j < J; j++)
            //   g_ij    --> g(i, j)
            //   q_fa_ij --> q_fa(i, j)
            //   q_fb_ij --> q_fb(i, j)
            //   fa_0j   --> q_fa(0, j)
            //   fb_0j   --> q_fb(0, j)
            //
            auto       g_AA_ij_ptr = _g_AA.get_data(i, 0);
            auto       g_Aa_ij_ptr = _g_Aa.get_data(i, 0);
            auto       g_aa_ij_ptr = _g_aa.get_data(i, 0);
            const auto g_aa_ij_end = g_aa_ij_ptr + J;
            auto       qfa_ij_ptr  = qfa.get_data(i, 0);
            auto       qfb_ij_ptr  = qfb.get_data(i, 0);
            auto       fa_0j_ptr   = fa.get_data();
            auto       fb_0j_ptr   = fb.get_data();
            auto       f_step      = J;
            while (g_aa_ij_ptr != g_aa_ij_end)
            {
                const auto g_AA_ij = *g_AA_ij_ptr;
                const auto g_Aa_ij = *g_Aa_ij_ptr;
                const auto g_aa_ij = *g_aa_ij_ptr;
                const auto qfa_ij  = *qfa_ij_ptr;
                const auto qfb_ij  = *qfb_ij_ptr;

                const auto alpha = value_type(1) / (
                    g_AA_ij * qfa_ij * qfa_ij +
                    g_aa_ij * qfb_ij * qfb_ij +
                    g_Aa_ij * qfa_ij * qfb_ij * 2);

                const auto theta = 2 * (
                    (g_AA_ij * qfa_ij) +
                    (g_Aa_ij * qfb_ij));

                const auto gamma = 2 * (
                    (g_aa_ij * qfb_ij) +
                    (g_Aa_ij * qfa_ij));

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

                    *d_ptr += alpha * ((theta * fa_k1j) + (gamma * fb_k1j));

                    //
                    // for (size_t k2 = 0; k2 < K; k2++)
                    //   fa_k2j --> fa(k2, j)
                    //   fb_k2j --> fb(k2, j)
                    //
                    auto       fa_k2j_ptr = fa_0j_ptr;
                    auto       fb_k2j_ptr = fb_0j_ptr;
                    const auto fa_k2j_end = fa_k2j_ptr + fa.get_length();
                    while (fa_k2j_ptr != fa_k2j_end)
                    {
                        const auto fa_k2j = *fa_k2j_ptr;
                        const auto fb_k2j = *fb_k2j_ptr;

                        const auto term1 = 2 * (
                            (g_AA_ij * fa_k1j * fa_k2j) +
                            (g_aa_ij * fb_k1j * fb_k2j));

                        const auto term2 = 2 * g_Aa_ij * (
                            (fa_k1j * fb_k2j) +
                            (fb_k1j * fa_k2j));

                        const auto term3 =
                            (theta * theta * fa_k1j * fa_k2j) +
                            (gamma * gamma * fb_k1j * fb_k2j);

                        const auto term4 = theta * gamma * (
                            (fa_k1j * fb_k2j) +
                            (fb_k1j * fa_k2j));

                        *h_ptr += alpha * (term1 + term2
                                - alpha * (term3 + term4));

                        fa_k2j_ptr += f_step;
                        fb_k2j_ptr += f_step;
                        h_ptr++;
                    }

                    fa_k1j_ptr += f_step;
                    fb_k1j_ptr += f_step;
                    d_ptr++;
                }

                g_AA_ij_ptr++;
                g_Aa_ij_ptr++;
                g_aa_ij_ptr++;
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
                const matrix_type & ,    ///< The Q matrix.
                const matrix_type & ,    ///< The F matrix for major alleles.
                const matrix_type & ,    ///< The F matrix for minor alleles.
                const matrix_type & qfa, ///< The Q*Fa product.
                const matrix_type & qfb) ///< The Q*Fb product.
                const override
        {
            assert(qfa.is_size(qfb));
            assert(qfa.is_size(get_height(), get_width()));

            auto       g_aa_ij_ptr = _g_aa.get_data();
            auto       g_Aa_ij_ptr = _g_Aa.get_data();
            auto       g_AA_ij_ptr = _g_AA.get_data();
            auto       qfa_ij_ptr  = qfa.get_data();
            auto       qfb_ij_ptr  = qfb.get_data();
            const auto qfb_ij_end  = qfb_ij_ptr + qfb.get_length();

            auto sum = value_type(0);

            while (qfb_ij_ptr != qfb_ij_end) // loop over all of Q x F (G)
            {
                const auto qfa_ij = *qfa_ij_ptr;
                const auto qfb_ij = *qfb_ij_ptr;

                sum += std::log(
                            (*g_AA_ij_ptr * qfa_ij * qfa_ij) +
                            (*g_aa_ij_ptr * qfb_ij * qfb_ij) +
                            (*g_Aa_ij_ptr * qfa_ij * qfb_ij * value_type(2)));

                g_AA_ij_ptr++;
                g_Aa_ij_ptr++;
                g_aa_ij_ptr++;
                qfa_ij_ptr++;
                qfb_ij_ptr++;
            }

            return sum;
        }

        ///
        /// \return A new mu matrix.
        ///
        virtual matrix_type create_mu(
            const value_type f_epsilon) ///< The F matrix boundary epsilon.
            const override
        {
            static const auto em_iterations = size_t(100);
            static const auto em_epsilon    = value_type(1.0e-6);

            const auto f_min = value_type(0.0) + f_epsilon;
            const auto f_max = value_type(1.0) - f_epsilon;

            const auto I = get_height();
            const auto J = get_width();

            matrix_type mu (J, 1);

            for (size_t j = 0; j < J; j++)
            {
                auto & mu_j = mu[j] = value_type(0.5);

                for (auto iter = em_iterations; iter > 0; iter--)
                {
                    const auto wu_j = value_type(1.0) - mu_j;
                    auto       sum  = value_type(0.0);

                    for (size_t i = 0; i < I; i++)
                    {
                        const auto AA = _g_AA(i, j) * mu_j * mu_j;
                        const auto aa = _g_aa(i, j) * wu_j * wu_j;
                        const auto Aa = _g_Aa(i, j) * mu_j * wu_j
                                      * value_type(2.0);

                        sum += (value_type(2.0) * AA + Aa)
                             / (value_type(2.0) * (AA + Aa + aa));
                    }

                    const auto previous_mu_j = mu_j;
                    mu_j = std::min(std::max(
                        f_min,
                        sum / value_type(I)),
                        f_max);

                    if (std::fabs(previous_mu_j - mu_j) <= em_epsilon)
                        break;
                }
            }

            return mu;
        }

        ///
        /// \return The height of the matrix.
        ///
        inline virtual size_t get_height() const override
        {
            return _g_aa.get_height();
        }

        ///
        /// \return The major-major base matrix.
        ///
        inline const matrix_type & get_major_major_matrix() const
        {
            return _g_AA;
        }

        ///
        /// \return The major-minor base matrix.
        ///
        inline const matrix_type & get_major_minor_matrix() const
        {
            return _g_Aa;
        }

        ///
        /// \return The minor-minor base matrix.
        ///
        inline const matrix_type & get_minor_minor_matrix() const
        {
            return _g_aa;
        }

        ///
        /// \return The string representation of the size of the matrix.
        ///
        inline virtual std::string get_size_str() const override
        {
            return _g_aa.get_size_str();
        }

        ///
        /// \return The width of the matrix.
        ///
        inline virtual size_t get_width() const override
        {
            return _g_aa.get_width();
        }

        ///
        /// \return The string representation of the matrix.
        ///
        inline virtual std::string str() const override
        {
            std::ostringstream out;
            out << _g_aa << '\n' << _g_Aa << '\n' << _g_AA;
            return out.str();
        }

    private:
        matrix_type _g_aa; // minor-minor
        matrix_type _g_Aa; // major-minor
        matrix_type _g_AA; // major-major

        // --------------------------------------------------------------------
        inline void _validate_sizes()
        {
            const auto h = _g_AA.get_height();
            const auto w = _g_AA.get_width();

            const auto is_size_mismatch = false
                    || h != _g_Aa.get_height()
                    || h != _g_aa.get_height()
                    || w != _g_Aa.get_width()
                    || w != _g_aa.get_width();

            if (is_size_mismatch)
                throw error("inconsistent matrix_type sizes in "
                            "likelihood genotype matrix_type.");
        }

        // --------------------------------------------------------------------
        struct qf_pair
        {
            value_type qfa_ij; // Q * F     at i,j
            value_type qfb_ij; // Q * (1-F) at i,j

            // ----------------------------------------------------------------
            inline qf_pair(
                    const value_type qfa_ij_,
                    const value_type qfb_ij_)
                : qfa_ij (qfa_ij_)
                , qfb_ij (qfb_ij_)
            {
            }

            // ----------------------------------------------------------------
            qf_pair(
                    const size_t        i, // individual
                    const size_t        j, // marker
                    const matrix_type & q,
                    const matrix_type & fa,
                    const matrix_type & fb)
                : qfa_ij (0)
                , qfb_ij (0)
            {
                assert(q.get_width() == fa.get_height());
                assert(fa.is_size(fb));
                assert(i < q.get_height());
                assert(j < fa.get_width());

                const auto J = fa.get_width();
                const auto K = fa.get_height();

                auto       fa_mj_ptr = fa.get_data(0, j);
                auto       fb_mj_ptr = fb.get_data(0, j);
                auto       q_im_ptr  = q.get_data(i, 0);
                const auto q_im_end  = q_im_ptr + K;

                while (q_im_ptr != q_im_end) // loop over m (0 < m < K)
                {
                    const auto q_im  = *q_im_ptr;
                    const auto fa_mj = *fa_mj_ptr;
                    const auto fb_mj = *fb_mj_ptr;

                    qfa_ij += q_im * fa_mj;
                    qfb_ij += q_im * fb_mj;

                    fa_mj_ptr += J; // stride is J across column of F
                    fb_mj_ptr += J;
                    q_im_ptr++;     // stride is 1 across rows of Q
                }
            }

            // ----------------------------------------------------------------
            value_type compute_f_ij(
                    const value_type g_aa_ij,
                    const value_type g_Aa_ij,
                    const value_type g_AA_ij)
                    const
            {
                auto f_ij = value_type(0);

                f_ij += g_AA_ij * qfa_ij * qfa_ij;
                f_ij += g_aa_ij * qfb_ij * qfb_ij;
                f_ij += g_Aa_ij * qfa_ij * qfb_ij * value_type(2);

                return f_ij;
            }

            // ----------------------------------------------------------------
            value_type compute_g_f_ijk(
                    const value_type g_aa_ij,
                    const value_type g_Aa_ij,
                    const value_type g_AA_ij,
                    const value_type q_ik)
                    const
            {
                auto g_f_ijk = value_type(0);

                g_f_ijk += value_type(2) * g_AA_ij * q_ik * qfa_ij;
                g_f_ijk -= value_type(2) * g_aa_ij * q_ik * qfb_ij;
                g_f_ijk += value_type(2) * g_Aa_ij * (
                    (qfb_ij * q_ik) - (qfa_ij * q_ik));

                return g_f_ijk;
            }

            // ----------------------------------------------------------------
            value_type compute_g_q_ijk(
                    const value_type g_aa_ij,
                    const value_type g_Aa_ij,
                    const value_type g_AA_ij,
                    const value_type fa_kj,
                    const value_type fb_kj)
                    const
            {
                auto g_q_ijk = value_type(0);

                g_q_ijk += value_type(2) * g_AA_ij * fa_kj * qfa_ij;
                g_q_ijk += value_type(2) * g_aa_ij * fb_kj * qfb_ij;
                g_q_ijk += value_type(2) * g_Aa_ij * (
                    (qfa_ij * fb_kj) + (qfb_ij * fa_kj));

                return g_q_ijk;
            }
        };
    };
}

#endif // JADE_LIKELIHOOD_GENOTYPE_MATRIX_HPP__
