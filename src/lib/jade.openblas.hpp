/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_OPENBLAS_HPP__
#define JADE_OPENBLAS_HPP__

#include "jade.assert.hpp"

namespace jade
{
    ///
    /// A template for a class providing access to OpenBLAS.
    ///
    template <typename TValue>
    class basic_openblas
    {
    public:
        /// The value type.
        typedef TValue value_type;

        ///
        /// Computes a vector-vector dot product.
        ///
        /// \return The result of the dot product of x and y, if n is positive.
        /// Otherwise, returns 0.
        ///
        static value_type dot(
                OPENBLAS_CONST blasint      n,    ///< The number of elements.
                OPENBLAS_CONST value_type * x,    ///< The X array.
                OPENBLAS_CONST blasint      incx, ///< The X increment.
                OPENBLAS_CONST value_type * y,    ///< The Y array.
                OPENBLAS_CONST blasint      incy) ///< The Y increment.
                ;

        ///
        /// Computes a matrix-matrix product with general matrices.
        ///
        static void gemm(
                OPENBLAS_CONST enum CBLAS_ORDER     Order,  ///< The order.
                OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA, ///< Transpose A.
                OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB, ///< Transpose B.
                OPENBLAS_CONST blasint              M,      ///< Rows of A.
                OPENBLAS_CONST blasint              N,      ///< Columns of B.
                OPENBLAS_CONST blasint              K,      ///< The order.
                OPENBLAS_CONST value_type           alpha,  ///< Alpha scalar.
                OPENBLAS_CONST value_type *         A,      ///< A matrix.
                OPENBLAS_CONST blasint              lda,    ///< Stride of A.
                OPENBLAS_CONST value_type *         B,      ///< B matrix.
                OPENBLAS_CONST blasint              ldb,    ///< Stride of B.
                OPENBLAS_CONST value_type           beta,   ///< Beta scalar.
                value_type *                        C,      ///< C matrix.
                OPENBLAS_CONST blasint              ldc)    ///< Stride of C.
                ;

        ///
        /// Computes a matrix-vector product using a general matrix.
        ///
        static void gemv(
                OPENBLAS_CONST enum CBLAS_ORDER     order, ///< The layout.
                OPENBLAS_CONST enum CBLAS_TRANSPOSE trans, ///< Transpose.
                OPENBLAS_CONST blasint              m,     ///< Rows of A.
                OPENBLAS_CONST blasint              n,     ///< Columns of A.
                OPENBLAS_CONST value_type           alpha, ///< Alpha scalar.
                OPENBLAS_CONST value_type *         a,     ///< A matrix.
                OPENBLAS_CONST blasint              lda,   ///< Stride of A.
                OPENBLAS_CONST value_type *         x,     ///< X vector.
                OPENBLAS_CONST blasint              incx,  ///< Stride of X.
                OPENBLAS_CONST value_type           beta,  ///< Beta scalar.
                value_type *                        y,     ///< Y vector.
                OPENBLAS_CONST blasint              incy)  ///< Stride of Y.
                ;

        ///
        /// Computes the solution to the system of linear equations with a
        /// square coefficient matrix A and multiple right-hand sides.
        ///
        /// \return Zero if successful; otherwise, non-zero.
        ///
        static lapack_int gesv(
                int          matrix_order, ///< The storate layout.
                lapack_int   n,            ///< The number of equations.
                lapack_int   nrhs,         ///< The number of right-hand sides.
                value_type * a,            ///< The coefficient matrix.
                lapack_int   lda,          ///< The leading dimension of a.
                lapack_int * ipiv,         ///< The pivot table.
                value_type * b,            ///< The right-hand sides.
                lapack_int   ldb)          ///< The leading dimension of b.
                ;

        ///
        /// Computes the Cholesky factorization of a symmetric (Hermitian)
        /// positive-definite matrix.
        ///
        /// \return Zero if successful; otherwise, non-zero.
        ///
        static lapack_int potrf(
                int          matrix_order, ///< The matrix storage layout.
                char         uplo,         ///< The upper-lower flag.
                lapack_int   n,            ///< The order of the matrix.
                value_type * a,            ///< The matrix data.
                lapack_int   lda)          ///< The stride of the matrix.
                ;

        ///
        /// Computes the inverse of a symmetric (Hermitian) positive-definite
        /// matrix using the Cholesky factorization.
        ///
        /// \return Zero if successful; otherwise, non-zero.
        ///
        static lapack_int potri(
                int          matrix_order, ///< The matrix storage layout.
                char         uplo,         ///< The upper-lower flag.
                lapack_int   n,            ///< The order of the matrix.
                value_type * a,            ///< The matrix data.
                lapack_int   lda)          ///< The stride of the matrix.
                ;
    };

    #ifndef DOXYGEN_IGNORE

    // ------------------------------------------------------------------------
    template <>
    inline double basic_openblas<double>::dot(
            OPENBLAS_CONST blasint  n,
            OPENBLAS_CONST double * x,
            OPENBLAS_CONST blasint  incx,
            OPENBLAS_CONST double * y,
            OPENBLAS_CONST blasint  incy)
    {
        assert(x != nullptr);
        assert(y != nullptr);

        return ::cblas_ddot(n, x, incx, y, incy);
    }

    // ------------------------------------------------------------------------
    template <>
    inline float basic_openblas<float>::dot(
            OPENBLAS_CONST blasint  n,
            OPENBLAS_CONST float *  x,
            OPENBLAS_CONST blasint  incx,
            OPENBLAS_CONST float *  y,
            OPENBLAS_CONST blasint  incy)
    {
        assert(x != nullptr);
        assert(y != nullptr);

        return ::cblas_sdot(n, x, incx, y, incy);
    }

