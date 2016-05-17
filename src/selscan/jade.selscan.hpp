/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SELSCAN_HPP__
#define JADE_SELSCAN_HPP__

#include "jade.args.hpp"
#include "jade.genotype_matrix_factory.hpp"

namespace jade
{
    ///
    /// A template for a class implementing the selscan program.
    ///
    template <typename TValue>
    class basic_selscan
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The verification type.
        typedef basic_verification<value_type> verification_type;

        /// The genotype matrix type.
        typedef basic_genotype_matrix<value_type> genotype_matrix_type;

        /// The genotype matrix factory type.
        typedef
            basic_genotype_matrix_factory<TValue>
            genotype_matrix_factory_type;

        ///
        /// Initializes a new instance of the class.
        ///
        explicit basic_selscan(
                args & a) ///< The command-line arguments.
            : steps     (a.read("--steps", "-s", size_t(10)))
            , g_ptr     (genotype_matrix_factory_type::create(a.pop<std::string>()))
            , g         (*g_ptr)
            , fa        (a.pop<std::string>())
            , c1        (a.pop<std::string>())
            , c2        (_init_scaling_matrix(a, c1))
            , RK        (c1.get_width())
            , J         (g.get_width())
            , mu        (g.create_mu())
            , rooted_fa (_compute_rooted_fa(fa))
            , c_inv     (RK, RK)
            , f_j_c_inv (RK, 1)
        {
            if (steps < 2)
                throw error() << "invalid value for --steps option ("
                              << steps << "); expected at least two steps";

            a.validate_empty();

            verification_type::validate_g(g);
            verification_type::validate_f(fa);
            verification_type::validate_c(c1);
            verification_type::validate_c(c2);
            verification_type::validate_gf_sizes(g, fa);
            verification_type::validate_fc_sizes(fa, c1);
            verification_type::validate_fc_sizes(fa, c2);
        }

        ///
        /// Executes the program.
        ///
        void execute()
        {
            std::cout << "step\tglobal-lle\tlocal-lle\tlle-ratio\n";

            std::vector<record> records;
            records.reserve(J);
            for (size_t j = 0; j < J; j++)
                records.emplace_back(j, _compute_score(0, j));

            for (size_t si = 0; si < steps; si++)
                for (auto & r : records)
                    r.update(si, _compute_score(si, r.get_j()));

            for (const auto & r : records)
                std::cout << _format(r.get_step())       << '\t'
                          << _format(r.get_score())      << '\t'
                          << _format(r.get_best_score()) << '\t'
                          << _format(r.get_lle_ratio())  << '\n';
        }

    private:
        typedef std::unique_ptr<genotype_matrix_type> genotype_matrix_ptr;

        // --------------------------------------------------------------------
        static matrix_type _compute_rooted_fa(const matrix_type & fa)
        {
            const size_t K = fa.get_height();
            const size_t J = fa.get_width();

            assert(K > 1);

            matrix_type rooted_fa (K - 1, J);

            for (size_t k = 0; k + 1 < K; k++)
                for (size_t j = 0; j < J; j++)
                    rooted_fa(k, j) = fa(k + 1, j) - fa(0, j);

            return rooted_fa;
        }

        // --------------------------------------------------------------------
        value_type _compute_score(
                const size_t si,
                const size_t j)
        {
            typedef typename matrix_type::openblas_type openblas_type;

            const auto percent = value_type(si) / value_type(steps - 1);
            for (size_t i = 0; i < RK * RK; i++)
                c_inv[i] = c1[i] + percent * (c2[i] - c1[i]);

            auto log_c_det = value_type(0);
            _invert(c_inv, log_c_det);

            //
            // The ?gemv routines perform a matrix-vector operation defined as
            //
            // y := (alpha * A * x) + (beta * y)
            //
            // where alpha and beta are scalars, x and y are vectors, and A is
            // an m-by-n matrix.
            //
            openblas_type::gemv(
                CblasRowMajor,                   // Layout
                CblasNoTrans,                    // transa
                blasint(c_inv.get_height()),     // m
                blasint(c_inv.get_width()),      // n
                1.0,                             // alpha
                c_inv.get_data(),                // A
                blasint(c_inv.get_width()),      // lda
                rooted_fa.get_data() + j,        // x
                blasint(rooted_fa.get_width()),  // incx
                0.0,                             // beta
                f_j_c_inv.get_data(),            // y
                blasint(f_j_c_inv.get_width())); // yinc

            //
            // The ?dot routines perform a vector-vector reduction operation
            // defined as
            //
            // res = Sum (x_i * y_i) for i = 1 to n
            //
            // where x_i and y_i are elements of vectors x and y.
            //
            const auto dot = openblas_type::dot(
                blasint(f_j_c_inv.get_height()), // n (height = RK)
                rooted_fa.get_data() + j,        // x
                blasint(rooted_fa.get_width()),  // xinc (width = 1)
                f_j_c_inv.get_data(),            // y
                blasint(f_j_c_inv.get_width())); // yinc (width = 1)

            static const auto pi  = value_type(std::acos(-1.0));
            static const auto tau = value_type(2) * pi;

            const auto rk   = static_cast<value_type>(RK);
            const auto mu_j = mu[j];
            const auto c_j  = mu_j * (value_type(1) - mu_j);
            const auto term = (rk * std::log(tau * c_j)) + (dot / c_j);
            return -(log_c_det + term) / value_type(2);
        }

