/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_TREE_CONTROLLER_HPP__
#define JADE_TREE_CONTROLLER_HPP__

#include "jade.controller.hpp"
#include "jade.rerooted_tree.hpp"

namespace jade
{
    ///
    /// A template for a class that encodes and decodes parameters for the
    /// Nelder-Mead algorithm. This class uses a user-specified Newick tree.
    ///
    template <typename TValue>
    class basic_tree_controller
        : public basic_controller<TValue>
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The options type.
        typedef basic_options<value_type> options_type;

        /// The settings type.
        typedef basic_settings<value_type> settings_type;

        /// The path type.
        typedef basic_tree_path<value_type> path_type;

        /// The simplex type.
        typedef basic_simplex<value_type> simplex_type;

        /// The rerooted tree type.
        typedef basic_rerooted_tree<value_type> rerooted_tree_type;

        /// The unrooted tree type.
        typedef basic_newick_node<value_type> unrooted_tree_type;

        /// The pointer to an unrooted tree type.
        typedef std::unique_ptr<unrooted_tree_type> unrooted_tree_ptr_type;

        /// The container type for the simplex.
        typedef typename simplex_type::container_type container_type;

        /// The exit condition type for the simplex.
        typedef typename simplex_type::exit_condition_type exit_condition_type;

        ///
        /// Initializes a new instance of the class based on the specified
        /// program settings.
        ///
        explicit basic_tree_controller(
                const settings_type & settings) ///< The program settings.
            : basic_controller<TValue> (settings)
            , _settings          (settings)
            , _table             ()
            , _unrooted_tree_ptr ()
            , _rerooted_tree     ()
        {
            const auto rk = this->get_rk();

            //
            // Parse the Newick-formatted file into a tree structure.
            //
            _unrooted_tree_ptr.reset(
                unrooted_tree_type::from_file(
                    settings.get_options().get_tin()));

            //
            // Initialize the rerooted tree.
            //
            _rerooted_tree.reset(*_unrooted_tree_ptr);

            //
            // Loop over the rows and columns of the lower triangle, and for
            // each cell in the covariance matrix, create a corresponding
            // table entry.
            //
            for (size_t r = 0; r < rk; r++)
                for (size_t c = 0; c <= r; c++)
                    _table.emplace_back(_rerooted_tree, r, c);
        }

        ///
        /// Writes results to standard output and files.
        ///
        virtual void emit_results(
                const options_type &        opts,      ///< The options.
                const simplex_type &        simplex,   ///< The simplex.
                const exit_condition_type & condition) ///< The context.
                override
        {
            //
            // The base class emits the C matrix; this also has the effect of
            // updating the tree with the optimized lengths.
            //
            basic_controller<TValue>::emit_results(opts, simplex, condition);

            //
            // Copy values back to the original tree. First reroot the tree to
            // its original structure so the order of the children is
            // consistent with the file specified by the user.
            //
            const auto root_id = _unrooted_tree_ptr->get_id();
            unrooted_tree_ptr_type source_root (
                _rerooted_tree.get_tree().find_id(root_id)->reroot());
            _unrooted_tree_ptr->for_each([&](node_type * const target) -> void
            {
                const auto id     = target->get_id();
                const auto source = source_root->find_id(id);
                if (source->has_length())
                    target->set_length(source->get_length());
                else
                    target->erase_length();
            });

            if (opts.is_tout_specified())
            {
                const auto & tout = opts.get_tout();
                std::cout << "Writing tree to " << tout << std::endl;
                _unrooted_tree_ptr->write(tout);
            }
            else
            {
                std::cout << "\n[Tree]\n" << *_unrooted_tree_ptr << std::endl;
            }
        }

        // --------------------------------------------------------------------
        virtual container_type init_parameters() override
        {
            container_type container;
            _copy_tree_to_container(container, _rerooted_tree.get_tree());
            return container;
        }

    protected:
        ///
        /// Decodes the specified Nelder-Mead container and stores the result
        /// into the lower triangle, including the diagonal, of the covariance
        /// matrix.
        /// \return True to indicate this method is successful.
        ///
        virtual bool _decode_lower(
                matrix_type &          dst, ///< The covariance matrix.
                const container_type & src) ///< The Nelder-Mead container.
                override
        {
            //
            // Copy the container values into the tree, which will be used to
            // build the covariance matrix.
            //
            auto iterator = src.begin();
            _copy_container_to_tree(_rerooted_tree.get_tree(), iterator);

            //
            // Loop over every table entry, which corresponds to the lower
            // triangle of the covariance matrix; compute and store the sum
            // of the lengths from the nodes that contribute to its value.
            //
            for (const auto & entry : _table)
                dst[entry.i] = entry.p.get_length();

            //
            // This method is always successful.
            //
            return true;
        }

    private:
        //
        // An entry in the table holding the index in the covariance matrix
        // and the set of nodes that contributes to the cell's value.
        //
        struct table_entry
        {
            size_t    i; // The index in the matrix.
            path_type p; // The overlapping tree path.

            inline table_entry(
                    const rerooted_tree_type & rerooted_tree,
                    const size_t               row,
                    const size_t               column)
                : i (row * rerooted_tree.get_rk() + column)
                , p (rerooted_tree.get_overlap(row, column))
            {
            }
        };

        //
        // The type for the vector of table entries used to populate the values
        // of the covariance matrix based on the lengths in the tree.
        //
        typedef std::vector<table_entry>      table_type;
        typedef basic_newick_node<value_type> node_type;

        //
        // Recursively copies the values from the specified Nelder-Mead
        // container into the nodes of the tree. The length of the parent is not
        // affected by the call to this method.
        //
        // \param parent   The parent of the nodes to assign.
        // \param iterator The container iterator providing lengths.
        //
        void _copy_container_to_tree(
                const node_type &                         parent,
                typename container_type::const_iterator & iterator)
                const
        {
            for (auto child : parent.get_children())
            {
                child->set_length(*iterator++);
                _copy_container_to_tree(*child, iterator);
            }
        }

        //
        // Recursively adds the lengths of the children of the specified node to
        // the Nelder-Mead container. The length of the parent is not used by
        // the call to this method.
        //
        // \param container The Nelder-Mead container receiving the lengths.
        // \param parent    The parent of the nodes to copy.
        //
        void _copy_tree_to_container(
                container_type &  container,
                const node_type & parent)
                const
        {
            for (auto child : parent.get_children())
            {
                container.push_back(child->get_length());
                _copy_tree_to_container(container, *child);
            }
        }

        const settings_type &  _settings;
        table_type             _table;
        unrooted_tree_ptr_type _unrooted_tree_ptr;
        rerooted_tree_type     _rerooted_tree;
    };
}

#endif // JADE_TREE_CONTROLLER_HPP__
