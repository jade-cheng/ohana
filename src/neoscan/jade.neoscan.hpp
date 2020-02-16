/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_NEOSCAN_HPP__
#define JADE_NEOSCAN_HPP__

#include "jade.args.hpp"
#include "jade.genotype_matrix_factory.hpp"

namespace jade
{
    ///
    /// A template for a class implementing the neoscan algorithm.  This
    /// algorithm scans for positive selection between ancient and modern data.
    /// It can take advantage of the dating for each individual ancient sample.
    ///
    template <typename TValue>
    class basic_neoscan
    {
        basic_neoscan() = delete;
        basic_neoscan(const basic_neoscan &) = delete;
        basic_neoscan & operator = (const basic_neoscan &) = delete;

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
            basic_genotype_matrix_factory<value_type>
            genotype_matrix_factory_type;

        ///
        /// Output for one column.
        ///
        struct output
        {
            value_type delta;      ///< The delta.
            value_type global_lle; ///< The global lle.
            value_type local_lle;  ///< The local lle.

            ///
            /// \return The computed log-likelihood ratio, i.e.
            ///         2.0 * (local_lle - global_lle).
            ///
            value_type compute_lle_ratio() const
            {
                return value_type(2.0) * (local_lle - global_lle);
            }

            ///
            /// Returns the delta, global_lle, local_lle, and computed
            /// log-likelihood ratio converted to high-precision strings and
            /// joined by the tab character. The returned string does not
            /// include and end-of-line character.
            ///
            /// \return A string representing the output values.
            ///
            std::string to_string() const
            {
                std::ostringstream out;
                matrix_type::set_high_precision(out);
                out << std::showpos
                    << delta      << '\t'
                    << global_lle << '\t'
                    << local_lle  << '\t'
                    << compute_lle_ratio();
                return out.str();
            }
        };

        ///
        /// Initializes a new instance of the class. Memory for the specified
        /// G, Q, and Fa matrices must remain valid for the lifetime of this
        /// instance.
        ///
        basic_neoscan(
                const genotype_matrix_type & g,     ///< The G matrix.
                const matrix_type          & q,     ///< The Q matrix.
                const matrix_type          & f,     ///< The F matrix.
                const matrix_type          & years) ///< The years matrix.
            : _g     (g)
            , _q     (q)
            , _f     (f)
            , _y     (_init_y(years, q))
            , _f_j   (f.get_height(), 1)
        {
            verification_type::validate_gqf_sizes(g, q, f);
        }

        ///
        /// Executes the algorithm using the specified action for the output
        /// of each column.
        ///
        /// \param output_action The action to perform for the output; this
        ///                      function or lambda must take one argument of
        ///                      type, output.
        ///
        template <typename TOutputAction>
        void execute(const TOutputAction & output_action) const
        {
            const auto J = _g.get_width();

            for (size_t j = 0; j < J; j++)
            {
                value_type col_min = 0;
                value_type col_max = 0;
                _f.get_min_max_column(j, col_min, col_max);

                const auto range_low  = -col_max;
                const auto range_high = value_type(1.0) - col_min;

                output out;
                out.delta      = value_type(0.0);
                out.global_lle = _compute_lle_j(j, out.delta);
                out.local_lle  = out.global_lle;

                static const auto tol = value_type(0.000001);
                static const auto phi = value_type(0.5 * (sqrt(5.0) + 1.0));

                const auto dr_phi = (range_high - range_low) / phi;

                auto a = range_low;
                auto b = range_high;
                auto c = range_high - dr_phi;
                auto d = range_low  + dr_phi;

                while (std::abs(c - d) > tol)
                {
                    if (_compute_lle_j(j, c) > _compute_lle_j(j, d))
                        b = d;
                    else
                        a = c;

                    c = b - (b - a) / phi;
                    d = a + (b - a) / phi;
                }

                const auto gss_delta = value_type(0.5) * (a + b);
                const auto gss_lle   = _compute_lle_j(j, gss_delta);

                if (gss_lle > out.local_lle)
                {
                    out.delta     = gss_delta;
                    out.local_lle = gss_lle;
                }

                output_action(out);
            }
        }

        ///
        /// Runs the program based on command-line arguments.
        /// \param a The command-line arguments.
        ///
        static void run(args & a)
        {
            const std::unique_ptr<genotype_matrix_type> g_ptr (
                genotype_matrix_factory_type::create(a.pop<std::string>()));

            const matrix_type q (a.pop<std::string>());
            const matrix_type f (a.pop<std::string>());
            const matrix_type y (a.pop<std::string>());
            a.validate_empty();

            std::cout << "d\tglobal_lle\tlocal_lle\tlle_ratio\n";

            const basic_neoscan neoscan (*g_ptr, q, f, y);

            neoscan.execute([](const output & out)
            {
                std::cout << out.to_string() << std::endl;
            });
        }

    private:
        typedef basic_discrete_genotype_matrix<value_type>   dgm_type;
        typedef basic_likelihood_genotype_matrix<value_type> lgm_type;