        // --------------------------------------------------------------------
        static std::string _format(const value_type value)
        {
            std::ostringstream out;
            matrix_type::set_high_precision(out);
            out << std::showpos << value;
            return out.str();
        }

        // --------------------------------------------------------------------
        static matrix_type _init_scaling_matrix(
                args &              a,
                const matrix_type & c1)
        {
            const auto path = a.read<std::string>("--c-scale", "-cs");
            return path.empty() ? value_type(2) * c1 : matrix_type(path);
        }

        // --------------------------------------------------------------------
        static bool _invert(matrix_type & c, value_type & log_c_det)
        {
            const auto rk = c.get_height();
            assert(c.is_size(rk, rk));

            //
            // Compute the Cholskey square root. If it fails, the matrix is
            // not positive definite, so return infinity to indicate this is
            // an unacceptable set of parameters.
            //
            if (!c.potrf_lower())
                return false;

            //
            // Calculate the log of the determinant by summing twice the log of
            // the diagonal entries.
            //
            log_c_det = value_type(0);
            {
                const auto end = c.get_data() + c.get_length() + rk;
                for (auto ptr = c.get_data(); ptr != end; ptr += rk + 1)
                    log_c_det += value_type(2) * std::log(*ptr);
            }

            //
            // Compute the inverse. If it fails, return infinity to indicate
            // this is an unacceptable set of parameters.
            //
            if (!c.potri_lower())
                return false;

            //
            // Mirror the values from the lower triangle to the upper triangle.
            //
            c.copy_lower_to_upper();
            return true;
        }

        // --------------------------------------------------------------------
        class record
        {
        public:
            // ----------------------------------------------------------------
            inline record(const size_t j, const value_type score)
                : _best_score (std::numeric_limits<value_type>::lowest())
                , _j          (j)
                , _lle_ratio  (std::numeric_limits<value_type>::quiet_NaN())
                , _score      (score)
                , _step       (std::numeric_limits<size_t>::max())
            {
            }

            // ----------------------------------------------------------------
            inline value_type get_best_score() const
            {
                return _best_score;
            }

            // ----------------------------------------------------------------
            inline size_t get_j() const
            {
                return _j;
            }

            // ----------------------------------------------------------------
            inline value_type get_lle_ratio() const
            {
                return _lle_ratio;
            }

            // ----------------------------------------------------------------
            inline value_type get_score() const
            {
                return _score;
            }

            // ----------------------------------------------------------------
            inline size_t get_step() const
            {
                return _step;
            }

            // ----------------------------------------------------------------
            inline void update(const size_t step, const value_type score)
            {
                if (score <= _best_score)
                    return;

                _step       = step;
                _best_score = score;
                _lle_ratio  = value_type(2) * (score - _score);
            }

        private:
            value_type _best_score;
            size_t     _j;
            value_type _lle_ratio;
            value_type _score;
            size_t     _step;
        };

        const size_t                 steps;
        const genotype_matrix_ptr    g_ptr;
        const genotype_matrix_type & g;
        const matrix_type            fa;
        const matrix_type            c1;
        const matrix_type            c2;
        const size_t                 RK;
        const size_t                 J;
        const matrix_type            mu;
        const matrix_type            rooted_fa;
        matrix_type                  c_inv;
        matrix_type                  f_j_c_inv;
    };
}

#endif // JADE_SELSCAN_HPP__
