/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_OPTIMIZER_HPP__
#define JADE_OPTIMIZER_HPP__

#include "jade.controller_factory.hpp"

namespace jade
{
    ///
    /// A template for a class that optimizes specified program settings.
    ///
    template <typename TValue>
    class basic_optimizer
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The controller type.
        typedef basic_controller<value_type> controller_type;

        /// The controller factory type.
        typedef basic_controller_factory<value_type> controller_factory_type;

        /// The settings type.
        typedef basic_settings<value_type> settings_type;

        /// The simplex type.
        typedef typename controller_type::simplex_type simplex_type;

        /// The container type for the simplex.
        typedef typename simplex_type::container_type container_type;

        /// The log arguments type for the simplex.
        typedef typename simplex_type::log_args log_args_type;

        ///
        /// Executes the optimizer based on the specified settings.
        ///
        static void execute(
            const settings_type & settings) ///< The settings used to optimize.
        {
            std::cout << "iter\tduration\tdelta-lle\tlog-likelihood\n";

            const auto & opts = settings.get_options();

            //
            // Depending upon whether or not the user specified a Newick tree,
            // construct the appropriate controller for the Nelder-Mead
            // algorithm, and use it to compute the objective function.
            //
            std::unique_ptr< controller_type > ctrl (
                controller_factory_type::create(settings));
            const auto objfunc = [&](const container_type & params)
                -> value_type { return ctrl->compute_objfunc(params); };

            //
            // Initialize the Nelder-Mead algorithm.
            //
            typedef typename simplex_type::options options_type;
            options_type options (ctrl->init_parameters());
            simplex_type simplex (objfunc, options);

            //
            // Instruct the Nelder-Mead algorithm to use the settings based on
            // supplied program options.
            //
            typedef typename simplex_type::execute_args execute_args_type;
            execute_args_type execute_args;
            execute_args.user    = ctrl.get();
            execute_args.logfunc = _logfunc;
            if (opts.is_max_iterations_specified())
                execute_args.max_iterations = opts.get_max_iterations();
            if (opts.is_max_time_specified())
                execute_args.max_seconds = opts.get_max_time();
            if (opts.is_epsilon_specified())
                execute_args.min_epsilon = opts.get_epsilon();

            //
            // Perform the minimization.
            //
            const auto condition = simplex.execute(objfunc, execute_args);

            //
            // Emit results.
            //
            ctrl->emit_results(opts, simplex, condition);
        }

    private:
        // --------------------------------------------------------------------
        static void _logfunc(const log_args_type & log_args)
        {
            const auto ctrl = (controller_type *)(log_args.user);
            assert(ctrl != nullptr);
            ctrl->log_iteration(log_args);
        }
    };
}

#endif // JADE_OPTIMIZER_HPP__
