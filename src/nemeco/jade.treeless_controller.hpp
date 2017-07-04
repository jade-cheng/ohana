/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_TREELESS_CONTROLLER_HPP__
#define JADE_TREELESS_CONTROLLER_HPP__

#include "jade.controller.hpp"

namespace jade
{
    ///
    /// A template for a class that encodes and decodes parameters for the
    /// Nelder-Mead algorithm. This class does not use a user-specified Newick
    /// tree.
    ///
    template <typename TValue>
    class basic_treeless_controller
        : public basic_controller<TValue>
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The settings type.
        typedef basic_settings<value_type> settings_type;

        /// The container type for the controller.
        typedef typename
            basic_controller<TValue>::container_type
            container_type;

        ///
        /// Initializes a new instance of the class based on the specified
        /// program settings.
        ///
        explicit basic_treeless_controller(
                const settings_type & settings) ///< The program settings.
            : basic_controller<TValue> (settings)
        {
        }

        // --------------------------------------------------------------------
        virtual container_type init_parameters() override
        {
            const auto & c  = this->get_c();
            const auto   rk = this->get_rk();
            assert(c.get_height() == rk);

            //
            // The container contains the lower-right triangle of data,
            // including the diagonal. This comes to RK * RK (the total number
            // of items in the matrix), subtract RK (the number of items in the
            // diagonal), divide by two (only half of these elements), plus RK
            // (the number of items in the diagonal).
            //
            container_type params;
            params.reserve(rk + ((rk * rk) - rk) / 2);

            //
            // Pointers to the start and end of the first row. Note the
            // covariance matrix is symmetric.
            //
            auto src_ptr = c.get_data();
            auto src_end = c.get_data() + rk;

            //
            // Loop over every row/column.
            //
            for (size_t i = 0; i < rk; i++)
            {
                //
                // Skip the columns before the diagonal.
                //
                src_ptr += i;

                //
                // Copy all columns from the diagonal to the end of the row.
                //
                while (src_ptr != src_end)
                    params.push_back(*src_ptr++);

                //
                // Advance the next end-of-row RK elements ahead.
                //
                src_end += rk;
            }

            return params;
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
            const auto rk = this->get_rk();

            //
            // The container contains the lower-right triangle of data,
            // including the diagonal. This comes to RK * RK (the total number
            // of items in the matrix), subtract RK (the number of items in the
            // diagonal), divide by two (only half of these elements), plus RK
            // (the number of items in the diagonal).
            //
            assert(src.size() == rk + ((rk * rk) - rk) / 2);

            //
            // Pointers to the start and end of the first column.
            //
            auto       dst_ptr = dst.get_data();
            auto       dst_end = dst.get_data() + dst.get_length();
            auto       src_ptr = src.data();
            const auto src_end = src.data() + src.size();

            //
            // Loop over every row/column.
            //
            while (src_ptr != src_end)
            {
                //
                // Don't lose track of the start of the column data.
                //
                const auto anchor = dst_ptr;

                //
                // Copy the rows of the column from the diagonal to the end.
                //
                for (; dst_ptr != dst_end; dst_ptr += rk)
                    *dst_ptr = *src_ptr++;

                //
                // The next start-of-column is below and to the right of the
                // previous one. The next end-of-column is one to the right.
                //
                dst_ptr = anchor + rk + 1;
                dst_end++;
            }

            //
            // This method is always successful.
            //
            return true;
        }
    };
}

#endif // JADE_TREELESS_CONTROLLER_HPP__
