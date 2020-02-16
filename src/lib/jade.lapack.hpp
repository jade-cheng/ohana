/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_LAPACK_HPP__
#define JADE_LAPACK_HPP__

#include "jade.assert.hpp"

namespace jade
{
    ///
    /// A template for a class providing access to LAPACK.
    ///
    template <typename TValue>
    class basic_lapack
    {
    public:
        /// The value type.
        typedef TValue value_type;

#ifndef LAPACK_ROW_MAJOR
#define LAPACK_ROW_MAJOR 101
#endif

#ifndef LAPACK_COL_MAJOR
#define LAPACK_COL_MAJOR 102
#endif

        /// A type indicating whether the elements of a matrix are in row-major
        /// or column-major order.
        enum layout_type
        {
            col_major = LAPACK_COL_MAJOR, ///< A column-major order.
            row_major = LAPACK_ROW_MAJOR, ///< A row-major order.
        };

        ///
        /// Computes the solution to the system of linear equations with a
        /// square coefficient matrix A and multiple right-hand sides.
        ///
        /// Matrices must be in column-major order.
        ///
        /// \return Zero if successful; otherwise, non-zero.
        ///
        static int gesv(
            layout_type  layout, ///< The matrix layout.
            int          n,      ///< The number of equations.
            int          nrhs,   ///< The number of right-hand sides.
            value_type * a,      ///< The coefficient matrix.
            int          lda,    ///< The leading dimension of a.
            int        * ipiv,   ///< The pivot table.
            value_type * b,      ///< The right-hand sides.
            int          ldb)    ///< The leading dimension of b.
            ;

        ///
        /// Computes the Cholesky factorization of a symmetric (Hermitian)
        /// positive-definite matrix.
        ///
        /// Matrices must be in column-major order.
        ///
        /// \return Zero if successful; otherwise, non-zero.
        ///
        static int potrf(
            layout_type  layout, ///< The matrix layout.
            char         uplo,   ///< The upper-lower flag.
            int          n,      ///< The order of the matrix.
            value_type * a,      ///< The matrix data.
            int          lda)    ///< The stride of the matrix.
            ;

        ///
        /// Computes the inverse of a symmetric (Hermitian) positive-definite
        /// matrix using the Cholesky factorization.
        ///
        /// Matrices must be in column-major order.
        ///
        /// \return Zero if successful; otherwise, non-zero.
        ///
        static int potri(
            layout_type  layout, ///< The matrix layout.
            char         uplo,   ///< The upper-lower flag.
            int          n,      ///< The order of the matrix.
            value_type * a,      ///< The matrix data.
            int          lda)    ///< The stride of the matrix.
            ;

    private:
        // --------------------------------------------------------------------
        class col_storage
        {
            col_storage() = delete;
            col_storage(const col_storage &) = delete;
            col_storage & operator = (const col_storage &) = delete;

        public:
            // ----------------------------------------------------------------
            col_storage(
                    value_type ** data_ptr,
                    int *         stride_ptr,
                    int           rows,
                    int           cols)
                : _data_ptr   (data_ptr)
                , _stride_ptr (stride_ptr)
                , _data_0     (*data_ptr)
                , _stride_0   (size_t(*stride_ptr))
                , _rows       (size_t(rows))
                , _cols       (size_t(cols))
            {
                assert(nullptr != data_ptr);
                assert(nullptr != *data_ptr);
                assert(nullptr != stride_ptr);
                assert(*stride_ptr > 0);
                assert(rows > 0);
                assert(cols > 0);

                _temp.resize(_rows * _cols);
                auto t = _temp.data();

                for (size_t c = 0; c < _cols; c++)
                    for (size_t r = 0; r < _rows; r++)
                        *t++ = _data_0[r * _stride_0 + c];

                *data_ptr   = _temp.data();
                *stride_ptr = int(_rows);
            }

            // ----------------------------------------------------------------
            ~col_storage()
            {
                *_data_ptr   = _data_0;
                *_stride_ptr = int(_stride_0);

                auto t = _temp.data();
                for (size_t c = 0; c < _cols; c++)
                    for (size_t r = 0; r < _rows; r++)
                        _data_0[r * _stride_0 + c] = *t++;
            }

        private:
            value_type **           _data_ptr;
            int *                   _stride_ptr;
            value_type *            _data_0;
            size_t                  _stride_0;
            size_t                  _rows;
            size_t                  _cols;
            std::vector<value_type> _temp;
        };

        // ----------------------------------------------------------------
        static std::unique_ptr<col_storage> init_storage(
            layout_type   layout,
            value_type ** data_ptr,
            int *         stride_ptr,
            int           rows,
            int           cols)
        {
            assert(layout == row_major || layout == col_major);

            std::unique_ptr<col_storage> ptr;

            if (layout == row_major)
            {
                ptr.reset(new col_storage(
                    data_ptr,
                    stride_ptr,
                    rows,
                    cols));
            }

            return ptr;
        }
    };

