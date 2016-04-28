/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_STOPWATCH_HPP__
#define JADE_STOPWATCH_HPP__

#include "jade.system.hpp"

namespace jade
{
    ///
    /// A template class imlementing a stopwatch.
    ///
    template <typename TClock>
    class basic_stopwatch
    {
    public:
        /// The clock type.
        typedef TClock clock_type;

        /// The time point type for the clock.
        typedef typename clock_type::time_point time_point_type;

        ///
        /// Initializes a new instance of the class.
        ///
        inline basic_stopwatch()
            : _t0 (clock_type::now())
        {
        }

        ///
        /// Compares the elapsed time with the specified number of seconds. The
        /// method returns a negative value if the elapsed time is lesser.
        ///
        /// \return Negative, positive, or zero.
        ///
        int compare(
                const double seconds) ///< The elapsed seconds to compare.
                const
        {
            const auto lhs = get_elapsed();
            const auto rhs = seconds;
            return lhs < rhs ? -1 : rhs < lhs ? +1 : 0;
        }

        ///
        /// \return The elapsed time in seconds as a floating-point value.
        ///
        double get_elapsed() const
        {
            const auto t1 = clock_type::now();
            const auto dt = std::chrono::duration<double>(t1 - _t0);
            return dt.count();
        }

        ///
        /// \return The current time as a string.
        ///
        std::string str() const
        {
            std::ostringstream out;
            out << std::fixed << std::setprecision(6) << get_elapsed();
            return out.str();
        }

        ///
        /// Compares the elapsed time with the specified number of seconds. The
        /// operator returns a negative value if the elapsed time is lesser.
        ///
        /// \return True if the value is lesser; otherwise false.
        ///
        inline bool operator < (
                const double rhs) ///< The elapsed seconds to compare.
                const
        {
            return compare(rhs) < 0;
        }

        ///
        /// Compares the elapsed time with the specified number of seconds. The
        /// operator returns a negative value if the elapsed time is greater.
        ///
        /// \return True if the value is greater; otherwise false.
        ///
        inline bool operator > (
                const double rhs) ///< The elapsed seconds to compare.
                const
        {
            return compare(rhs) > 0;
        }

    private:
        time_point_type _t0;
    };

    /// A class implementing a stopwatch.
    typedef basic_stopwatch<std::chrono::high_resolution_clock> stopwatch;
}

///
/// Writes the value of a stopwatch to an output stream.
///
/// \return The output stream.
///
template <typename TClock>
std::ostream & operator << (
        std::ostream &                        dst, ///< The output stream.
        const jade::basic_stopwatch<TClock> & src) ///< The stopwatch.
{
    return dst << src.str();
}

#endif // JADE_STOPWATCH_HPP__
