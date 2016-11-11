/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_AGI_CONTROLLER_HPP__
#define JADE_AGI_CONTROLLER_HPP__

#include "jade.controller.hpp"

namespace jade
{
    ///
    /// A template for a class that encodes and decodes parameters for the
    /// Nelder-Mead algorithm. This class uses an admixture graph input file.
    ///
    template <typename TValue>
    class basic_agi_controller
        : public basic_controller<TValue>
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The admixture graph input reader type.
        typedef basic_agi_reader<value_type> agi_reader_type;

        /// The shunting-yard algorithm type.
        typedef basic_shunting_yard<value_type> shunting_yard_type;

        /// The arguments passed to the shunting-yard algorithm.
        typedef typename shunting_yard_type::args_type args_type;

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

        /// The exit condition type for the simplex.
        typedef typename simplex_type::exit_condition_type exit_condition_type;

        ///
        /// Initializes a new instance of the class based on the specified
        /// program settings.
        ///
        explicit basic_agi_controller(
                const settings_type & settings) ///< The program settings.
            : basic_controller<TValue> (settings)
            , _agi                     (settings.get_agi())
            , _args                    (_agi.get_args())
        {
        }

        ///
        /// Writes results to standard output and files.
        ///
        void emit_results(
                const options_type &        opts,      ///< The options.
                const simplex_type &        simplex,   ///< The simplex.
                const exit_condition_type & condition) ///< The context.
                override
        {
            //
            // The base class emits the C matrix; this also has the effect of
            // updating the arguments used to evaluate the expressions.
            //
            basic_controller<TValue>::emit_results(opts, simplex, condition);

            //
            // Loop over each argument and display its value.
            //
            std::cout << "\n[Admixture Graph Output]\n";
            matrix_type::set_high_precision(std::cout);
            for (const auto & name : _agi.get_branch_names())
                std::cout << name << "\t" << _args[name] << "\n";
            for (const auto & name : _agi.get_proportion_names())
                std::cout << name << "\t" << _args[name] << "\n";
            std::cout.flush();
        }

        ///
        /// Creates and returns the initial set of parameters for the Nelder-
        /// Mead algorithm.
        ///
        /// \return The initial parameters for the Nelder-Mead algorithm.
        ///
        container_type init_parameters() override
        {
            //
            // Return a container initially filled with 0.5 for both branch and
            // proportion variables.
            //
            container_type out;
            out.resize(_args.size(), value_type(0.5));
            return out;
        }

    protected:
        ///
        /// Decodes the specified Nelder-Mead container and stores the result
        /// into the lower triangle, including the diagonal, of the covariance
        /// matrix.
        /// \return True if successful; otherwise, false.
        ///
        bool _decode_lower(
                matrix_type &          dst, ///< The covariance matrix.
                const container_type & src) ///< The Nelder-Mead container.
                override
        {
            //
            // If any value in the container is not positive, then return false
            // to indicate these are unacceptable values.
            //
            for (const auto item : src)
                if (item <= value_type(0))
                    return false;

            //
            // If any value corresponding to a proportion is not less than one,
            // then return false to indicate these are unacceptable values.
            // Note the proportion variables are stored first in the container
            // (and are followed by the branch variables).
            //
            {
                const auto n = _agi.get_proportion_names().size();
                for (size_t i = 0; i < n; i++)
                    if (src[i] >= value_type(1))
                        return false;
            }

            //
            // Copy the values from the Nelder-Mead container into the table
            // of arguments used to evaluate the expression. Since these values
            // are the same for all expressions, the same container is used for
            // each expression evaluated.
            //
            {
                size_t i = 0;
                for (const auto & name : _agi.get_proportion_names())
                    _args[name] = src[i++];
                for (const auto & name : _agi.get_branch_names())
                    _args[name] = src[i++];
            }

            //
            // Copy the expression output to the lower-triangle of the
            // covariance matrix:
            //
            //  | 0 - - |
            //  | 1 2 - |
            //  | 3 4 5 |
            //
            // Each value in the matrix is determined by evaluating a
            // corresponding expression.
            //
            {
                const auto & entries = _agi.get_entries();
                const auto   rk      = _agi.get_k() - 1;
                size_t       i       = 0;
                for (size_t row = 0; row < rk; row++)
                    for (size_t col = 0; col <= row; col++)
                        dst(row, col) = entries[i++].evaluate(_args);
            }

            //
            // Return true to indicate the values are acceptable.
            //
            return true;
        }

    private:
        const agi_reader_type & _agi;  // The admixture graph input.
        args_type               _args; // Arguments used to evaluate entries.
    };
}

#endif // JADE_AGI_CONTROLLER_HPP__
