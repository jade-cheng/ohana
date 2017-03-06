/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SETTINGS_HPP__
#define JADE_SETTINGS_HPP__

#include "jade.agi_reader.hpp"
#include "jade.genotype_matrix_factory.hpp"
#include "jade.options.hpp"

namespace jade
{
    ///
    /// A template for a class that prepares settings for the algorithm.
    ///
    template <typename TValue>
    class basic_settings
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The admixture graph input reader.
        typedef basic_agi_reader<value_type> agi_reader_type;

        /// A pointer to an admixture graph input reader.
        typedef std::unique_ptr<agi_reader_type> agi_ptr_type;

        /// The genotype matrix factory type.
        typedef basic_genotype_matrix_factory<value_type>
            genotype_matrix_factory_type;

        /// The genotype matrix type.
        typedef basic_genotype_matrix<value_type> genotype_matrix_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The options type.
        typedef basic_options<value_type> options_type;

        /// The verification type.
        typedef basic_verification<value_type> verification_type;

        ///
        /// Initializes a new instance of the class based on the specified
        /// command-line arguments.
        ///
        explicit basic_settings(
                args & a) ///< The command-line arguments.
            : _opts (a)
            , _agi  ()
            , _f    ()
            , _rf   ()
            , _c    ()
            , _mu   ()
        {
            if (_opts.is_ain_specified())
                _agi.reset(new agi_reader_type(_opts.get_ain()));

            const std::unique_ptr<genotype_matrix_type> g_ptr (
                genotype_matrix_factory_type::create(a.pop<std::string>()));
            const auto & g = *g_ptr;
            verification_type::validate_g(g);

            _f.read(a.pop<std::string>());
            verification_type::validate_f(_f);

            a.validate_empty();

            openblas_set_num_threads(int(_opts.get_num_threads()));

            _rf = create_rf(_f);
            _mu = g.create_mu();

            //
            // If the user specifies the C matrix, read it from the file; if
            // the user specifies a tree or the AGI file, do not initialize its
            // values because they will be initialized by the optimizer based
            // on the tree or AGI definitions. If the C matrix, the tree, and
            // the AGI file are unspecified, then create it as the sample
            // covariance matrix of the rooted F matrix.
            //
            if (_opts.is_cin_specified())
            {
                _c.read(_opts.get_cin());
                verification_type::validate_c(_c);
                verification_type::validate_fc_sizes(_f, _c);
            }
            else if (_opts.is_tin_specified())
            {
                const auto rk = _rf.get_height();
                _c.resize(rk, rk);
            }
            else if (_opts.is_ain_specified())
            {
                const auto agi_k = _agi->get_k();
                const auto f_k   = _f.get_height();

                if (agi_k != f_k)
                    throw jade::error()
                        << "inconsistent population sizes " << agi_k << " and "
                        << f_k << " in AGI file and F matrix.";

                const auto rk = _rf.get_height();
                _c.resize(rk, rk);
            }
            else
            {
                _c = create_c(_rf, _mu);
            }

            verification_type::validate_gf_sizes(g, _f);
        }

        ///
        /// \return The admixture graph input reader.
        ///
        inline const agi_reader_type & get_agi() const
        {
            assert(nullptr != _agi.get());
            return *_agi;
        }

        ///
        /// \return The C matrix.
        ///
        inline const matrix_type & get_c() const
        {
            return _c;
        }

        ///
        /// \return The mu vector.
        ///
        inline const matrix_type & get_mu() const
        {
            return _mu;
        }

        ///
        /// \return The options.
        ///
        inline const options_type & get_options() const
        {
            return _opts;
        }

        ///
        /// \return The rooted F matrix.
        ///
        inline const matrix_type & get_rf() const
        {
            return _rf;
        }

        // --------------------------------------------------------------------
        static matrix_type create_c(
                const matrix_type & rf, ///< The [RK x 1] rooted F matrix.
                const matrix_type & mu) ///< The [J x 1] sample allele freq.
        {
            const auto RK = rf.get_height();
            const auto J  = rf.get_width();

            assert(J > 1);
            assert(mu.is_size(J, 1));

            static const auto n1  = value_type(1.0);
            const auto        nj  = static_cast<value_type>(J);
            const auto        nj1 = static_cast<value_type>(J - 1);

            //
            // Create a vector of average row values for the RF matrix.
            //
            matrix_type rf_avg (RK, 1);
            for (size_t rk = 0; rk < RK; rk++)
                rf_avg[rk] = rf.get_row_sum(rk) / nj;

            //
            // The covariance matrix is [K-1 x K-1].
            //
            matrix_type c (RK, RK);

            //
            // Calculate the lower triangle of sample covariance of the RF
            // matrix and divide it by the mux.
            //
            for (size_t j = 0; j < J; j++)
            {
                const auto mu_j = mu[j];
                const auto s_j  = n1 / (mu_j * (n1 - mu_j)) / nj1;

                for (size_t row = 0; row < RK; row++)
                {
                    const auto s_row = s_j * (rf(row, j) - rf_avg[row]);
                    for (size_t col = 0; col <= row; col++)
                        c(row, col) += s_row * (rf(col, j) - rf_avg[col]);
                }
            }

            //
            // Copy the lower triangle to the upper triangle and return the
            // covariance matrix.
            //
            c.copy_lower_to_upper();
            return c;
        }

        // --------------------------------------------------------------------
        static matrix_type create_rf(const matrix_type & f)
        {
            const auto K = f.get_height();
            const auto J = f.get_width();

            assert(K > 1);

            const auto RK = K - 1;

            matrix_type rf (RK, J);

            for (size_t rk = 0; rk < RK; rk++)
                for (size_t j = 0; j < J; j++)
                    rf(rk, j) = f(rk + 1, j) - f(0, j);

            return rf;
        }

        options_type _opts;
        agi_ptr_type _agi;
        matrix_type  _f;
        matrix_type  _rf;
        matrix_type  _c;
        matrix_type  _mu;
    };
}

#endif // JADE_SETTINGS_HPP__
