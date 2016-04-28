/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_GENOTYPE_MATRIX_HPP__
#define JADE_GENOTYPE_MATRIX_HPP__

#include "jade.matrix.hpp"

namespace jade
{
    ///
    /// A template for an abstract class implementing operations for a genotype
    /// matrix.
    ///
    template <typename TValue>
    class basic_genotype_matrix
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<TValue> matrix_type;

        ///
        /// Reclaims resources used by the class and derived classes.
        ///
        inline virtual ~basic_genotype_matrix()
        {
        }

        ///
        /// Computes the derivative vector and hessian matrix for a specified
        /// marker of the F matrix.
        ///
        virtual void compute_derivatives_f(
                const matrix_type & q,     ///< The Q matrix.
                const matrix_type & fa,    ///< The F matrix.
                const matrix_type & fb,    ///< The 1-F matrix.
                const matrix_type & qfa,   ///< The Q*F product.
                const matrix_type & qfb,   ///< The Q*(1-F) product.
                const size_t        j,     ///< The marker.
                matrix_type &       d_vec, ///< The derivative vector.
                matrix_type &       h_mat) ///< The hessian matrix.
                const = 0;

        ///
        /// Computes the derivative vector and hessian matrix for a specified
        /// individual of the Q matrix.
        ///
        virtual void compute_derivatives_q(
                const matrix_type & q,     ///< The Q matrix.
                const matrix_type & fa,    ///< The F matrix.
                const matrix_type & fb,    ///< The 1-F matrix.
                const matrix_type & qfa,   ///< The Q*F product.
                const matrix_type & qfb,   ///< The Q*(1-F) product.
                const size_t        i,     ///< The individual.
                matrix_type &       d_vec, ///< The derivative vector.
                matrix_type &       h_mat) ///< The hessian matrix.
                const = 0;

        ///
        /// \return The log of the likelihood function.
        ///
        virtual value_type compute_lle(
                const matrix_type & q,   ///< The Q matrix.
                const matrix_type & fa,  ///< The F matrix for major alleles.
                const matrix_type & fb,  ///< The F matrix for minor alleles.
                const matrix_type & qfa, ///< The Q*Fa product.
                const matrix_type & qfb) ///< The Q*Fb product.
                const = 0;

        ///
        /// \return A new mu matrix.
        ///
        virtual matrix_type create_mu() const = 0;

        ///
        /// \return The height of the matrix.
        ///
        virtual size_t get_height() const = 0;

        ///
        /// \return The string representation of the size of the matrix.
        ///
        virtual std::string get_size_str() const = 0;

        ///
        /// \return The width of the matrix.
        ///
        virtual size_t get_width() const = 0;

        ///
        /// \return The string representation of the matrix.
        ///
        virtual std::string str() const = 0;

    protected:
        ///
        /// Initializes a new instance of the class.
        ///
        inline basic_genotype_matrix()
        {
        }
    };
}

#endif // JADE_GENOTYPE_MATRIX_HPP__