    // ------------------------------------------------------------------------
    template <>
    inline void basic_openblas<double>::gemm(
            OPENBLAS_CONST enum CBLAS_ORDER     Order,
            OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
            OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB,
            OPENBLAS_CONST blasint              M,
            OPENBLAS_CONST blasint              N,
            OPENBLAS_CONST blasint              K,
            OPENBLAS_CONST double               alpha,
            OPENBLAS_CONST double *             A,
            OPENBLAS_CONST blasint              lda,
            OPENBLAS_CONST double *             B,
            OPENBLAS_CONST blasint              ldb,
            OPENBLAS_CONST double               beta,
            double *                            C,
            OPENBLAS_CONST blasint              ldc)
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
    inline void basic_openblas<float>::gemm(
            OPENBLAS_CONST enum CBLAS_ORDER     Order,
            OPENBLAS_CONST enum CBLAS_TRANSPOSE TransA,
            OPENBLAS_CONST enum CBLAS_TRANSPOSE TransB,
            OPENBLAS_CONST blasint              M,
            OPENBLAS_CONST blasint              N,
            OPENBLAS_CONST blasint              K,
            OPENBLAS_CONST float                alpha,
            OPENBLAS_CONST float *              A,
            OPENBLAS_CONST blasint              lda,
            OPENBLAS_CONST float *              B,
            OPENBLAS_CONST blasint              ldb,
            OPENBLAS_CONST float                beta,
            float *                             C,
            OPENBLAS_CONST blasint              ldc)
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
    inline void basic_openblas<double>::gemv(
            OPENBLAS_CONST enum CBLAS_ORDER     order,
            OPENBLAS_CONST enum CBLAS_TRANSPOSE trans,
            OPENBLAS_CONST blasint              m,
            OPENBLAS_CONST blasint              n,
            OPENBLAS_CONST double               alpha,
            OPENBLAS_CONST double *             a,
            OPENBLAS_CONST blasint              lda,
            OPENBLAS_CONST double *             x,
            OPENBLAS_CONST blasint              incx,
            OPENBLAS_CONST double               beta,
            double *                            y,
            OPENBLAS_CONST blasint              incy)
    {
        assert(a != nullptr);
        assert(x != nullptr);
        assert(y != nullptr);

        ::cblas_dgemv(order, trans, m, n, alpha,
            a, lda, x, incx, beta, y, incy);
    }

    // ------------------------------------------------------------------------
    template <>
    inline void basic_openblas<float>::gemv(
            OPENBLAS_CONST enum CBLAS_ORDER     order,
            OPENBLAS_CONST enum CBLAS_TRANSPOSE trans,
            OPENBLAS_CONST blasint              m,
            OPENBLAS_CONST blasint              n,
            OPENBLAS_CONST float                alpha,
            OPENBLAS_CONST float *              a,
            OPENBLAS_CONST blasint              lda,
            OPENBLAS_CONST float *              x,
            OPENBLAS_CONST blasint              incx,
            OPENBLAS_CONST float                beta,
            float *                             y,
            OPENBLAS_CONST blasint              incy)
    {
        assert(a != nullptr);
        assert(x != nullptr);
        assert(y != nullptr);

        ::cblas_sgemv(order, trans, m, n, alpha,
            a, lda, x, incx, beta, y, incy);
    }

    // ------------------------------------------------------------------------
    template <>
    inline lapack_int basic_openblas<double>::gesv(
            int          matrix_order,
            lapack_int   n,
            lapack_int   nrhs,
            double *     a,
            lapack_int   lda,
            lapack_int * ipiv,
            double *     b,
            lapack_int   ldb)
    {
        assert(a != nullptr);
        assert(b != nullptr);

        return ::LAPACKE_dgesv(matrix_order, n, nrhs, a, lda, ipiv, b, ldb);
    }

    // ------------------------------------------------------------------------
    template <>
    inline lapack_int basic_openblas<float>::gesv(
            int          matrix_order,
            lapack_int   n,
            lapack_int   nrhs,
            float *      a,
            lapack_int   lda,
            lapack_int * ipiv,
            float *      b,
            lapack_int   ldb)
    {
        assert(a != nullptr);
        assert(b != nullptr);

        return ::LAPACKE_sgesv(matrix_order, n, nrhs, a, lda, ipiv, b, ldb);
    }

    // ------------------------------------------------------------------------
    template <>
    inline lapack_int basic_openblas<double>::potrf(
            int          matrix_order,
            char         uplo,
            lapack_int   n,
            double *     a,
            lapack_int   lda)
    {
        assert(a != nullptr);

        return ::LAPACKE_dpotrf(matrix_order, uplo, n, a, lda);
    }

    // ------------------------------------------------------------------------
    template <>
    inline lapack_int basic_openblas<float>::potrf(
            int          matrix_order,
            char         uplo,
            lapack_int   n,
            float *      a,
            lapack_int   lda)
    {
        assert(a != nullptr);

        return ::LAPACKE_spotrf(matrix_order, uplo, n, a, lda);
    }

    // ------------------------------------------------------------------------
    template <>
    inline lapack_int basic_openblas<double>::potri(
            int          matrix_order,
            char         uplo,
            lapack_int   n,
            double *     a,
            lapack_int   lda)
    {
        assert(a != nullptr);

        return ::LAPACKE_dpotri(matrix_order, uplo, n, a, lda);
    }

    // ------------------------------------------------------------------------
    template <>
    inline lapack_int basic_openblas<float>::potri(
            int          matrix_order,
            char         uplo,
            lapack_int   n,
            float *      a,
            lapack_int   lda)
    {
        assert(a != nullptr);

        return ::LAPACKE_spotri(matrix_order, uplo, n, a, lda);
    }

    #endif // DOXYGEN_IGNORE
}

#endif // JADE_OPENBLAS_HPP__
