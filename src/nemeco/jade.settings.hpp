/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SETTINGS_HPP__
#define JADE_SETTINGS_HPP__

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
            , _f    ()
            , _rf   ()
            , _c    ()
            , _mu   ()
        {
            const std::unique_ptr<genotype_matrix_type> g_ptr (
                genotype_matrix_factory_type::create(a.pop<std::string>()));
            const auto & g = *g_ptr;
            verification_type::validate_g(g);

            _f.read(a.pop<std::string>());
            verification_type::validate_f(_f);

            a.validate_empty();

            if (_opts.is_num_threads_specified())
                openblas_set_num_threads(int(_opts.get_num_threads()));

            _rf = _create_rf(_f);
            _mu = g.create_mu();

            //
            // If the user specifies the C matrix, read it from the file; if the
            // user specifies a tree, do not initialize the C matrix because it
            // will be created (by the optimizer) based on the tree. If neither
            // the C matrix nor the tree are specified, then create it based on
            // the rooted F matrix.
            //
            if (_opts.is_cin_specified())
            {
                _c.read(_opts.get_cin());
                verification_type::validate_c(_c);
                verification_type::validate_fc_sizes(_f, _c);
            }
            else if (!_opts.is_tin_specified())
            {
                _c = _create_c(_rf);
            }

            verification_type::validate_gf_sizes(g, _f);
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

    private:
        // --------------------------------------------------------------------
        static matrix_type _create_c(const matrix_type & rf)
        {
            const auto RK = rf.get_height();
            const auto J  = rf.get_width();

            //
            // The covariance matrix is [K-1 x K-1].
            //
            matrix_type c (RK, RK);

            //
            // Loop over the J columns of RF; multiply each row by its
            // transpose, scale it by 1/J, and add it to the working C matrix.
            //
            typedef typename matrix_type::openblas_type openblas_type;
            const auto alpha = value_type(1) / value_type(J);
            const auto beta  = value_type(1);
            const auto rf_beg = rf.get_data();
            const auto rf_end = rf_beg + J;
            for (auto rf_ptr = rf_beg; rf_ptr != rf_end; rf_ptr++)
            {
                openblas_type::gemm(
                    CblasRowMajor, // layout
                    CblasNoTrans,  // transa
                    CblasTrans,    // transb
                    blasint(RK),   // m
                    blasint(RK),   // n
                    blasint(1),    // k
                    alpha,         // alpha
                    rf_ptr,        // a
                    blasint(J),    // lda
                    rf_ptr,        // b
                    blasint(J),    // ldb
                    beta,          // beta,
                    c.get_data(),  // c
                    blasint(RK));  // ldc
            }

            return c;
        }

        // --------------------------------------------------------------------
        static matrix_type _create_rf(const matrix_type & f)
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
        matrix_type  _f;
        matrix_type  _rf;
        matrix_type  _c;
        matrix_type  _mu;
    };
}

#endif // JADE_SETTINGS_HPP__
