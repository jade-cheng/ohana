/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_NWK2SVG_HPP__
#define JADE_NWK2SVG_HPP__

#include "jade.svg_tree.hpp"

namespace jade
{
    ///
    /// A template for a class that converts Newick-formatted trees to SVG
    /// representations.
    ///
    template <typename TValue>
    class basic_nwk2svg
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The node type.
        typedef jade::basic_newick_node<value_type> node_type;

        /// The SVG tree type.
        typedef jade::basic_svg_tree<value_type> svg_tree_type;

        ///
        /// Executes the program through the specified streams.
        ///
        static void execute(
                std::istream & in,  ///< The input stream.
                std::ostream & out) ///< The output stream.
        {
            node_type     node (in);
            svg_tree_type svg  (node);

            svg.optimize_positions();
            svg.write(out);
        }
    };
}

#endif // JADE_NWK2SVG_HPP__
