/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_TREE_PATH_HPP__
#define JADE_TREE_PATH_HPP__

#include "jade.newick.hpp"

namespace jade
{
    ///
    /// A template for a class that maintains a table of nodes indicating a
    /// distance to the root of the tree.
    ///
    template <typename TValue>
    class basic_tree_path
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The Newick node type.
        typedef basic_newick_node<value_type> node_type;

        //
        // Initialize a new instance of the class with no nodes.
        //
        inline basic_tree_path()
            : _map ()
        {
        }

        ///
        /// Initializes a new instance of the class by computing the set of
        /// nodes in the path to the root. The specified node is included in the
        /// result, but the root node is not. For example, computing a tree path
        /// for node 'D' for the tree, "((B:2,(C:4,D:5)n1:3)n2:1)A;" results in
        /// the container (D, n1, n2).
        ///
        explicit basic_tree_path(
                const node_type & node) ///< The node to measure.
            : _map ()
        {
            //
            // Loop toward the root node, starting at the specified node, but
            // do not include the root node in the result; in each iteration,
            // accumulate the nodes traversed.
            //
            for (auto n = &node; !n->is_root(); n = n->get_parent())
                _map[n->get_id()] = n;
        }

        ///
        /// \return The sum of the lengths for all nodes of this instance.
        ///
        value_type get_length() const
        {
            auto sum = value_type(0);
            for (const auto & pair : _map)
                sum += pair.second->get_length();
            return sum;
        }

        ///
        /// Computes the overlapped tree path of this instance and another
        /// instance. This method returns only the nodes that exist in both
        /// containers.
        ///
        /// \return A new tree path.
        ///
        basic_tree_path operator & (
                const basic_tree_path & rhs) ///< The other tree path.
                const
        {
            basic_tree_path p;

            for (const auto & pair: rhs._map)
                if (_map.find(pair.first) != _map.end())
                    p._map[pair.first] = pair.second;

            return p;
        }

    private:
        std::map<int, const node_type *> _map;
    };
}

#endif // JADE_TREE_PATH_HPP__
