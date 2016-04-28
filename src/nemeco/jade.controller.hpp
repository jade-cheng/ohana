/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_CONTROLLER_HPP__
#define JADE_CONTROLLER_HPP__

#include "jade.likelihood.hpp"
#include "jade.settings.hpp"
#include "jade.simplex.hpp"
#include "jade.stopwatch.hpp"

namespace jade
{
    ///
    /// A template for a class that performs the Nelder-Mead optimization. This
    /// class implements shared features of the two concrete controllers of this
    /// module, one that uses a Newick tree as input and another that does not.
    ///
    template <typename TValue>
    class basic_controller
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The likelihood type.
        typedef basic_likelihood<value_type> likelihood_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The options type.
        typedef basic_options<value_type> options_type;

        /// The settings type.
        typedef basic_settings<value_type> settings_type;

        /// The simplex type.
        typedef basic_simplex<value_type> simplex_type;

        /// The container type for the simplex.
        typedef typename simplex_type::container_type container_type;

        /// The exit condition for the simplex.
        typedef typename simplex_type::exit_condition_type exit_condition_type;

        /// The loc arguments for the simplex.
        typedef typename simplex_type::log_args log_args_type;

        ///
        /// Reclaims resources, if any, used by the instance.
        ///
        inline virtual ~basic_controller()
        {
        }

        ///
        /// Computes the objective function by decoding the specified
        /// Nelder- Mead parameters into a covariance matrix, performing a
        /// Cholskey square root, calculating the determinant, computing the
        /// inverse, and calling the likelihood function.
        ///
        /// \return The negation of the log likelihood.
        ///
        value_type compute_objfunc(
                const container_type & params) ///< The parameters.
        {
            static const auto inf = std::numeric_limits<value_type>::max();

            //
            // No branch length may be negative; if one is, return infinity
            // to reject these parameters.
            //
            for (const auto n : params)
                if (n <= value_type(0))
                    return inf;

            //
            // The LAPACK routines need only the lower triangle stored in
            // the matrix. If these routines are successful, the lower
            // triangle is copied to the upper triangle before calculating
            // the likelihood.
            //
            _decode_lower(_c, params);

            //
            // Compute the Cholskey square root. If it fails, the matrix is
            // not positive definite, so return infinity to indicate this is
            // an unacceptable set of parameters.
            //
            if (!_c.potrf_lower())
                return inf;

            //
            // Calculate the log of the determinant by summing twice the log of
            // the diagonal entries.
            //
            auto log_c_det = value_type(0);
            {
                const auto end = _c.get_data() + _c.get_length() + _rk;
                for (auto ptr = _c.get_data(); ptr != end; ptr += _rk + 1)
                    log_c_det += value_type(2) * std::log(*ptr);
            }

            //
            // Compute the inverse. If it fails, return infinity to indicate
            // this is an unacceptable set of parameters.
            //
            if (!_c.potri_lower())
                return inf;

            //
            // Mirror the values from the lower triangle to the upper triangle.
            //
            _c.copy_lower_to_upper();

            //
            // The Nelder-Mead algorithm minimizes the objective function, so
            // return the negation of the log-likelihood function.
            //
            return -_likelihood(_c, log_c_det);
        }

        ///
        /// Writes results to standard output and files.
        ///
        virtual void emit_results(
                const options_type &        opts,    ///< The options.
                const simplex_type &        simplex, ///< The simplex.
                const exit_condition_type &)         ///< The context.
        {
            //
            // Decode the final set of parameters (the optimized matrix).
            //
            _decode_lower(_c, simplex.get_vertex());
            _c.copy_lower_to_upper();

            //
            // Display the log-likelihood, which is the negative of the last
            // value from the objective function.
            //
            std::cout
                << "\nlog likelihood = "
                << -simplex.get_objval()
                << std::endl;

            //
            // Display or save the covariance matrix.
            //
            if (opts.is_cout_specified())
            {
                const auto & cout = opts.get_cout();
                std::cout << "Writing C matrix to " << cout << std::endl;

                std::ofstream out (cout);
                if (!out.good())
                    throw error() << "failed to create matrix '" << cout << "'";

                matrix_type::set_high_precision(out);
                out << _c;
            }
            else
            {
                std::cout << "[C Matrix]\n";
                matrix_type::set_high_precision(std::cout);
                std::cout << _c << std::endl;
            }
        }

        ///
        /// \return A reference to the covariance matrix.
        ///
        inline const matrix_type & get_c() const
        {
            return _c;
        }

        ///
        /// \return The rooted K value for this instance.
        ///
        inline size_t get_rk() const
        {
            return _rk;
        }

        ///
        /// Creates and returns the initial set of parameters for the Nelder-
        /// Mead algorithm.
        ///
        /// \return The initial parameters for the Nelder-Mead algorithm.
        ///
        virtual container_type init_parameters() = 0;

        ///
        /// Logs information about the specified context for one iteration of
        /// the Nelder-Mead algorithm.
        ///
        void log_iteration(
                const log_args_type & log_args) ///< The Nelder-Mead arguments.
        {
            const auto lle = -log_args.simplex->get_objval();

            const auto dlle = log_args.iteration == 1
                ? value_type(0)
                : lle - _lle;

            std::ostringstream line;
            line << log_args.iteration
                 << std::fixed << std::setprecision(6)
                 << '\t' << _iteration_time;
            matrix_type::set_high_precision(line);
            line << '\t' << dlle << '\t' << lle;
            std::cout << line.str() << std::endl;

            _lle = lle;
            _iteration_time = stopwatch();
        }

    protected:
        ///
        /// Initializes a new instance of the class based on the specified
        /// settings.
        ///
        explicit basic_controller(
                const settings_type & settings) ///< The algorithm settings.
            : _rk             (settings.get_rf().get_height())
            , _c              (settings.get_c())
            , _lle            (0)
            , _likelihood     (settings.get_rf(), settings.get_mu())
            , _iteration_time ()
        {
            assert(_rk > 0);
            assert(_c.is_size(_rk, _rk));
        }

        ///
        /// Decodes the specified Nelder-Mead container and stores the result
        /// into the lower triangle, including the diagonal, of the covariance
        /// matrix.
        ///
        virtual void _decode_lower(
                matrix_type &          dst, ///< The covariance matrix.
                const container_type & src) ///< The Nelder-Mead container.
                = 0;

    private:
        size_t          _rk;             // The rooted K value; i.e. K - 1.
        matrix_type     _c;              // The covariance matrix.
        value_type      _lle;            // The log-likelihood value.
        likelihood_type _likelihood;     // The likelihood functor.
        stopwatch       _iteration_time; // The time elapsed per iteration.
    };
}

#endif // JADE_CONTROLLER_HPP__
