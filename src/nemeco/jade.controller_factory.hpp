/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_CONTROLLER_FACTORY_HPP__
#define JADE_CONTROLLER_FACTORY_HPP__

#include "jade.agi_controller.hpp"
#include "jade.tree_controller.hpp"
#include "jade.treeless_controller.hpp"

namespace jade
{
    ///
    /// A template for a class that creates controllers that use or do not use
    /// Newick trees specified on the command line.
    ///
    template <typename TValue>
    class basic_controller_factory
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The settings type.
        typedef basic_settings<value_type> settings_type;

        /// The controller type.
        typedef basic_controller<value_type> controller_type;

        /// The options type.
        typedef basic_options<value_type> options_type;

        /// The AGI controller type.
        typedef basic_agi_controller<value_type> agi_controller_type;

        /// The tree controller type.
        typedef basic_tree_controller<value_type> tree_controller_type;

        /// The treeless controller type.
        typedef basic_treeless_controller<value_type> treeless_controller_type;

        ///
        /// \return A new controller.
        ///
        static controller_type * create(
                const settings_type & settings) ///< The program settings.
        {
            const auto & opts = settings.get_options();
            if (opts.is_tin_specified())
                return new tree_controller_type(settings);
            if (opts.is_ain_specified())
                return new agi_controller_type(settings);
            else
                return new treeless_controller_type(settings);
        }
    };
}

#endif // JADE_CONTROLLER_FACTORY_HPP__
