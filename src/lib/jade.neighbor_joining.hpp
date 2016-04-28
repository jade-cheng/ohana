/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_NEIGHBOR_JOINING_HPP__
#define JADE_NEIGHBOR_JOINING_HPP__

#include "jade.matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that implements the neighbor joining algorithm.
    ///
    template <typename TValue>
    class basic_neighbor_joining
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        ///
        /// Initializes a new instance of the class.
        ///
        explicit basic_neighbor_joining(
                const matrix_type & distances) ///< The distance matrix.
            : _children ()
            , _lengths  ()
            , _names    ()
            , _root     (invalid_id)
        {
            //
            // Allow empty matrices even though no tree will be produced from
            // the write method.
            //
            if (distances.is_empty())
                return;

            //
            // Allow just one node even though the tree produced from the write
            // method will consist on only the node name ("0").
            //
            assert(distances.is_square());
            auto n = distances.get_height();
            id_type next_id = 0;
            if (n == 1)
            {
                _root = _add_leaf(next_id);
                return;
            }

            //
            // Prepare a list of ids for the initial set of nodes.
            //
            typedef std::vector<id_type> vector_type;
            vector_type x;
            for (size_t i = 0; i < n; i++)
                x.push_back(_add_leaf(next_id));

            //
            // Prepare the distance matrix that will be reduced by the
            // algorithm.
            //
            matrix_type d (distances);

            //
            // Loop until there are only two nodes remaining in the distance
            // matrix.
            //
            while (n > 2)
            {
                //
                // Find the minimum Q value in the matrix, and use it to find
                // the two nodes that will be joined. Join them by creating a
                // new parent node.
                //
                const q_data  q  (d);
                const id_type id (next_id++);
                _add_parent(id, x[q.i], q.d_ik);
                _add_parent(id, x[q.j], q.d_jk);

                //
                // Prepare the new, reduced distance matrix as well as the
                // corresponding id vector.
                //
                matrix_type dd (n - 1, n - 1);
                vector_type xx { id };
                for (size_t r = 0, rr = 1; r < n; r++)
                {
                    if (r == q.i || r == q.j)
                        continue;
                    xx.push_back(x[r]);
                    dd(rr, 0) = value_type(0.5) *
                        (d(r, q.i) + d(r, q.j) - q.d_ij);
                    for (size_t c = 0, cc = 1; c < r; c++)
                        if (c != q.i && c != q.j)
                            dd(rr, cc++) = d(r, c);
                    rr++;
                }

                //
                // Copy the lower triangle to the upper triangle so the data
                // in the next Q matrix matches the expected values.
                //
                dd.copy_lower_to_upper();

                d.swap(dd);
                x.swap(xx);
                n--;
            }

            //
            // Connect the last two nodes; note the loop above places new nodes
            // at index zero, so here it is known that the leaf node must be at
            // index 1, and so the root note must be at index 0.
            //
            _root = x[0];
            _add_parent(_root, x[1], d(1, 0));
        }

        ///
        /// \return A string representation of this class.
        ///
        std::string str() const
        {
            std::ostringstream out;
            write(out);
            return out.str();
        }

        ///
        /// Writes the constructed tree in Newick format to the specified output
        /// stream.
        ///
        void write(
                std::ostream & out) ///< The output stream.
                const
        {
            if (_root != invalid_id)
            {
                _write(out, _root);
                out << ';';
            }
        }

    private:
        typedef size_t id_type;

        static constexpr id_type invalid_id =
            std::numeric_limits<id_type>::max();

        // --------------------------------------------------------------------
        id_type _add_leaf(id_type & next_id)
        {
            const auto id = next_id++;
            _children[id];
            _names.insert(id);
            return id;
        }

        // --------------------------------------------------------------------
        void _add_parent(
                const id_type    parent_id,
                const id_type    child_id,
                const value_type child_length)
        {
            _children[parent_id].push_back(child_id);
            _lengths[child_id] = child_length;
        }

        // --------------------------------------------------------------------
        void _write(std::ostream & out, const id_type id) const
        {
            const auto & children = _children.find(id)->second;
            if (!children.empty())
            {
                out << "(";

                auto iter = children.begin();
                _write(out, *iter);

                while (++iter != children.end())
                {
                    out << ",";
                    _write(out, *iter);
                }

                out << ")";
            }

            const auto name_iter = _names.find(id);
            if (name_iter != _names.end())
                out << id;

            const auto length_iter = _lengths.find(id);
            if (length_iter != _lengths.end())
                out << ":" << length_iter->second;
        }

        // --------------------------------------------------------------------
        struct q_data
        {
            value_type d_ij;
            value_type d_ik;
            value_type d_jk;
            size_t     i;
            size_t     j;

            // ----------------------------------------------------------------
            explicit q_data(const matrix_type & d)
                : d_ij ()
                , d_ik ()
                , d_jk ()
                , i    ()
                , j    ()
            {
                static const auto k_0_5 = value_type(0.5);

                const auto n     = d.get_height();
                const auto k_n_2 = value_type(n - 2);

                //
                // Cache the row sums; column sums would work equally well
                // because the matrix is symmetric.
                //
                std::vector<value_type> sigma;
                for (size_t c = 0; c < n; c++)
                    sigma.push_back(d.get_row_sum(c));

                //
                // Compute the values of the Q matrix.
                //
                matrix_type q (n, n);
                for (size_t r = 0; r < n; r++)
                    for (size_t c = 0; c < r; c++)
                        q(r, c) = k_n_2 * d(r, c) - sigma[r] - sigma[c];

                //
                // Find the cell with the minimum value.
                //
                i = 1, j = 0;
                for (size_t r = 2; r < n; r++)
                    for (size_t c = 0; c < r; c++)
                        if (q(r, c) < q(i, j))
                            i = r, j = c;

                //
                // Compute distances between the new nodes.
                //
                d_ij = d(i, j);
                d_ik = k_0_5 * (d_ij + ((sigma[i] - sigma[j]) / k_n_2));
                d_jk = d_ij - d_ik;
            }
        };

        std::map<id_type, std::vector<id_type>> _children;
        std::map<id_type, value_type>           _lengths;
        std::set<id_type>                       _names;
        id_type                                 _root;
    };
}

#endif // JADE_NEIGHBOR_JOINING_HPP__
