/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_BLAS_HPP__
#define JADE_BLAS_HPP__

#include "jade.assert.hpp"

namespace jade
{
    ///
    /// A template for a class providing access to BLAS.
    ///
    template <typename TValue>
    class basic_blas
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// A type indicating whether the elements of a matrix are in row-major
        /// or column-major order.
        typedef CBLAS_ORDER layout_type;

        /// A type indicating a kind of transpose operation to perform on a
        /// matrix.
        typedef CBLAS_TRANSPOSE transpose_type;

        ///
        /// Computes a vector-vector dot product.
        ///
        /// \return The result of the dot product of x and y, if n is positive.
        /// Otherwise, returns 0.
        ///
        static value_type dot(
            const int          n,    ///< The number of elements.
            const value_type * x,    ///< The X array.
            const int          incx, ///< The X increment.
            const value_type * y,    ///< The Y array.
            const int          incy) ///< The Y increment.
            ;

        ///
        /// Computes a matrix-matrix product with general matrices.
        ///
        static void gemm(
            const layout_type    Order,  ///< The order.
            const transpose_type TransA, ///< Transpose A.
            const transpose_type TransB, ///< Transpose B.
            const int            M,      ///< Rows of A.
            const int            N,      ///< Columns of B.
            const int            K,      ///< The order.
            const value_type     alpha,  ///< Alpha scalar.
            const value_type *   A,      ///< A matrix.
            const int            lda,    ///< Stride of A.
            const value_type *   B,      ///< B matrix.
            const int            ldb,    ///< Stride of B.
            const value_type     beta,   ///< Beta scalar.
            value_type *         C,      ///< C matrix.
            const int            ldc)    ///< Stride of C.
            ;

        ///
        /// Computes a matrix-vector product using a general matrix.
        ///
        static void gemv(
            const layout_type    order, ///< The layout.
            const transpose_type trans, ///< Transpose.
            const int            m,     ///< Rows of A.
            const int            n,     ///< Columns of A.
            const value_type     alpha, ///< Alpha scalar.
            const value_type *   a,     ///< A matrix.
            const int            lda,   ///< Stride of A.
            const value_type *   x,     ///< X vector.
            const int            incx,  ///< Stride of X.
            const value_type     beta,  ///< Beta scalar.
            value_type *         y,     ///< Y vector.
            const int            incy)  ///< Stride of Y.
            ;
    };

    #ifndef DOXYGEN_IGNORE

    // ------------------------------------------------------------------------
    template <>
    inline double basic_blas<double>::dot(
        const int      n,
        const double * x,
        const int      incx,
        const double * y,
        const int      incy)
    {
        assert(x != nullptr);
        assert(y != nullptr);

        return ::cblas_ddot(n, x, incx, y, incy);
    }

    // ------------------------------------------------------------------------
    template <>
    inline float basic_blas<float>::dot(
        const int     n,
        const float * x,
        const int     incx,
        const float * y,
        const int     incy)
    {
        assert(x != nullptr);
        assert(y != nullptr);

        return ::cblas_sdot(n, x, incx, y, incy);
    }

    // ------------------------------------------------------------------------
    template <>
    inline void basic_blas<double>::gemm(
        const layout_type    Order,
        const transpose_type TransA,
        const transpose_type TransB,
        const int            M,
        const int            N,
        const int            K,
        const double         alpha,
        const double *       A,
        const int            lda,
        const double *       B,
        const int            ldb,
        const double         beta,
        double *             C,
        const int            ldc)
    {
        assert(A != nullptr);
        assert(B != nullptr);
        assert(C != nullptr);

        ::cblas_dgemm(
            Order, TransA, TransB, M, N, K, alpha,
            A, lda, B, ldb, beta, C, ldc);
    }

    // ------------------------------------------------------------------------
    template <>
    inline void basic_blas<float>::gemm(
        const layout_type    Order,
        const transpose_type TransA,
        const transpose_type TransB,
        const int            M,
        const int            N,
        const int            K,
        const float          alpha,
        const float *        A,
        const int            lda,
        const float *        B,
        const int            ldb,
        const float          beta,
        float *              C,
        const int            ldc)
    {
        assert(A != nullptr);
        assert(B != nullptr);
        assert(C != nullptr);

        ::cblas_sgemm(
            Order, TransA, TransB, M, N, K, alpha,
            A, lda, B, ldb, beta, C, ldc);
    }

    // ------------------------------------------------------------------------
    template <>
    inline void basic_blas<double>::gemv(
        const layout_type    order,
        const transpose_type trans,
        const int            m,
        const int            n,
        const double         alpha,
        const double *       a,
        const int            lda,
        const double *       x,
        const int            incx,
        const double         beta,
        double *             y,
        const int            incy)
    {
        assert(a != nullptr);
        assert(x != nullptr);
        assert(y != nullptr);

        ::cblas_dgemv(order, trans, m, n, alpha,
            a, lda, x, incx, beta, y, incy);
    }

    // ------------------------------------------------------------------------
    template <>
    inline void basic_blas<float>::gemv(
        const layout_type    order,
        const transpose_type trans,
        const int            m,
        const int            n,
        const float          alpha,
        const float *        a,
        const int            lda,
        const float *        x,
        const int            incx,
        const float          beta,
        float *              y,
        const int            incy)
    {
        assert(a != nullptr);
        assert(x != nullptr);
        assert(y != nullptr);

        ::cblas_sgemv(order, trans, m, n, alpha,
            a, lda, x, incx, beta, y, incy);
    }

    #endif // DOXYGEN_IGNORE
}

#endif // JADE_BLAS_HPP__