        // --------------------------------------------------------------------
        void _compute_ab_ij(
                const size_t   i,
                value_type   & a_ij,
                value_type   & b_ij)
                const
        {
            assert(i < _y.get_length());

            const auto K = _f_j.get_length();

            auto       q_src   = _q.get_data(i, 0);
            auto       f_j_src = _f_j.get_data();
            const auto f_j_end = _f_j.get_data() + K;

            a_ij = value_type(0.0);
            b_ij = value_type(0.0);

            while (f_j_src != f_j_end)
            {
                const auto q_ik = *q_src++;
                const auto f_kj = *f_j_src++;

                a_ij += q_ik * f_kj;
                b_ij += q_ik * (value_type(1.0) - f_kj);
            }
        }

        // --------------------------------------------------------------------
        void _compute_f_j(
                const size_t     j,
                const size_t     i,
                const value_type d)
                const
        {
            static const auto epsilon       = value_type(1.0e-6);
            static const auto lower_epsilon = value_type(0.0) + epsilon;
            static const auto upper_epsilon = value_type(1.0) - epsilon;

            const auto J = _f.get_width();
            const auto K = _f.get_height();

            assert(j < J);
            assert(i < _y.get_height());

            auto       src_ptr = _f.get_data(0, j);
            auto       dst_ptr = _f_j.get_data();
            const auto dst_end = _f_j.get_data() + K;
            const auto dy      = d * _y[i];

            while (dst_ptr != dst_end)
            {
                *dst_ptr++ = std::min(std::max(
                      lower_epsilon,
                      *src_ptr + dy),
                      upper_epsilon);
                src_ptr += J;
            }
        }

        // --------------------------------------------------------------------
        value_type _compute_lle_j(const size_t j, const value_type d) const
        {
            const auto I = _q.get_height();

            assert(d >= value_type(-1.0) && d <= value_type(+1.0));
            assert(j < _g.get_width());

            if (_g.is_dgm())
            {
                const auto & g = _g.to_dgm().get_matrix();

                auto lle_all = value_type(0.0);

                for (size_t i = 0; i < I; i++)
                {
                    value_type g_ij;
                    if (!_try_convert(g(i, j), g_ij))
                        continue;

                    _compute_f_j(j, i, d);

                    value_type a_ij, b_ij;
                    _compute_ab_ij(i, a_ij, b_ij);

                    lle_all += std::log(a_ij) * g_ij;
                    lle_all += std::log(b_ij) * (value_type(2.0) - g_ij);
                }

                return lle_all;
            }

            if (_g.is_lgm())
            {
                const auto & g    = _g.to_lgm();
                const auto & g_AA = g.get_major_major_matrix();
                const auto & g_Aa = g.get_major_minor_matrix();
                const auto & g_aa = g.get_minor_minor_matrix();

                auto lle_all = value_type(0.0);

                for (size_t i = 0; i < I; i++)
                {
                    if (_y[i] < value_type(0.0))
                        continue;

                    const auto g_AA_ij = g_AA(i, j);
                    const auto g_Aa_ij = g_Aa(i, j);
                    const auto g_aa_ij = g_aa(i, j);

                    _compute_f_j(j, i, d);

                    value_type a_ij, b_ij;
                    _compute_ab_ij(i, a_ij, b_ij);

                    lle_all += std::log(
                                (g_AA_ij * a_ij * a_ij) +
                                (g_aa_ij * b_ij * b_ij) +
                                (g_Aa_ij * a_ij * b_ij * value_type(2.0)));
                }

                return lle_all;
            }

            throw jade::error("unsupported genotype matrix");
        }

        // --------------------------------------------------------------------
        static matrix_type _init_y(
            const matrix_type & years, ///< The years (loaded from a file).
            const matrix_type & q)     ///< The Q matrix.
        {
            if (!years.is_column_vector())
                throw jade::error()
                    << "invalid years matrix has "
                    << years.get_width()
                    << " columns; expected column vector";

            const auto I = q.get_height();
            assert(I > 0);

            if (years.get_height() != I)
                throw jade::error()
                    << "inconsistent number of years specified ("
                    << years.get_height()
                    << "); expected height of Q matrix (" << I << ")";

            matrix_type out (I, 1);

            const auto max_value = years.get_max_value();
            const auto min_value = years.get_min_value();
            const auto avg_value = years.get_sum() / value_type(I);
            const auto years_end = years.get_data() + I;
            auto       years_ptr = years.get_data();
            auto       out_ptr   = out.get_data();

            while (years_ptr != years_end)
            {
                const auto y = *years_ptr++;
                *out_ptr++ = y < value_type(0.0)
                    ? value_type(1.0)
                    : (avg_value - y) / std::max(max_value - y, y - min_value);
            }

            return out;
        }

        // --------------------------------------------------------------------
        static bool _try_convert(const genotype g, value_type & out)
        {
            switch (g)
            {
            case jade::genotype_major_major:
                out = value_type(0.0);
                return true;

            case jade::genotype_major_minor:
                out = value_type(1.0);
                return true;

            case jade::genotype_minor_minor:
                out = value_type(2.0);
                return true;

            default:
                return false;
            }
        }

        const genotype_matrix_type & _g;     // [I x J]
        const matrix_type          & _q;     // [I x K]
        const matrix_type          & _f;     // [K x J]
        const matrix_type            _y;     // [I x 1]

        // scratch space
        mutable matrix_type _f_j; // [K x 1]
    };
}

#endif // JADE_NEOSCAN_HPP__
