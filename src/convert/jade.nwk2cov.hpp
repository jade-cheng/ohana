/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_NWK2COV_HPP__
#define JADE_NWK2COV_HPP__

#include "jade.matrix.hpp"
#include "jade.rerooted_tree.hpp"

namespace jade
{
    ///
    /// A template for a class that converts Newick-formatted trees to
    /// covariance matrices.
    ///
    template <typename TValue>
    class basic_nwk2cov
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The Newick node type.
        typedef basic_newick_node<value_type> newick_node_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The rerooted tree type.
        typedef basic_rerooted_tree<value_type> rerooted_tree_type;

        ///
        /// Executes the program through the specified streams.
        ///
        static void execute(
                std::istream & in,  ///< The input stream.
                std::ostream & out) ///< The output stream.
        {
            const newick_node_type   node (in);
            const rerooted_tree_type tree (node);

            const auto rk = tree.get_rk();
            matrix_type c (rk, rk);

            for (size_t i = 0; i < rk; i++)
                for (size_t j = 0; j <= i; j++)
                    c(i, j) = tree.get_overlap(i, j).get_length();

            c.copy_lower_to_upper();

            matrix_type::set_high_precision(out);
            out << c;
        }
    };
}

#endif // JADE_NWK2COV_HPP__
