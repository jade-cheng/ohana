/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_NEWICK_HPP__
#define JADE_NEWICK_HPP__

#include "jade.scanner.hpp"

namespace jade
{
    ///
    /// A template class representing a node from a Newick tree.
    ///
    template <typename TValue>
    class basic_newick_node
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The constant Newick node pointer type.
        typedef const basic_newick_node * const_pointer_type;

        /// The Newick node pointer type.
        typedef basic_newick_node * pointer_type;

        /// The children vector type.
        typedef std::vector<pointer_type> children_type;

        /// The scanner type.
        typedef basic_scanner<char> scanner_type;

        ///
        /// Reclaims resources used by this instance and its children.
        ///
        inline ~basic_newick_node()
        {
            _destruct();
        }

        ///
        /// Initializes a new instance of the class. Initially, the instance
        /// has no name, no length, no parent, and no children.
        ///
        basic_newick_node()
            : _children   ()
            , _has_length (false)
            , _id         (0)
            , _length     (0)
            , _name       ()
            , _parent     ()
        {
        }

        ///
        /// Initializes a new instance of the class from the specified input
        /// stream. The input stream must end with a semicolon. If an error
        /// occurs, the method throws an exception.
        ///
        /// \throw jade::error Thrown if there is an error parsing the tree.
        ///
        explicit basic_newick_node(
                std::istream & in) ///< The input stream to read.
            : basic_newick_node ()
        {
            scanner_type s (in);
            _read(s);
        }

        ///
        /// Initializes a new instance of the class from the specified input
        /// string. The input string must end with a semicolon. If an error
        /// occurs, the method throws an exception.
        ///
        /// \throw jade::error Thrown if there is an error parsing the tree.
        ///
        explicit basic_newick_node(
                const std::string & in) ///< The input string to read.
            : basic_newick_node ()
        {
            scanner_type s (in);
            _read(s);
        }

        ///
        /// Erases the length.
        ///
        inline void erase_length()
        {
            _length = 0;
            _has_length = false;
        }

        ///
        /// \return A colleciton of all descendents that match the specified
        /// predicate. The method searches in depth-first order and examines
        /// this node as well.
        ///
        template <typename TPredicate>
        std::set<const_pointer_type> find_all(
                const TPredicate predicate) ///< The predicate to match.
                const
        {
            std::set<const_pointer_type> container;

            this->for_each([predicate, &container](
                     const_pointer_type node)
                     -> void
            {
                if (predicate(node))
                    container.insert(node);
            });

            return container;
        }

        ///
        /// \return A colleciton of all descendents that match the specified
        /// predicate. The method searches in depth-first order and examines
        /// this node as well.
        ///
        template <typename TPredicate>
        std::set<pointer_type> find_all(
                const TPredicate predicate) ///< The predicate to match.
        {
            std::set<pointer_type> container;

            this->for_each([predicate, &container](
                     pointer_type node)
                     -> void
            {
                if (predicate(node))
                    container.insert(node);
            });

            return container;
        }

        ///
        /// \return The collection of descendents of this node.
        ///
        inline std::set<const_pointer_type> find_descendents() const
        {
            return find_all([this](const_pointer_type node) -> bool
            {
                return node != this;
            });
        }

        ///
        /// \return The collection of descendents of this node.
        ///
        inline std::set<pointer_type> find_descendents()
        {
            return find_all([this](pointer_type node) -> bool
            {
                return node != this;
            });
        }

        ///
        /// \return The first descendent that matches the specified predicate.
        /// The method searches in depth-first order and examines this node as
        /// well.
        ///
        template <typename TPredicate>
        const_pointer_type find_first(
                const TPredicate predicate) ///< The predicate to match.
                const
        {
            for (auto child : _children)
            {
                const auto first = child->find_first(predicate);
                if (nullptr != first)
                    return first;
            }

            return predicate(this) ? this : nullptr;
        }

