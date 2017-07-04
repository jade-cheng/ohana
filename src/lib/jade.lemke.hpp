/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_LEMKE_HPP__
#define JADE_LEMKE_HPP__

#include "jade.matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that implements Lemke's algorithm.
    ///
    template <typename TValue>
    class basic_lemke
    {
    public:
        ///
        /// The value associated with an invalid or unassigned index.
        ///
        static constexpr auto invalid_index =
            std::numeric_limits<size_t>::max();

        ///
        /// Possible states of the algorithm.
        ///
        struct state
        {
            ///
            /// The state type.
            ///
            enum type
            {
                executing,              ///< The algorithm is executing.
                completed,              ///< The algorithm completed.
                aborted_initialization, ///< Aborted during initialization.
                aborted_elimination,    ///< Aborted with an invalid pivot.
                aborted_pivot           ///< Aborted finding the pivot.
            };

            ///
            /// \return The string representation of the type.
            ///
            static const char * str(
                    const type value) ///< The type.
            {
                switch (value)
                {
                    #define CASE(E) case E: return #E; break
                    CASE(executing);
                    CASE(completed);
                    CASE(aborted_initialization);
                    CASE(aborted_elimination);
                    CASE(aborted_pivot);
                    #undef CASE
                }

                return nullptr;
            }
        };

        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The labels type.
        typedef std::vector<size_t> labels_type;

        /// The state type.
        typedef typename state::type state_type;

        ///
        /// Initializes a new instance of the class based on the specified
        /// tableau.
        ///
        explicit basic_lemke(
                const matrix_type & tableau) ///< The tableau.
            : _labels    ()
            , _pivot_col (invalid_index)
            , _pivot_row (invalid_index)
            , _state     (state::executing)
            , _tableau   (tableau)
        {
            assert(tableau.get_height() > 0);
            assert(tableau.get_width() == tableau.get_height() * 2 + 2);

            const auto & t  = _tableau;
            const auto   n  = t.get_height();
            const auto   z0 = n + n;

            //
            // Initially add the labels.
            //
            _labels.reserve(n);
            for (size_t i = 0; i < n; i++)
                _labels.push_back(i);

            //
            // The first pivot column is z_0; attempt to find the first pivot
            // row, but abort if necessary.
            //
            _pivot_col = z0;
            if (!_find_initial_pivot_row())
                _terminate(state::aborted_initialization);
        }

        ///
        /// Initializes a new instance of the class based on the specified
        /// M and Q matrices.
        ///
        inline basic_lemke(
                const matrix_type & m, ///< The M matrix.
                const matrix_type & q) ///< The Q matrix.
            : basic_lemke(_create_tableau(m, q))
        {
        }

        ///
        /// Initializes a new instance of the class based on the specified
        /// Q and A matrices and c and b vectors.
        ///
        inline basic_lemke(
                const matrix_type & q, ///< The Q matrix.
                const matrix_type & a, ///< The A matrix.
                const matrix_type & c, ///< The c vector.
                const matrix_type & b) ///< The b vector.
            : basic_lemke(_create_tableau(q, a, c, b))
        {
        }

        ///
        /// \return The specified label index as a string; labels are
        /// formatted as w_1, w_2, ..., w_n; z_0, z_1, ..., z_n; or q.
        ///
        std::string format_label(
                const size_t label) ///< The label to format.
                const
        {
            const auto & t  = _tableau;
            const auto   n  = t.get_height();
            const auto   z1 = n;
            const auto   z0 = n + n;
            const auto   q  = n + n + 1;

            std::ostringstream out;
            if      (label  < z1) out << "w_" << label + 1;
            else if (label  < z0) out << "z_" << label + 1 - n;
            else if (label == z0) out << "z_0";
            else if (label ==  q) out << "q";
            else                  out << label;
            return out.str();
        }

        ///
        /// \return The vector of labels.
        ///
        inline const labels_type & get_labels() const
        {
            return _labels;
        }

        ///
        /// \return The output.
        ///
        matrix_type get_output() const
        {
            const auto & t  = _tableau;
            const auto   n  = t.get_height();
            const auto   z1 = n;
            const auto   q  = n + n + 1;

            matrix_type out (n, 1);

            for (size_t i = 0; i < n; i++)
            {
                const auto label = _labels[i];
                if (label >= z1)
                    out[label - z1] = t(i, q);
            }

            return out;
        }

        ///
        /// \return The pivot column, or invalid_index if the algorithm has
        /// terminated.
        ///
        inline size_t get_pivot_col() const
        {
            return _pivot_col;
        }

        ///
        /// \return The pivot row, or invalid_index if the algorithm has
        /// terminated.
        ///
        inline size_t get_pivot_row() const
        {
            return _pivot_row;
        }

        ///
        /// \return The state.
        ///
        inline state_type get_state() const
        {
            return _state;
        }

        ///
        /// \return The tableau.
        ///
        inline const matrix_type & get_tableau() const
        {
            return _tableau;
        }

        ///
        /// \return True if the algorithm is still executing; otherwise, false.
        ///
        inline bool is_executing() const
        {
            return _state == state::executing;
        }

        ///
        /// Performs one step of the algorithm.
        /// \return True if the still executing; otherwise, false.
        ///
        bool iterate()
        {
            if (_state != state::executing)
                return false;

            if (!_eliminate())
                return _terminate(state::aborted_elimination);

            if (!_relabel())
                return _terminate(state::completed);

            if (!_find_pivot_row())
                return _terminate(state::aborted_pivot);

            return true;
        }

        ///
        /// Executes the algorithm until it has completed or has aborted.
        /// \return True if completed; otherwise, false.
        ///
        bool solve()
        {
            while (_state == state::executing)
                iterate();

            return _state == state::completed;
        }

        ///
        /// Attempts to solve the linear complementarity problem using Lemke's
        /// Algorithm. If successful, this method stores the z vector into the
        /// output parameter.
        ///
        /// \return True if successful; otherwise, false.
        ///
        static bool solve(
                matrix_type &       out,     ///< The output vector.
                const matrix_type & tableau) ///< The tableau.
        {
            basic_lemke lemke (tableau);
            if (!lemke.solve())
                return false;

            out = lemke.get_output();
            return true;
        }

        ///
        /// Attempts to solve the linear complementarity problem using Lemke's
        /// Algorithm. If successful, this method stores the z vector into the
        /// output parameter.
        ///
        /// \return True if successful; otherwise, false.
        ///
        inline static bool solve(
                matrix_type &       out, ///< The output vector.
                const matrix_type & m,   ///< The M matrix.
                const matrix_type & q)   ///< The q vector.
        {
            return solve(out, _create_tableau(m, q));
        }

        ///
        /// Attempts to solve the linear complementarity problem using Lemke's
        /// Algorithm. If successful, this method stores the z vector into the
        /// output parameter.
        ///
        /// \return True if successful; otherwise, false.
        ///
        inline static bool solve(
                matrix_type &       out, ///< The output vector.
                const matrix_type & q,   ///< The Q matrix.
                const matrix_type & a,   ///< The A matrix.
                const matrix_type & c,   ///< The c vector.
                const matrix_type & b)   ///< The b vector.
        {
            return solve(out, _create_tableau(q, a, c, b));
        }

        ///
        /// \return A string representation of the class.
        ///
        std::string str() const
        {
            const auto & t = _tableau;
            const auto   n = t.get_height();
            const auto   q = n + n + 1;

            static const size_t cx = 8;
            std::ostringstream out;
            out << std::setprecision(3)
                << std::setw(cx) << "BV";

            for (size_t j = 0; j <= q; j++)
                out << std::setw(cx) << format_label(j);
            out << std::endl;

            for (size_t i = 0; i < n; i++)
            {
                out << std::setw(cx) << format_label(_labels[i]);
                for (size_t j = 0; j <= q; j++)
                    out << std::setw(cx) << t(i, j);
                out << std::endl;
            }

            out << std::endl
                << "state: " << state::str(_state) << std::endl
                << "pivot: ";

            if (_pivot_row == invalid_index)
                out << "<none>";
            else
                out << "{ " << format_label(_pivot_row)
                    << ", " << format_label(_pivot_col) << " }";

            out << std::endl << std::endl;
            return out.str();
        }

    private:
        // --------------------------------------------------------------------
        static matrix_type _create_m(
                const matrix_type & q,
                const matrix_type & a)
        {
            assert(q.get_width() == a.get_width() || a.is_empty());
            assert(q.is_square());
            assert(q.get_height() >= 1);

            const auto qn = q.get_height();
            const auto ah = a.get_height();
            const auto mn = qn + ah;

            matrix_type m (mn, mn);

            //
            // Assign Q to the top-left quadrant.
            //
            for (size_t i = 0; i < qn; i++)
                for (size_t j = 0; j < qn; j++)
                    m(i, j) = q(i, j);

            //
            // Assign A to the bottom-left quadrant.
            //
            for (size_t i = 0; i < ah; i++)
                for (size_t j = 0; j < qn; j++)
                    m(qn + i, j) = a(i, j);

            //
            // Assign -A^T to the top-right quadrant.
            //
            for (size_t i = 0; i < ah; i++)
                for (size_t j = 0; j < qn; j++)
                    m(j, qn + i) = -a(i, j);

            return m;
        }

        // --------------------------------------------------------------------
        static matrix_type _create_q(
                const matrix_type & c,
                const matrix_type & b)
        {
            assert(c.is_column_vector());
            assert(b.is_column_vector() || b.is_empty());
            assert(c.get_height() >= 1);

            const auto ch = c.get_height();
            const auto bh = b.get_height();

            matrix_type q (ch + bh, 1);

            //
            // Assign C to the top.
            //
            for (size_t i = 0; i < ch; i++)
                q[i] = c[i];

            //
            // Assign -B to the bottom.
            //
            for (size_t i = 0; i < bh; i++)
                q[ch + i] = -b[i];

            return q;
        }

        // --------------------------------------------------------------------
        static matrix_type _create_tableau(
                const matrix_type & m,
                const matrix_type & q)
        {
            assert(m.is_square());
            assert(q.is_column_vector());
            assert(m.get_height() == q.get_height());

            const auto n = q.get_length();
            matrix_type t (n, 2 * n + 2);

            //
            // Assign the identity matrix to w_1, w_2, ..., w_n.
            //
            for (size_t i = 0; i < n; i++)
                t(i, i) = 1;

            //
            // Assign -M_ij to z_ik where k = n + j.
            //
            for (size_t i = 0; i < n; i++)
                for (size_t j = 0; j < n; j++)
                    t(i, n + j) = -m(i, j);

            //
            // Assign -1 to z_0.
            //
            for (size_t i = 0; i < n; i++)
                t(i, 2 * n) = -1;

            //
            // Assign q.
            //
            for (size_t i = 0; i < n; i++)
                t(i, 2 * n + 1) = q[i];

            return t;
        }

        // --------------------------------------------------------------------
        inline static matrix_type _create_tableau(
                const matrix_type & q,
                const matrix_type & a,
                const matrix_type & c,
                const matrix_type & b)
        {
            return _create_tableau(_create_m(q, a), _create_q(c, b));
        }

        // --------------------------------------------------------------------
        bool _eliminate()
        {
            auto &     t = _tableau;
            const auto w = t.get_width();
            const auto n = t.get_height();

            //
            // If the pivot is zero, then abort.
            //
            auto & t_pivot = t(_pivot_row, _pivot_col);
            if (std::fabs(t_pivot) < _epsilon)
                return false;

            //
            // Normalize the pivot row.
            //
            for (size_t j = 0; j < w; j++)
                if (j != _pivot_col)
                    t(_pivot_row, j) /= t_pivot;
            t_pivot = value_type(1);

            //
            // Eliminate the non-pivot rows.
            //
            for (size_t i = 0; i < n; i++)
            {
                if (i == _pivot_row)
                    continue;

                auto & t_ip = t(i, _pivot_col);
                for (size_t j = 0; j < w; j++)
                    if (j != _pivot_col)
                        t(i, j) -= t_ip * t(_pivot_row, j);
                t_ip = value_type(0);
            }

            return true;
        }

        // --------------------------------------------------------------------
        bool _find_initial_pivot_row()
        {
            const auto & t = _tableau;
            const auto   n = t.get_height();
            const auto   q = n + n + 1;

            _pivot_row = invalid_index;

            auto pivot_value = value_type(0);

            //
            // Loop over each row.
            //
            for (size_t i = 0; i < n; i++)
            {
                //
                // Skip rows with a non-negative q value.
                //
                const auto t_iq = t(i, q);
                if (t_iq >= 0)
                    continue;

                //
                // Select the row with the lowest q value.
                //
                if (_pivot_row == invalid_index || t_iq < pivot_value)
                {
                    _pivot_row  = i;
                    pivot_value = t_iq;
                }
            }

            //
            // Return true if the pivot row was found.
            //
            return _pivot_row != invalid_index;
        }

        // --------------------------------------------------------------------
        bool _find_pivot_row()
        {
            const auto & t = _tableau;
            const auto   n = t.get_height();
            const auto   q = n + n + 1;

            _pivot_row = invalid_index;

            auto ratio = value_type(0);

            //
            // Loop over each row.
            //
            for (size_t i = 0; i < n; i++)
            {
                //
                // Skip rows that are non-positive in the pivot column.
                //
                const auto t_ip = t(i, _pivot_col);
                if (t_ip <= 0)
                    continue;

                //
                // Select the row with the lowest ratio.
                //
                const auto r_i = t(i, q) / t_ip;
                if (_pivot_row == invalid_index || r_i < ratio)
                {
                    _pivot_row = i;
                    ratio = r_i;
                }
            }

            //
            // Return true if the pivot row was found.
            //
            return _pivot_row != invalid_index;
        }

        // --------------------------------------------------------------------
        bool _relabel()
        {
            const auto & t  = _tableau;
            const auto   n  = t.get_height();
            const auto   z1 = n;
            const auto   z0 = n + n;

            //
            // Keep track of the label being replaced.
            //
            const auto old_label = _labels[_pivot_row];

            //
            // Replace it with the label associated with the pivot column.
            //
            _labels[_pivot_row] = _pivot_col;

            //
            // If the z_0 column was replaced, then the algorithm has completed
            // successfully.
            //
            if (old_label == z0)
                return false;

            //
            // Use the complementary label as the next pivot column.
            //
            _pivot_col = old_label >= z1
                    ? old_label - n
                    : old_label + n;

            return true;
        }

        // --------------------------------------------------------------------
        bool _terminate(const state_type new_state)
        {
            _pivot_row = invalid_index;
            _pivot_col = invalid_index;
            _state     = new_state;
            return false;
        }

        static constexpr auto _epsilon = value_type(0.000001);

        labels_type _labels;
        size_t      _pivot_col;
        size_t      _pivot_row;
        state_type  _state;
        matrix_type _tableau;
    };
}

#endif // JADE_LEMKE_HPP__
