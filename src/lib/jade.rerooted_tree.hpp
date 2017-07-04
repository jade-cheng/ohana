/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_REROOTED_TREE_HPP__
#define JADE_REROOTED_TREE_HPP__

#include "jade.tree_path.hpp"

namespace jade
{
    ///
    /// A template for a class that maintains a list of paths from all leaf
    /// nodes to the rerooted node with the name "0".
    ///
    template <typename TValue>
    class basic_rerooted_tree
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The tree path type.
        typedef basic_tree_path<value_type> path_type;

        /// The Newick node type.
        typedef basic_newick_node<value_type> node_type;

        /// The Newick node pointer type.
        typedef std::unique_ptr<node_type> node_ptr_type;

        /// The tree path vector type.
        typedef std::vector<path_type> vector_type;

        ///
        /// Initialize a new instance of the class.
        ///
        inline basic_rerooted_tree()
            : _rk       (0)
            , _node_ptr ()
            , _vector   ()
        {
        }

        ///
        /// Initialize a new instance of the class.
        ///
        explicit basic_rerooted_tree(
                const node_type & node) ///< The node to validate and reroot.
            : _rk       (_validate_tree(node))
            , _node_ptr (node.find_name("0")->reroot())
            , _vector   ()
        {
            const auto & tree = *_node_ptr;

            //
            // Create a list of tree paths, one for each leaf node.
            //
            _vector.reserve(_rk);
            for (size_t i = 1; i <= _rk; i++)
            {
                std::ostringstream name_stream;
                name_stream << i;

                const auto name     = name_stream.str();
                const auto node_ptr = tree.find_name(name.c_str());
                _vector.emplace_back(*node_ptr);
            }
        }

        ///
        /// \return The rooted K value.
        ///
        inline size_t get_rk() const
        {
            return _rk;
        }

        ///
        /// \return The overlapping path between two nodes and the root node.
        ///
        inline path_type get_overlap(
                const size_t node1, ///< The first node.
                const size_t node2) ///< The second node.
                const
        {
            return _vector[node1] & _vector[node2];
        }

        ///
        /// \return The tree path for the node with the name that corresponds
        /// to the specified index.
        ///
        inline const path_type & get_path(
                const size_t index) ///< The index of the node.
                const
        {
            assert(index < _vector.size());
            return _vector[index];
        }

        ///
        /// \return A reference to the rerooted tree.
        ///
        inline node_type & get_tree()
        {
            return *_node_ptr;
        }

        ///
        /// \return A reference to the rerooted tree.
        ///
        inline const node_type & get_tree() const
        {
            return *_node_ptr;
        }

        ///
        /// Resets the tree maintained by this instance.
        ///
        inline void reset(
                const node_type & node) ///< The node to validate and reroot.
        {
            *this = basic_rerooted_tree(node);
        }

    private:
        // --------------------------------------------------------------------
        static size_t _validate_tree(const node_type & tree)
        {
            //
            // Gather references to the leaf nodes.
            //
            const auto leaf_nodes = tree.find_leafs();
            const auto k          = leaf_nodes.size();

            //
            // Ensure there is at least one component.
            //
            if (k == 0)
                throw error()
                    << "invalid Newick tree: there must be at "
                    << "least one node";

            //
            // Ensure all leaf nodes are named and named uniquely.
            //
            std::set<std::string> names;
            for (const auto leaf : leaf_nodes)
            {
                if (!leaf->has_name())
                    throw error() << "invalid Newick tree: at least one "
                                  << "leaf node has no name";

                const auto & name = leaf->get_name();
                if (names.find(name) != names.end())
                    throw error() << "invalid Newick tree; duplicate leaf "
                                  << "node name '" << name << "'";

                names.insert(leaf->get_name());
            }

            //
            // Ensure leafs are named numerically based on their
            // corresponding component; e.g. "0", "1", "2", etc.
            //
            for (size_t index = 0; index < k; index++)
            {
                std::ostringstream name_stream;
                name_stream << index;

                const auto name = name_stream.str();
                if (names.find(name) == names.end())
                    throw error() << "invalid Newick tree: missing leaf "
                                  << "named '" << name << "'";
            }

            //
            // Return the rooted K value.
            //
            return k - 1;
        };

        size_t        _rk;
        node_ptr_type _node_ptr;
        vector_type   _vector;
    };
}

#endif // JADE_REROOTED_TREE_HPP__