        ///
        /// \return The first descendent that matches the specified predicate.
        /// The method searches in depth-first order and examines this node as
        /// well.
        ///
        template <typename TPredicate>
        pointer_type find_first(
                const TPredicate predicate) ///< The predicate to match.
        {
            for (auto child : _children)
            {
                const pointer_type first = child->find_first(predicate);
                if (nullptr != first)
                    return first;
            }

            return predicate(this) ? this : nullptr;
        }

        ///
        /// \return The node with the specified id.
        ///
        const_pointer_type find_id(
                const int id) ///< The id of the node to find.
                const
        {
            return find_first([id](const_pointer_type node) -> bool
            {
                return node->_id == id;
            });
        }

        ///
        /// \return The node with the specified id.
        ///
        pointer_type find_id(
                const int id) ///< The id of the node to find.
        {
            return find_first([id](pointer_type node) -> bool
            {
                return node->_id == id;
            });
        }

        ///
        /// \return The set of leaf nodes. If this node is a leaf node, it is
        /// returned in the collection.
        ///
        std::set<const_pointer_type> find_leafs() const
        {
            return find_all([](const_pointer_type node) -> bool
            {
                return node->is_leaf();
            });
        }

        ///
        /// \return The set of leaf nodes. If this node is a leaf node, it is
        /// returned in the collection.
        ///
        std::set<pointer_type> find_leafs()
        {
            return find_all([](pointer_type node) -> bool
            {
                return node->is_leaf();
            });
        }

        ///
        /// \return The node with the specified name.
        ///
        const_pointer_type find_name(
                char const * const name) ///< The name of the node to find.
                const
        {
            return find_first([name](const_pointer_type node) -> bool
            {
                return node->_name == name;
            });
        }

        ///
        /// \return The node with the specified name.
        ///
        pointer_type find_name(
                char const * const name) ///< The name of the node to find.
        {
            return find_first([name](pointer_type node) -> bool
            {
                return node->_name == name;
            });
        }

        ///
        /// \return The root of the tree associated with this node.
        ///
        const_pointer_type find_root() const
        {
            return is_root() ? this : _parent->find_root();
        }

        ///
        /// \return The root of the tree associated with this node.
        ///
        pointer_type find_root()
        {
            return is_root() ? this : _parent->find_root();
        }

        ///
        /// Executes the specified action for all descendent nodes, including
        /// this instance, in depth-first order.
        ///
        template <typename TAction>
        void for_each(
                const TAction action) ///< The action to perform.
                const
        {
            for (auto child : _children)
                child->for_each(action);

            action(this);
        }

        ///
        /// Executes the specified action for all descendent nodes, including
        /// this instance, in depth-first order.
        ///
        template <typename TAction>
        void for_each(
                const TAction action) ///< The action to perform.
        {
            for (auto child : _children)
                child->for_each(action);

            action(this);
        }

        ///
        /// \return A new instance based on the specified file.
        ///
        /// \throw jade::error Thrown if there is an error parsing the tree.
        ///
        inline static pointer_type from_file(
                char const * const path) ///< The path to the file.
        {
            std::ifstream in (path);
            return new basic_newick_node(in);
        }

        ///
        /// \return A new instance based on the specified file.
        ///
        /// \throw jade::error Thrown if there is an error parsing the tree.
        ///
        inline static basic_newick_node * from_file(
                const std::string & path) ///< The path to the file to decode.
        {
            return from_file(path.c_str());
        }

        ///
        /// \return The collection of immediate children of this node.
        ///
        inline const children_type & get_children() const
        {
            return _children;
        }

        ///
        /// \return The unique ID for this node within the tree. If the tree is
        /// rerooted, this id persists into the new tree.
        ///
        inline int get_id() const
        {
            return _id;
        }

        ///
        /// \return The length of this node; the length is valid only if the
        /// has_length method returns true.
        ///
        inline value_type get_length() const
        {
            return _length;
        }

        ///
        /// \return The name of this node.
        ///
        inline const std::string & get_name() const
        {
            return _name;
        }

        ///
        /// \return The parent of this node.
        ///
        inline const_pointer_type get_parent() const
        {
            return _parent;
        }

        ///
        /// \return The parent of this node.
        ///
        inline pointer_type get_parent()
        {
            return _parent;
        }