    #ifndef DOXYGEN_IGNORE

    // ------------------------------------------------------------------------
    template <>
    inline int basic_lapack<double>::gesv(
        layout_type layout,
        int         n,
        int         nrhs,
        double *    a,
        int         lda,
        int *       ipiv,
        double *    b,
        int         ldb)
    {
        assert(a != nullptr);
        assert(b != nullptr);

#if defined(JADE_USE_ACCELERATE_FRAMEWORK)
        const auto a_storage = init_storage(layout, &a, &lda, n, n);
        const auto b_storage = init_storage(layout, &b, &ldb, n, nrhs);
        int info;
        ::dgesv_(&n, &nrhs, a, &lda, ipiv, b, &ldb, &info);
        return info;
#elif defined(JADE_USE_NETLIB_PACKAGES)
        return LAPACKE_dgesv(layout, n, nrhs, a, lda, ipiv, b, ldb);
#else
        #error Unsupported build environment
#endif
    }

    // ------------------------------------------------------------------------
    template <>
    inline int basic_lapack<float>::gesv(
        layout_type layout,
        int         n,
        int         nrhs,
        float *     a,
        int         lda,
        int *       ipiv,
        float *     b,
        int         ldb)
    {
        assert(a != nullptr);
        assert(b != nullptr);

#if defined(JADE_USE_ACCELERATE_FRAMEWORK)
        const auto a_storage = init_storage(layout, &a, &lda, n, n);
        const auto b_storage = init_storage(layout, &b, &ldb, n, nrhs);
        int info;
        (void)::sgesv_(&n, &nrhs, a, &lda, ipiv, b, &ldb, &info);
        return info;
#elif defined(JADE_USE_NETLIB_PACKAGES)
        return LAPACKE_sgesv(layout, n, nrhs, a, lda, ipiv, b, ldb);
#else
        #error Unsupported build environment
#endif
    }

    // ------------------------------------------------------------------------
    template <>
    inline int basic_lapack<double>::potrf(
        layout_type layout,
        char        uplo,
        int         n,
        double *    a,
        int         lda)
    {
        assert(a != nullptr);

#if defined(JADE_USE_ACCELERATE_FRAMEWORK)
        const auto storage = init_storage(layout, &a, &lda, n, n);
        int info;
        (void)::dpotrf_(&uplo, &n, a, &lda, &info);
        return info;
#elif defined(JADE_USE_NETLIB_PACKAGES)
        return LAPACKE_dpotrf(layout, uplo, n, a, lda);
#else
        #error Unsupported build environment
#endif
    }

    // ------------------------------------------------------------------------
    template <>
    inline int basic_lapack<float>::potrf(
        layout_type layout,
        char        uplo,
        int         n,
        float *     a,
        int         lda)
    {
        assert(a != nullptr);

#if defined(JADE_USE_ACCELERATE_FRAMEWORK)
        const auto storage = init_storage(layout, &a, &lda, n, n);
        int info;
        (void)::spotrf_(&uplo, &n, a, &lda, &info);
        return info;
#elif defined(JADE_USE_NETLIB_PACKAGES)
        return LAPACKE_spotrf(layout, uplo, n, a, lda);
#else
        #error Unsupported build environment
#endif
    }

    // ------------------------------------------------------------------------
    template <>
    inline int basic_lapack<double>::potri(
        layout_type layout,
        char        uplo,
        int         n,
        double *    a,
        int         lda)
    {
        assert(a != nullptr);

#if defined(JADE_USE_ACCELERATE_FRAMEWORK)
        const auto storage = init_storage(layout, &a, &lda, n, n);
        int info;
        (void)::dpotri_(&uplo, &n, a, &lda, &info);
        return info;
#elif defined(JADE_USE_NETLIB_PACKAGES)
        return LAPACKE_dpotri(layout, uplo, n, a, lda);
#else
        #error Unsupported build environment
#endif
    }

    // ------------------------------------------------------------------------
    template <>
    inline int basic_lapack<float>::potri(
        layout_type layout,
        char        uplo,
        int         n,
        float *     a,
        int         lda)
    {
        assert(a != nullptr);

#if defined(JADE_USE_ACCELERATE_FRAMEWORK)
        const auto storage = init_storage(layout, &a, &lda, n, n);
        int info;
        (void)::spotri_(&uplo, &n, a, &lda, &info);
        return info;
#elif defined(JADE_USE_NETLIB_PACKAGES)
        return LAPACKE_spotri(layout, uplo, n, a, lda);
#else
        #error Unsupported build environment
#endif
    }

    #endif // DOXYGEN_IGNORE
}

#endif // JADE_LAPACK_HPP__
