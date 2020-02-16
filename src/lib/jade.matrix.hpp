/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_MATRIX_HPP__
#define JADE_MATRIX_HPP__

#include "jade.blas.hpp"
#include "jade.error.hpp"
#include "jade.lapack.hpp"

namespace jade
{
    ///
    /// A template class for a matrix.
    ///
    template <typename TValue>
    class basic_matrix
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The BLAS type.
        typedef basic_blas<value_type> blas_type;

        /// The LAPACK type.
        typedef basic_lapack<value_type> lapack_type;

        /// The initializer list type.
        typedef std::initializer_list<
            std::initializer_list<value_type>
        > initializer_list_type;

        ///
        /// Initializes a new instance of the class with no size.
        ///
        inline basic_matrix()
            : basic_matrix(0, 0)
        {
        }

        ///
        /// Initializes a new instance of the class based on the specified
        /// width and height.
        ///
        basic_matrix(
                const size_t cy, ///< The height.
                const size_t cx) ///< The width.
            : _cy (cy)
            , _cx (cx)
            , _m  ()
        {
            assert((cx == 0 && cy == 0) || (cx != 0 && cy != 0));
            _m.resize(cx * cy);
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        inline explicit basic_matrix(
                const char * const path) ///< The path to the file.
            : basic_matrix()
        {
            read(path);
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        inline explicit basic_matrix(
                const std::string & path) ///< The path to the file.
            : basic_matrix(path.c_str())
        {
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified input stream.
        ///
        inline explicit basic_matrix(
                std::istream & in) ///< The input stream.
            : basic_matrix()
        {
            read(in);
        }

        ///
        /// Initializes a new instance of the class based on the specified
        /// values.
        ///
        /// \param values The two-dimensional values.
        ///
        explicit basic_matrix(const initializer_list_type & values)
            : _cy (values.size())
            , _cx (_cy == 0 ? 0 : values.begin()->size())
            , _m  ()
        {
            assert(std::all_of(values.begin(), values.end(), [this](
                const std::initializer_list<value_type> & row_values) -> bool {
                    return row_values.size() == _cx; }));

            _m.reserve(_cy * _cx);
            for (const auto row : values)
                for (const auto n : row)
                    _m.push_back(n);
        }

        ///
        /// \return True if the specified predicate is true for all values.
        ///
        template <typename TPredicate>
        inline bool all_of(
                const TPredicate predicate) ///< The predicate function.
                const
        {
            return std::all_of(_m.begin(), _m.end(), predicate);
        }

        ///
        /// \return True if the specified predicate is true for any value.
        ///
        template <typename TPredicate>
        inline bool any_of(
                const TPredicate predicate) ///< The predicate function.
                const
        {
            return std::any_of(_m.begin(), _m.end(), predicate);
        }

        ///
        /// Clamps all values in the matrix to the specified range.
        ///
        void clamp(
                const value_type min, ///< The minimum value.
                const value_type max) ///< The maximum value.
        {
            assert(min <= max);

            auto       dst_ptr = get_data();
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
            {
                *dst_ptr = std::min(std::max(min, *dst_ptr), max);
                dst_ptr++;
            }
        }

        ///
        /// Clamps all values in a column to the specified range.
        ///
        void clamp_column(
                const size_t     column, ///< The column to clamp.
                const value_type min,    ///< The minimum value.
                const value_type max)    ///< The maximum value.
        {
            assert(column < get_width());
            assert(min <= max);

            auto       dst_ptr = get_data(0, column);
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
            {
                *dst_ptr = std::min(std::max(min, *dst_ptr), max);
                dst_ptr += get_width();
            }
        }

        ///
        /// Clamps all values in a row to the specified range.
        ///
        void clamp_row(
                const size_t     row, ///< The row to clamp.
                const value_type min, ///< The minimum value.
                const value_type max) ///< The maximum value.
        {
            assert(row < get_height());
            assert(min <= max);

            auto       dst_ptr = get_data(row, 0);
            const auto dst_end = dst_ptr + get_width();

            while (dst_ptr != dst_end)
            {
                *dst_ptr = std::min(std::max(min, *dst_ptr), max);
                dst_ptr++;
            }
        }

        ///
        /// \return True if any element in the matrix is infinite.
        ///
        bool contains_inf() const
        {
            auto       src_ptr = get_data();
            const auto src_end = src_ptr + get_length();

            while (src_ptr != src_end)
                if (std::isinf(*src_ptr++))
                    return true;

            return false;
        }

        ///
        /// \return True if any element in the matrix is not a number.
        ///
        bool contains_nan() const
        {
            auto       src_ptr = get_data();
            const auto src_end = src_ptr + get_length();

            while (src_ptr != src_end)
                if (std::isnan(*src_ptr++))
                    return true;

            return false;
        }

        ///
        /// \return A new column vector based on the values from the specified
        /// column.
        ///
        basic_matrix copy_column(
                const size_t column) ///< The column to copy.
                const
        {
            basic_matrix out;
            copy_column(column, out);
            return out;
        }

        ///
        /// Copies a column into the specified vector.
        ///
        void copy_column(
                const size_t   column, ///< The column to copy.
                basic_matrix & out)    ///< The output vector.
                const
        {
            assert(column < get_width());

            out.resize(get_height(), 1);

            auto       dst_ptr = out.get_data();
            auto       src_ptr = get_data(0, column);
            const auto src_end = src_ptr + get_length();

            while (src_ptr != src_end)
            {
                *dst_ptr++ = *src_ptr;
                src_ptr += get_width();
            }
        }

        ///
        /// Copies the elements in the lower triangle of the square matrix to
        /// the corresponding upper triangle elements.
        ///
        void copy_lower_to_upper()
        {
            assert(is_square());

            const auto n = get_height();
            if (n < 2)
                return;

            //
            // Prepare pointers to the data.
            //
            const auto dst_end = get_data() + get_length();
            auto       dst_ptr = get_data() + 1;
            auto       src_end = get_data() + get_length();
            auto       src_ptr = get_data() + n;

            //
            // Loop until the entire triangle is copied.
            //
            while (dst_ptr != dst_end)
            {
                const auto dst_anchor = dst_ptr;
                const auto src_anchor = src_ptr;

                //
                // Advance down the row of the lower triangle and across the
                // column of the upper triangle.
                //
                while (src_ptr != src_end)
                {
                    *dst_ptr++ = *src_ptr;
                    src_ptr += n;
                }

                //
                // Adjust the pointers for the next row and column.
                //
                dst_ptr = dst_anchor + n + 1;
                src_ptr = src_anchor + n + 1;
                src_end++;
            }
        }

        ///
        /// \return A new row vector based on the values from the specified row.
        ///
        basic_matrix copy_row(
                const size_t row) ///< The row to copy.
                const
        {
            basic_matrix out;
            copy_row(row, out);
            return out;
        }

        ///
        /// Copies a row into the specified matrix.
        ///
        void copy_row(
                const size_t   row, ///< The row to copy.
                basic_matrix & out) ///< The output vector.
                const
        {
            assert(row < get_height());

            out.resize(1, get_width());

            auto       dst_ptr = out.get_data();
            auto       src_ptr = get_data(row, 0);
            const auto src_end = src_ptr + get_width();

            while (src_ptr != src_end)
                *dst_ptr++ = *src_ptr++;
        }

        ///
        /// Copies the elements in the upper triangle of the square matrix to
        /// the corresponding lower triangle elements.
        ///
        void copy_upper_to_lower()
        {
            assert(is_square());

            const auto n = get_height();
            if (n < 2)
                return;

            //
            // Prepare pointers to the data.
            //
            const auto src_end = get_data() + get_length();
            auto       src_ptr = get_data() + 1;
            auto       dst_end = get_data() + get_length();
            auto       dst_ptr = get_data() + n;

            //
            // Loop until the entire triangle is copied.
            //
            while (src_ptr != src_end)
            {
                const auto dst_anchor = dst_ptr;
                const auto src_anchor = src_ptr;

                //
                // Advance down the row of the lower triangle and across the
                // column of the upper triangle.
                //
                while (dst_ptr != dst_end)
                {
                    *dst_ptr = *src_ptr++;
                    dst_ptr += n;
                }

                //
                // Adjust the pointers for the next row and column.
                //
                dst_ptr = dst_anchor + n + 1;
                src_ptr = src_anchor + n + 1;
                dst_end++;
            }
        }

        ///
        /// \return The transpose of the matrix.
        ///
        inline basic_matrix create_transpose() const
        {
            basic_matrix out;
            create_transpose(out);
            return out;
        }

        ///
        /// Stores the transpose of the matrix.
        ///
        void create_transpose(
                basic_matrix & out) ///< The matrix receiving the transpose.
                const
        {
            if (this == &out)
            {
                if (is_empty())
                    return;

                if (is_vector())
                {
                    std::swap(out._cx, out._cy);
                    return;
                }

                if (is_square())
                {
                    for (size_t i = 1; i < _cy; i++)
                        for (size_t j = 0; j < i; j++)
                            std::swap(out(i, j), out(j, i));
                    return;
                }

                basic_matrix tmp;
                create_transpose(tmp);
                out.swap(tmp);
                return;
            }

            if (is_empty() || is_vector() || is_square())
            {
                out = *this;
                out.create_transpose(out);
                return;
            }

            out.resize(_cx, _cy);

            auto & src = *this;
            for (size_t i = 0; i < _cy; i++)
                for (size_t j = 0; j < _cx; j++)
                    out(j, i) = src(i, j);
        }

        ///
        /// Multiplies a left matrix by a right matrix and stores the result
        /// into a destination matrix. If the beta parameter is provided, the
        /// initial values of the destination matrix are scaled by beta, and the
        /// product of the left and right matrices is added to the destination
        /// matrix. If the alpha parameter is provided, the product of the left
        /// and right matrices is scaled by alpha.
        ///
        /// \param lhs   The matrix left of the operator.
        /// \param rhs   The matrix right of the operator.
        /// \param dst   The matrix receiving the product.
        /// \param alpha The scalar applied to the left matrix.
        /// \param beta  The scalar applied to the right matrix.
        ///
        static void gemm(
            const basic_matrix & lhs,
            const basic_matrix & rhs,
            basic_matrix &       dst,
            const value_type     alpha = value_type(1),
            const value_type     beta  = value_type(0))
        {
            assert(lhs.get_width() == rhs.get_height());
            assert(dst.is_size(lhs.get_height(), rhs.get_width()));

            blas_type::gemm(
                CblasRowMajor,         // Layout
                CblasNoTrans,          // transa
                CblasNoTrans,          // transb
                int(lhs.get_height()), // m
                int(rhs.get_width()),  // n
                int(lhs.get_width()),  // k
                alpha,                 // alpha
                lhs.get_data(),        // a
                int(lhs.get_width()),  // lda
                rhs.get_data(),        // b
                int(rhs.get_width()),  // ldb
                beta,                  // beta
                dst.get_data(),        // c
                int(dst.get_width())); // ldc
        }

        ///
        /// Computes the solution to the system of linear equations with a
        /// square coefficient matrix A and multiple right-hand sides.
        /// Specifically, this method solves for X the system of linear
        /// equations A*X = B, where A is an n-by-n matrix, the columns of
        /// matrix B are individual right-hand sides, and the columns of X are
        /// the corresponding solutions.
        ///
        /// \return True if successful; otherwise, false.
        ///
        bool gesv()
        {
            const auto cx = get_width();
            const auto cy = get_height();

            assert(cx > cy);

            std::vector<int> ipiv;
            ipiv.resize(cy);

            return 0 == lapack_type::gesv(
                    lapack_type::row_major, // layout
                    int(cy),                // n
                    int(cx - cy),           // nrhs
                    get_data(),             // a
                    int(cx),                // lda
                    ipiv.data(),            // ipiv
                    get_data() + cy,        // b
                    int(cx));               // ldb
        }

        ///
        /// \return The sum of a column.
        ///
        value_type get_column_sum(
                const size_t column) ///< The column to sum.
                const
        {
            assert(column < get_width());

            auto       sum = value_type(0);
            auto       ptr = get_data(0, column);
            const auto end = ptr + get_length();

            while (ptr != end)
            {
                sum += *ptr;
                ptr += get_width();
            }

            return sum;
        }

        ///
        /// \return A pointer to the first element of the data.
        ///
        inline const value_type * get_data() const
        {
            assert(!is_empty());
            return _m.data();
        }

        ///
        /// \return A pointer to the first element of the data.
        ///
        inline value_type * get_data()
        {
            assert(!is_empty());
            return _m.data();
        }

        ///
        /// \return A pointer to the specified element of the vector data.
        ///
        inline const value_type * get_data(
                const size_t index) ///< The index of the element.
                const
        {
            assert(is_vector());
            assert(index < get_length());
            return _m.data() + index;
        }

        ///
        /// \return A pointer to the specified element of the vector data.
        ///
        inline value_type * get_data(
                const size_t index) ///< The index of the element.
        {
            assert(index < get_length());
            return _m.data() + index;
        }

        ///
        /// \return AReturns a pointer to the specified element of the vector data.
        ///
        inline const value_type * get_data(
                const size_t row,    ///< The row.
                const size_t column) ///< The column.
                const
        {
            return _m.data() + get_index(row, column);
        }

        ///
        /// \return A pointer to the specified element of the vector data.
        ///
        inline value_type * get_data(
                const size_t row,    ///< The row of the element.
                const size_t column) ///< The column of the element.
        {
            return _m.data() + get_index(row, column);
        }

        ///
        /// \return The height of the matrix.
        ///
        inline size_t get_height() const
        {
            return _cy;
        }

        ///
        /// \return The element index for the specified row and column.
        ///
        inline size_t get_index(
                const size_t row,    ///< The row.
                const size_t column) ///< The column.
                const
        {
            assert(row < _cy);
            assert(column < _cx);
            return row * _cx + column;
        }

        ///
        /// \return The length of the matrix.
        ///
        size_t get_length() const
        {
            return _m.size();
        }

        ///
        /// Returns the maximum element value. This method should not be called
        /// if the matrix is empty.
        ///
        /// \return The maximum element value.
        ///
        value_type get_max_value() const
        {
            assert(!is_empty());
            return *std::max_element(_m.begin(), _m.end());
        }

        ///
        /// Returns the minimum element value. This method should not be called
        /// if the matrix is empty.
        ///
        /// \return The minimum element value.
        ///
        value_type get_min_value() const
        {
            assert(!is_empty());
            return *std::min_element(_m.begin(), _m.end());
        }

        ///
        /// Returns the minimum and maximum elements in the matrix. If the
        /// matrix is empty, this method returns false. Otherwise, this method
        /// stores the results in the specified arguments and returns true.
        ///
        /// \param min The minimum value (output).
        /// \param max The maximum value (output).
        ///
        /// \return True if successful; otherwise, false.
        ///
        bool get_min_max(value_type & min, value_type & max) const
        {
            if (_m.empty())
                return false;

            const auto p = *std::minmax_element(_m.begin(), _m.end());
            min = p.first;
            max = p.second;
            return true;
        }


        ///
        /// Returns the minimum and maximum elements in a column. If the
        /// matrix is empty, this method returns false. Otherwise, this method
        /// stores the results in the specified arguments and returns true.
        ///
        /// \param column The column index.
        /// \param min    The minimum value (output).
        /// \param max    The maximum value (output).
        ///
        /// \return True if successful; otherwise, false.
        ///
        bool get_min_max_column(
            const size_t column, ///< The column index.
            value_type & min,    ///< The minimum column element.
            value_type & max)    ///< The maximum column element.
            const
        {
            assert(column < get_width());

            if (_m.empty())
                return false;

            auto       ptr = get_data(0, column);
            auto const end = ptr + get_length();

            min = max = *ptr;

            for (ptr += get_width(); ptr < end; ptr += get_width())
            {
                const auto v = *ptr;
                min = std::min(min, v);
                max = std::max(max, v);
            }

            return true;
        }


        ///
        /// \return The sum of a row.
        ///
        value_type get_row_sum(
                const size_t row) ///< The row to sum.
                const
        {
            assert(row < get_height());

            auto       sum = value_type(0);
            auto       ptr = get_data(row, 0);
            const auto end = ptr + get_width();

            while (ptr < end)
                sum += *ptr++;

            return sum;
        }

        ///
        /// \return A string representation of the size of the matrix.
        ///
        std::string get_size_str() const
        {
            std::ostringstream out;
            out << '[' << _cy << 'x' << _cx << ']';
            return out.str();
        }

        ///
        /// \return The sum of all elements in the matrix.
        ///
        value_type get_sum() const
        {
            return std::accumulate(
                    _m.begin(),
                    _m.end(),
                    value_type(0),
                    std::plus<value_type>());
        }

        ///
        /// \return The height of the matrix.
        ///
        inline size_t get_width() const
        {
            return _cx;
        }

        ///
        /// \return Thevalue at the specified index in the vector data.
        ///
        inline value_type get_value(
                const size_t index) ///< The index in the vector data.
                const
        {
            assert(index < get_length());
            return _m[index];
        }

        ///
        /// \return The value at the specified row and column.
        ///
        inline value_type get_value(
                const size_t row,    ///< The row index.
                const size_t column) ///< The column index.
                const
        {
            const auto index = get_index(row, column);
            return _m[index];
        }

        ///
        /// Computes and stores the inverse of this matrix using the Cholesky
        /// square root method. Only the lower-left triangle is used to perform
        /// the calculation. If this method fails, indicating this matrix is
        /// not positive semidefinite, then the contents of this instance and
        /// the parameter are left undefined, and the method returns false.
        /// Otherwise, the lower triangle is copied to the upper triangle,
        /// the parameter is assigned the log of the determinant of the
        /// original matrix values, and the method returns true.
        /// \return True if successful; otherwise, false.
        ///
        bool invert(
                value_type & log_det) ///< The log of the determinant.
        {
            assert(is_square());

            //
            // Compute the Cholesky square root. If it fails, return false to
            // indicate this is an unacceptable set of parameters.
            //
            if (!potrf_lower())
                return false;

            //
            // Calculate the log of the determinant by summing twice the log of
            // the diagonal entries.
            //
            log_det = value_type(0.0);
            {
                const auto end = get_data() + get_length() + _cx;
                for (auto ptr = get_data(); ptr != end; ptr += _cx + 1)
                    log_det += value_type(2.0) * std::log(*ptr);
            }

            //
            // Compute the inverse. If it fails, return false to indicate
            // this is an unacceptable set of parameters.
            //
            if (!potri_lower())
                return false;

            //
            // Mirror the values from the lower triangle to the upper triangle.
            //
            copy_lower_to_upper();
            return true;
        }

        ///
        /// Computes and stores the inverse of this matrix using the Cholesky
        /// square root method. Only the lower-left triangle is used to perform
        /// the calculation. If this method fails, indicating this matrix is
        /// not positive semidefinite, then the contents of this instance is
        /// left undefined, and the method returns false. Otherwise, the lower
        /// triangle is copied to the upper triangle, and the method returns
        /// true.
        /// \return True if successful; otherwise, false.
        ///
        inline bool invert()
        {
            value_type log_det;
            return invert(log_det);
        }

        ///
        /// \return True if this is a column vector.
        ///
        inline bool is_column_vector() const
        {
            return _cx == 1;
        }

        ///
        /// \return True if this is empty.
        ///
        inline bool is_empty() const
        {
            return _m.empty();
        }

        ///
        /// \return True if this is a row vector.
        ///
        inline bool is_row_vector() const
        {
            return _cy == 1;
        }

        ///
        /// \return True if this has the same length as a specified vector.
        ///
        inline bool is_length(
                const basic_matrix & other) ///< The other matrix.
                const
        {
            return get_length() == other.get_length();
        }

        ///
        /// \return True if this has the specified length.
        ///
        inline bool is_length(
                const size_t length) ///< The length to compare.
                const
        {
            return get_length() == length;
        }

        ///
        /// \return True if this has the same size as a specified matrix.
        ///
        inline bool is_size(
                const basic_matrix & other) ///< The other matrix.
                const
        {
            return (_cx == other._cx) && (_cy == other._cy);
        }

        ///
        /// \return True if this has the same size as the specified dimensions.
        ///
        inline bool is_size(
                const size_t height, ///< The height of the matrix.
                const size_t width)  ///< The width of the matrix.
                const
        {
            return (_cy == height) && (_cx == width);
        }

        ///
        /// \return True if this matrix is square.
        ///
        inline bool is_square() const
        {
            return _cx == _cy;
        }

        ///
        /// \return True if this is a vector.
        ///
        inline bool is_vector() const
        {
            return is_column_vector() || is_row_vector();
        }

        ///
        /// Multiplies a column by a specified value.
        ///
        void multiply_column(
                const size_t     column, ///< The column to multiply.
                const value_type value)  ///< The value to multiply.
        {
            assert(column < get_width());

            auto       dst_ptr = get_data(0, column);
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
            {
                *dst_ptr *= value;
                dst_ptr += get_width();
            }
        }

        ///
        /// \return The product of the column and the vector.
        ///
        value_type multiply_column(
                const size_t         column, ///< The column to multiply.
                const basic_matrix & vector) ///< The vector to multiply.
        {
            assert(column < get_height());
            assert(vector.is_vector());

            auto       a_ptr = get_data(0, column);
            const auto a_end = a_ptr + get_length();
            auto       b_ptr = vector.get_data();
            auto       sum   = value_type(0);

            while (a_ptr != a_end)
            {
                sum += *a_ptr * *b_ptr++;
                a_ptr += get_width();
            }

            return sum;
        }

        ///
        /// Multiplies a row by a specified value.
        ///
        void multiply_row(
                const size_t     row,   ///< The row to multiply.
                const value_type value) ///< The value to multiply.
        {
            assert(row < get_height());

            auto       dst_ptr = get_data(row, 0);
            const auto dst_end = dst_ptr + get_width();

            while (dst_ptr != dst_end)
                *dst_ptr++ *= value;
        }

        ///
        /// \return The product of the row and the vector.
        ///
        value_type multiply_row(
                const size_t         row,    ///< The row to multiply.
                const basic_matrix & vector) ///< The vector to multiply.
                const
        {
            assert(row < get_height());
            assert(vector.is_vector());

            auto       a_ptr = get_data(row, 0);
            const auto a_end = a_ptr + get_width();
            auto       b_ptr = vector.get_data();
            auto       sum   = value_type(0);

            while (a_ptr != a_end)
                sum += *a_ptr++ * *b_ptr++;

            return sum;
        }

        ///
        /// Forms the Cholesky factorization of a symmetric positive-definite
        /// matrix: [A = L * L^T] where L is the lower triangular portion of the
        /// matrix.
        ///
        /// \return True if the method is successful.
        ///
        bool potrf_lower()
        {
            assert(is_square());
            assert(!is_empty());

            const auto n = int(get_width());

            return 0 == lapack_type::potrf(
                lapack_type::row_major, // layout
                'L',                    // uplo
                n,                      // n
                get_data(),             // a
                n);                     // lda
        }

        ///
        /// Computes the inverse inv(A) of a symmetric positive definite matrix
        /// A. Before calling this routine, call dpotrf to factorize A.
        ///
        /// \return True if the method is successful.
        ///
        bool potri_lower()
        {
            assert(is_square());
            assert(!is_empty());

            const auto n = int(get_width());

            return 0 == lapack_type::potri(
                lapack_type::row_major, // layout
                'L',                    // uplo
                n,                      // n
                get_data(),             // a
                n);                     // lda
        }

        ///
        /// Reads the matrix values from the specified file.
        ///
        /// \throw An exception if there is an error reading the file.
        ///
        inline void read(
                const std::string & path) ///< The path to the file.
        {
            read(path.c_str());
        }

        ///
        /// Reads the matrix values from the specified file.
        ///
        /// \throw An exception if there is an error reading the file.
        ///
        void read(
                const char * const path) ///< The path to the file.
        {
            assert(path != nullptr);

            std::ifstream in (path);
            if (!in.good())
                throw error() << "failed to open matrix '" << path << "'";

            try
            {
                read(in);
            }
            catch (const std::exception & e)
            {
                throw error()
                        << "failed to read matrix '"
                        << path << "': " << e.what();
            }
        }

        ///
        /// Reads the matrix values from the specified stream.
        ///
        /// \throw An exception if there is an error reading the stream.
        ///
        void read(
                std::istream & in) ///< The input stream.
        {
            size_t cx, cy;
            if (!(in >> cy >> cx))
                throw error()
                    << "failed to parse matrix size";

            if ((cx == 0 && cy != 0) || (cx != 0 && cy == 0))
                throw error()
                    << "invalid matrix size ["
                    << cy << "x" << cx << "]";

            vector_type m;
            m.reserve(cx * cy);

            for (size_t y = 0; y < cy; y++)
            {
                for (size_t x = 0; x < cx; x++)
                {
                    value_type value;
                    if (!(in >> value))
                        throw error()
                            << "failed to parse matrix value at cell ["
                            << y+1 << "," << x+1 << "]";

                    m.push_back(value);
                }
            }

            _cx = cx;
            _cy = cy;
            _m.swap(m);
        }

        ///
        /// Resizes the matrix to the specified dimensions. The values of the
        /// matrix are not reset.
        ///
        void resize(
                const size_t height, ///< The new height.
                const size_t width)  ///< The new width.
        {
            assert((height == 0 && width == 0)
                || (height != 0 && width != 0));

            if (height == _cy && width == _cx)
                return;

            _cx = width;
            _cy = height;
            _m.resize(width * height);
        }

        ///
        /// Sets the specified stream to scientific notation with a high
        /// precision.
        ///
        inline static void set_high_precision(
                std::ostream & out) ///< The output stream.
        {
            static const auto precision =
                1 + std::numeric_limits<value_type>::digits10;

            out << std::scientific <<  std::setprecision(precision);
        }

        ///
        /// Sets a single value of the vector.
        ///
        inline void set_value(
                const size_t       index, ///< The index of the value.
                const value_type & value) ///< The new value.
        {
            assert(index < _m.size());
            _m[index] = value;
        }

        ///
        /// Sets a single value of the matrix.
        ///
        inline void set_value(
                const size_t       row,    ///< The row to assign.
                const size_t       column, ///< The colum to assign.
                const value_type & value)  ///< The value to assign.
        {
            const auto index = get_index(row, column);
            _m[index] = value;
        }

        ///
        /// Sets all values of the matrix to the specified value.
        ///
        void set_values(
            const value_type value) ///< The value to assign.
        {
            for (auto & m : _m)
                m = value;
        }

        ///
        /// \return A string representation of this instance.
        ///
        std::string str() const
        {
            std::ostringstream out;
            write(out);
            return out.str();
        }

        ///
        /// Swaps this matrix and another matrix.
        ///
        inline void swap(
                basic_matrix & other) ///< The matrix to swap.
        {
            std::swap(_cx, other._cx);
            std::swap(_cy, other._cy);
            _m.swap(other._m);
        }

        ///
        /// Transposes this matrix.
        ///
        inline void transpose()
        {
            create_transpose(*this);
        }

        ///
        /// Writes this matrix to the specified file.
        ///
        /// \throw An exception if there is an error writing to the file.
        ///
        void write(
                const std::string & path) ///< The path to the file.
                const
        {
            write(path.c_str());
        }

        ///
        /// Writes this matrix to the specified file.
        ///
        /// \throw An exception if there is an error writing to the file.
        ///
        void write(
                const char * const path) ///< The path to the file.
                const
        {
            assert(path != nullptr);

            std::ofstream out (path);

            if (!out.good())
                throw error() << "failed to create matrix '" << path << "'";

            write(out);
        }

        ///
        /// Writes this matrix to the specified output stream.
        ///
        void write(
                std::ostream & out) ///< The output stream.
                const
        {
            value_type const * ptr = _m.data();

            out << _cy << ' ' << _cx << '\n';

            if (_cx == 0)
                return;

            for (size_t y = 0; y < _cy; y++)
            {
                out << *ptr++;

                for (size_t x = 1; x < _cx; x++)
                    out << '\t' << *ptr++;

                out << '\n';
            }
        }

        ///
        /// \return The element at the specified index.
        ///
        inline const value_type & operator [] (
                const size_t rhs) ///< The element index.
                const
        {
            assert(rhs < get_length());
            return _m[rhs];
        }

        ///
        /// \return The element at the specified index.
        ///
        inline value_type & operator [] (
                const size_t rhs) ///< The element index.
        {
            assert(rhs < get_length());
            return _m[rhs];
        }

        ///
        /// \return The element at the specified row and column.
        ///
        inline const value_type & operator () (
                const size_t rhs_1, ///< The row.
                const size_t rhs_2) ///< The column.
                const
        {
            const auto index = get_index(rhs_1, rhs_2);
            return _m[index];
        }

        ///
        /// \return The element at the specified row and column.
        ///
        inline value_type & operator () (
                const size_t rhs_1, ///< The row.
                const size_t rhs_2) ///< The column.
        {
            const auto index = get_index(rhs_1, rhs_2);
            return _m[index];
        }

        ///
        /// Adds the specified matrix to this instance.
        ///
        /// \param rhs The matrix to add.
        /// \return    This instance.
        ///
        basic_matrix & operator += (const basic_matrix & rhs)
        {
            assert(get_width() == rhs.get_width());
            assert(get_height() == rhs.get_height());

            auto       dst_ptr = get_data();
            auto       src_ptr = rhs.get_data();
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
                *dst_ptr++ += *src_ptr++;

            return *this;
        }

        ///
        /// Adds the specified scalar to this instance.
        ///
        /// \param rhs The scalar to add.
        /// \return    This instance.
        ///
        basic_matrix & operator += (const value_type rhs)
        {
            auto       dst_ptr = get_data();
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
                *dst_ptr++ += rhs;

            return *this;
        }

        ///
        /// Subtracts the specified matrix from this instance.
        ///
        /// \param rhs The matrix to subtract.
        /// \return    This instance.
        ///
        basic_matrix & operator -= (const basic_matrix & rhs)
        {
            assert(get_width() == rhs.get_width());
            assert(get_height() == rhs.get_height());

            auto       dst_ptr = get_data();
            auto       src_ptr = rhs.get_data();
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
                *dst_ptr++ -= *src_ptr++;

            return *this;
        }

        ///
        /// Subtracts the specified scalar from this instance.
        ///
        /// \param rhs The scalar to subtract.
        /// \return    This instance.
        ///
        basic_matrix & operator -= (const value_type rhs)
        {
            auto       dst_ptr = get_data();
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
                *dst_ptr++ -= rhs;

            return *this;
        }

        ///
        /// Multiplies this matrix and the specified matrix and assigns the
        /// product to this instance.
        ///
        /// \param rhs The matrix to multiply.
        /// \return    This instance.
        ///
        basic_matrix & operator *= (const basic_matrix & rhs)
        {
            basic_matrix dst (get_height(), rhs.get_width());
            gemm(*this, rhs, dst);
            swap(dst);
            return *this;
        }

        ///
        /// Multiplies this matrix and the specified scalar and assigns the
        /// product to this instance.
        ///
        /// \param rhs The scalar to multiply.
        /// \return    This instance.
        ///
        basic_matrix & operator *= (const value_type rhs)
        {
            auto       dst_ptr = get_data();
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
                *dst_ptr++ *= rhs;

            return *this;
        }

        ///
        /// Divides this matrix by the specified scalar and assigns the
        /// quotient to this instance.
        ///
        /// \param rhs The scalar to multiply.
        /// \return    This instance.
        ///
        basic_matrix & operator /= (const value_type rhs)
        {
            auto       dst_ptr = get_data();
            const auto dst_end = dst_ptr + get_length();

            while (dst_ptr != dst_end)
                *dst_ptr++ /= rhs;

            return *this;
        }

        ///
        /// Adds a matrix.
        ///
        /// \param rhs The matrix to add.
        /// \return    A new matrix.
        ///
        inline basic_matrix operator + (const basic_matrix & rhs) const
        {
            return basic_matrix(*this) += rhs;
        }

        ///
        /// Adds a value.
        ///
        /// \param rhs The value to add.
        /// \return    A new matrix.
        ///
        inline basic_matrix operator + (const value_type rhs) const
        {
            return basic_matrix(*this) += rhs;
        }

        ///
        /// Subtracts a matrix.
        ///
        /// \param rhs The matrix to subtract.
        /// \return    A new matrix.
        ///
        inline basic_matrix operator - (const basic_matrix & rhs) const
        {
            return basic_matrix(*this) -= rhs;
        }

        ///
        /// Subtracts a value.
        ///
        /// \param rhs The value to subtract.
        /// \return    A new matrix.
        ///
        inline basic_matrix operator - (const value_type rhs) const
        {
            return basic_matrix(*this) -= rhs;
        }

        ///
        /// \return The negation of this instance.
        ///
        basic_matrix operator - () const
        {
            basic_matrix out (get_height(), get_width());

            auto       dst = out.get_data();
            auto       src = get_data();
            const auto end = get_data() + get_length();

            while (src != end)
                *dst++ = -(*src++);

            return out;
        }

        ///
        /// Multiplies a matrix.
        ///
        /// \param rhs The matrix to multiply.
        /// \return    A new matrix.
        ///
        inline basic_matrix operator * (const basic_matrix & rhs) const
        {
            basic_matrix dst (get_height(), rhs.get_width());
            gemm(*this, rhs, dst);
            return dst;
        }

        ///
        /// Multiplies a scalar.
        ///
        /// \param rhs The scalar to multiply.
        /// \return    A new matrix.
        ///
        inline basic_matrix operator * (const value_type rhs) const
        {
            return basic_matrix(*this) *= rhs;
        }

        ///
        /// Divides a scalar.
        ///
        /// \param rhs The scalar to divides.
        /// \return    A new matrix.
        ///
        inline basic_matrix operator / (const value_type rhs) const
        {
            return basic_matrix(*this) /= rhs;
        }

    private:
        typedef std::vector<value_type> vector_type;

        size_t      _cy;
        size_t      _cx;
        vector_type _m;
    };
}

///
/// Multiplies the specified scalar with the specified matrix.
///
/// \return A new matrix.
///
template <typename TValue>
jade::basic_matrix<TValue> operator * (
        const TValue                       lhs, ///< The scalar.
        const jade::basic_matrix<TValue> & rhs) ///< The matrix.
{
    return rhs * lhs;
}

///
/// Writes the specified matrix to the specified output stream.
///
/// \return The output stream.
///
template <typename TValue>
std::ostream & operator << (
        std::ostream &                     lhs, ///< The output stream.
        const jade::basic_matrix<TValue> & rhs) ///< The matrix.
{
    rhs.write(lhs);
    return lhs;
}

///
/// Reads the specified matrix from the specified output stream.
///
/// \return The input stream.
///
template <typename TValue>
std::istream & operator >> (
        std::istream &               lhs, ///< The input stream.
        jade::basic_matrix<TValue> & rhs) ///< The matrix.
{
    rhs.read(lhs);
    return lhs;
}

#endif // JADE_MATRIX_HPP__
