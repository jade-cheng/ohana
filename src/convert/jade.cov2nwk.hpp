/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_COV2NWK_HPP__
#define JADE_COV2NWK_HPP__

#include "jade.neighbor_joining.hpp"
#include "jade.verification.hpp"

namespace jade
{
    ///
    /// A template for a class that approximates covariance matrices as
    /// Newick-formatted trees.
    ///
    template <typename TValue>
    class basic_cov2nwk
    {
    public:
        /// The value type.
        typedef TValue value_type;

        ///
        /// Executes the program through the specified streams.
        ///
        static void execute(
                std::istream & in,  ///< The input stream.
                std::ostream & out) ///< The output stream.
        {
            typedef basic_matrix<value_type> matrix_type;
            const matrix_type c (in);

            typedef basic_verification<value_type> verification_type;
            verification_type::validate_c(c);

            const auto rk = c.get_height();
            const auto k  = rk + 1;

            matrix_type padded_c (k, k);
            for (size_t i = 0; i < rk; i++)
                for (size_t j = 0; j < rk; j++)
                    padded_c(i + 1, j + 1) = c(i, j);

            matrix_type distances (k, k);
            for (size_t i = 0; i < k; i++)
            {
                const auto c_ii = padded_c(i, i);
                for (size_t j = 0; j < k; j++)
                {
                    const auto c_jj = padded_c(j, j);
                    const auto c_ij = padded_c(i, j);
                    distances(i, j) = c_ii + c_jj - c_ij - c_ij;
                }
            }

            typedef basic_neighbor_joining<value_type> algorithm_type;
            const algorithm_type algorithm (distances);

            algorithm.write(out);
            out << std::endl;
        }
    };
}

#endif // JADE_COV2NWK_HPP__