        ///
        /// \return True if the node has a defined length.
        ///
        inline bool has_length() const
        {
            return _has_length;
        }

        ///
        /// \return True if the node has a name.
        ///
        inline bool has_name() const
        {
            return !_name.empty();
        }

        ///
        /// \return True if the node has no children; i.e. it is a leaf node.
        ///
        inline bool is_leaf() const
        {
            return _children.empty();
        }

        ///
        /// \return True if the node has no parent; i.e. it is a root node.
        ///
        inline bool is_root() const
        {
            return _parent == nullptr;
        }

        ///
        /// Creates a new tree based on the tree associated with this node; in
        /// the tree returned, a copy of this node is the root.
        ///
        /// \return A new Newick node, the root of an equivalent tree.
        ///
        pointer_type reroot() const
        {
            //
            // Reroot the structure of the tree.
            //
            auto root = _clone();
            _copy_children(this, root, nullptr);
            _copy_parents(this, root);

            //
            // To preserve information, assign the length
            // to the old root to this new root.
            //
            auto old_root = find_root();
            root->_length = old_root->_length;
            root->_has_length = old_root->_has_length;

            return root;
        }

        ///
        /// Assigns the length.
        ///
        void set_length(
            const value_type length) ///< The length to assign.
        {
            assert(!std::isnan(length));
            assert(!std::isinf(length));
            _length = length;
            _has_length = true;
        }

        ///
        /// Assigns the name.
        ///
        inline void set_name(
            char const * const name) ///< The name to assign.
        {
            assert(name != nullptr);
            _name = name;
        }

        ///
        /// Encodes this instance and returns the output as a string. A
        /// semicolon is written as the last character of the file.
        ///
        /// \return The encoded string.
        ///
        inline std::string str() const
        {
            std::ostringstream out;
            write(out);
            return out.str();
        }

        ///
        /// Writes this instance into the specified output stream. The output
        /// stream receives a semicolon after the tree data.
        ///
        void write(
                std::ostream & out) ///< The output stream.
                const
        {
            _write(out);
            out << ';';
        }

        ///
        /// Writes this instance into the specified file. A semicolon is
        /// written as the last character of the file.
        ///
        inline void write(
                char const * const path) ///< The path to the output file.
                const
        {
            std::ofstream out (path);
            write(out);
            out << std::endl;
        }

        ///
        /// Writes this instance into the specified file. A semicolon is
        /// written as the last character of the file.
        ///
        inline void write(
                const std::string & path) ///< The path to the output file.
                const
        {
            write(path.c_str());
        }

    private:
        basic_newick_node(const basic_newick_node &) = delete;
        basic_newick_node & operator = (const basic_newick_node &) = delete;

        // --------------------------------------------------------------------
        pointer_type _clone() const
        {
            auto node = new basic_newick_node();
            node->_has_length = _has_length;
            node->_id         = _id;
            node->_length     = _length;
            node->_name       = _name;
            return node;
        }

        // --------------------------------------------------------------------
        static void _copy_children(
                const_pointer_type source,
                pointer_type       target,
                const_pointer_type excluded)
        {
            //
            // Loop over the children from the source node.
            //
            for (auto source_child : source->_children)
            {
                //
                // Skip the excluded child, if necessary.
                //
                if (source_child == excluded)
                    continue;

                //
                // Clone the source child and insert it into the target children.
                //
                auto target_child = source_child->_clone();
                target->_children.push_back(target_child);
                target_child->_parent = target;

                //
                // Recursively copy children.
                //
                _copy_children(source_child, target_child, nullptr);
            }
        }

        // --------------------------------------------------------------------
        static void _copy_parents(
                const_pointer_type source,
                pointer_type       target)
        {
            //
            // Do nothing if the source is the root.
            //
            const auto source_parent = source->_parent;
            if (source_parent == nullptr)
                return;

            //
            // Clone the source parent and assign it as a child of the target.
            //
            auto target_child = source_parent->_clone();
            target->_children.push_back(target_child);
            target_child->_parent = target;

            //
            // Recursively process the children and parents.
            //
            _copy_children(source_parent, target_child, source);
            _copy_parents(source_parent, target_child);

            //
            // Reverse the direction of the length for the new child.
            //
            target_child->_length = source->_length;
            target_child->_has_length = source->_has_length;
        }

