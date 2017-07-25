/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_OPTIMIZER_HPP__
#define JADE_OPTIMIZER_HPP__

#include "jade.improver.hpp"
#include "jade.settings.hpp"
#include "jade.stopwatch.hpp"

namespace jade
{
    ///
    /// A template for a class that optimizes the Q and F matrices.
    ///
    template <typename TValue>
    class basic_optimizer
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The options type.
        typedef basic_options<value_type> options_type;

        /// The settings type.
        typedef basic_settings<value_type> settings_type;

        /// The improver type.
        typedef basic_improver<value_type> improver_type;

        ///
        /// Executes the optimization process.
        ///
        static void execute(
                const settings_type & settings, ///< The settings.
                matrix_type &         q0,       ///< The initial Q matrix.
                matrix_type &         f0)       ///< The initial F matrix.
        {
            //
            // Release memory for the initial Q and F matrices.
            //
            matrix_type q, fa;
            q0.swap(q);
            f0.swap(fa);

            const auto & opts = settings.get_options();
            const auto   fg   = settings.get_fg();
            const auto   fif  = settings.get_fif();
            const auto & g    = settings.get_g();

            const stopwatch sw1;

            matrix_type fb (fa.get_height(), fa.get_width());
            _compute_fb(fa, fb);

            matrix_type qfa = q * fa;
            matrix_type qfb = q * fb;

            auto lle = g.compute_lle(q, fa, fb, qfa, qfb);
            _emit_header(settings, sw1, lle);

            for (size_t iter = 1;; iter++)
            {
                if (opts.is_max_iterations_specified())
                    if (iter > opts.get_max_iterations())
                        break;

                if (opts.is_max_time_specified())
                    if (sw1 > opts.get_max_time())
                        break;

                const stopwatch sw2;

                if (!opts.is_fixed_q())
                {
                    q = improver_type::improve_q(g, q, fa, fb, qfa, qfb, fg);
                    matrix_type::gemm(q, fa, qfa);
                    matrix_type::gemm(q, fb, qfb);
                }

                if (!opts.is_fixed_f())
                {
                    fa = improver_type::improve_f(g, q, fa, fb, qfa, qfb, fif);
                    _compute_fb(fa, fb);
                    matrix_type::gemm(q, fa, qfa);
                    matrix_type::gemm(q, fb, qfb);
                }

                const auto lle_prime = g.compute_lle(q, fa, fb, qfa, qfb);
                const auto dlle      = lle_prime - lle;

                _emit_line(settings, sw2, iter, lle_prime, dlle);

                lle = lle_prime;

                if (opts.is_epsilon_specified())
                    if (dlle >= value_type(0) && dlle <= opts.get_epsilon())
                        break;
            }

            _emit_results(settings, q, fa);
        }

    private:
        // --------------------------------------------------------------------
        static void _compute_fb(const matrix_type & fa, matrix_type & fb)
        {
            const auto fa_end = fa.get_data() + fa.get_length();
            auto       fa_ptr = fa.get_data();
            auto       fb_ptr = fb.get_data();

            while (fa_ptr != fa_end)
                *fb_ptr++ = value_type(1) - *fa_ptr++;
        }

        // --------------------------------------------------------------------
        static void _emit_header(
                const settings_type & in,
                const stopwatch &     sw,
                const value_type      lle)
        {
            const auto & opts = in.get_options();

            if (opts.is_quiet())
                return;

            std::ostringstream line;
            line << 0 << std::fixed << std::setprecision(6)
                 << '\t' << sw;
            matrix_type::set_high_precision(line);
            line << '\t' << lle;

            std::cout
                << "seed: " << opts.get_seed() << std::endl
                << std::endl
                << "iter\tduration\tlog_likelihood\tdelta-lle" << std::endl
                << line.str() << std::endl;
        }

        // --------------------------------------------------------------------
        static void _emit_line(
                const settings_type & in,
                const stopwatch &     sw,
                const size_t          iter,
                const value_type      lle,
                const value_type      dlle)
        {
            if (in.get_options().is_quiet())
                return;

            std::ostringstream line;

            line << iter
                 << std::fixed << std::setprecision(6)
                 << '\t' << sw;
            matrix_type::set_high_precision(line);
            line << '\t' << lle << '\t' << dlle;

            std::cout << line.str() << std::endl;
        }

        // --------------------------------------------------------------------
        static void _emit_matrix(
                const matrix_type &   matrix,
                const std::string &   path,
                const settings_type & settings,
                const char * const    name)
        {
            if (path.empty())
            {
                matrix_type::set_high_precision(std::cout);
                std::cout << "[" << name << " Matrix]\n" << matrix;
                return;
            }

            if (!settings.get_options().is_quiet())
                std::cout
                    << "Writing " << name << " matrix to "
                    << path << "\n";

            std::ofstream out (path);
            if (!out.good())
                throw error() << "failed to create matrix '" << path << "'";

            matrix_type::set_high_precision(out);
            out << matrix;
        }

        // --------------------------------------------------------------------
        static void _emit_results(
                const settings_type & in,
                const matrix_type   & q,
                const matrix_type   & f)
        {
            static const std::string no_path;

            const auto & opts = in.get_options();

            if (!opts.is_quiet())
                std::cout << std::endl;

            if (!opts.is_fixed_q())
            {
                const auto qout = opts.is_qout_specified()
                    ? opts.get_qout() : no_path;
                _emit_matrix(q, qout, in, "Q");
            }

            if (!opts.is_fixed_f())
            {
                if (!opts.is_fixed_q() && !opts.is_qout_specified())
                    std::cout << std::endl;

                const auto fout = opts.is_fout_specified()
                    ? opts.get_fout() : no_path;
                _emit_matrix(f, fout, in, "F");
            }
        }
    };
}

#endif // JADE_OPTIMIZER_HPP__