        // --------------------------------------------------------------------
        inline void _destruct()
        {
            for (auto child : _children)
                delete child;
            _children.clear();
        }

        // --------------------------------------------------------------------
        void _read(scanner_type & in)
        {
            //
            // This method executes from the constructor and also throws
            // exceptions, so take care to reclaim resources (the children) in
            // case an exception occurs.
            //
            try
            {
                //
                // Recursively create all nodes in the tree.
                //
                auto node_count = 0;
                _read(in, node_count);

                //
                // Require the last non-whitespace character, the semicolon.
                //
                in.expect(';');

                //
                // Ensure no more non-whitespace characters are in the stream.
                //
                in.skip_whitespace();
                if (in.is_end_of_data())
                    return;

                throw jade::error()
                    << "expected end of stream but encountered "
                    << " '" << in.read_token() << "'";
            }
            catch (...)
            {
                //
                // Reclaim resources before rethrowing the exception.
                //
                _destruct();
                throw;
            }
        }

        // --------------------------------------------------------------------
        void _read(scanner_type & in, int & node_count)
        {
            static const char delims[] = ";:(),";

            //
            // Create a new id for each node created.
            //
            _id = ++node_count;

            //
            // Determine if this node has children.
            //
            if (in.try_char('('))
            {
                //
                // Loop over all children, which are separated by colons.
                //
                do
                {
                    //
                    // Create the child, storing it into the collection of
                    // children, and updating the child to reference the parent.
                    //
                    auto child = new basic_newick_node();
                    child->_parent = this;
                    _children.push_back(child);
                    child->_read(in, node_count);
                }
                while (in.try_char(','));

                //
                // Require a closing parenthesis after the last child.
                //
                in.expect(')');
            }

            //
            // Parse the name, if any.
            //
            _name = _trim(in.read_token(delims));

            //
            // If the stream provides a colon, parse the length.
            //
            if (in.try_char(':'))
            {
                _length     = in.read_real<value_type>();
                _has_length = true;
            }
        }

        // --------------------------------------------------------------------
        static std::string _trim(const std::string & s)
        {
            size_t begin = 0;
            size_t end = s.length();
            while (begin < end && ::isspace(s[begin]))
                begin++;
            while (end > begin && ::isspace(s[end - 1]))
                end--;
            return s.substr(begin, end - begin);
        }

        // --------------------------------------------------------------------
        void _write(std::ostream & out) const
        {
            const auto end = _children.end();

            //
            // Loop over each child, if any.
            //
            auto iter = _children.begin();
            if (iter != end)
            {
                //
                // Enclose all children in parentheses.
                //
                out << '(';
                (*iter)->_write(out);

                //
                // Separate children with commas.
                //
                while (++iter != end)
                {
                    out << ',';
                    (*iter)->_write(out);
                }

                //
                // Close the parentheses for the children.
                //
                out << ')';
            }

            //
            // Write the name, if one exists.
            //
            if (has_name())
                out << _name;

            //
            // Write the length, if one exists.
            //
            if (_has_length)
                out << ':' << _length;
        }

        children_type _children;   ///< A container for children.
        bool          _has_length; ///< A flag for a defined length.
        int           _id;         ///< A unique ID for the node.
        value_type    _length;     ///< A possible length.
        std::string   _name;       ///< A name.
        pointer_type  _parent;     ///< A parent node.
    };
}

///
/// Writes the specified node to the output stream.
///
/// \param lhs The output stream.
/// \param rhs The newick node.
/// \return    The output stream.
///
template <typename TValue>
inline std::ostream & operator << (
        std::ostream &                          lhs,
        const jade::basic_newick_node<TValue> & rhs)
{
    rhs.write(lhs);
    return lhs;
}

#endif // JADE_NEWICK_HPP__
