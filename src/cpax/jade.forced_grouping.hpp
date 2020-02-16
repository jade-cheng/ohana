/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_FORCED_GROUPING_HPP__
#define JADE_FORCED_GROUPING_HPP__

#include "jade.randomizer.hpp"

namespace jade
{
    ///
    /// A template for a class that implements the forced grouping feature.
    ///
    template <typename TValue>
    class basic_forced_grouping
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The randomizer type.
        typedef basic_randomizer<value_type> randomizer_type;

        ///
        /// Initializes a new instance of the class.
        ///
        basic_forced_grouping()
            : _a ()
            , _b ()
            , _i (0)
            , _k (0)
        {
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        explicit basic_forced_grouping(
                char const * const path) ///< The path to the file.
            : _a ()
            , _b ()
            , _i ()
            , _k ()
        {
            assert(path != nullptr);
            try
            {
                std::istringstream in (_strip_comments(path));

                if (!(in >> _i) || _i == 0)
                    throw error()
                        << "error parsing number of individuals";

                if (!(in >> _k) || _k == 0)
                    throw error()
                        << "error parsing number of components";

                _a.reserve(_i);

                for (size_t i = 0; i < _i; i++)
                {
                    size_t value;

                    if (!(in >> value))
                        throw error()
                            << "error parsing component assignment "
                            << "for individual " << i+1;

                    _a.push_back(value);
                }

                const auto P = 1 + *std::max_element(_a.begin(), _a.end());

                _b.reserve(P);

                for (size_t p = 0; p < P; p++)
                {
                    try
                    {
                        _b.emplace_back(in);
                    }
                    catch (const std::exception & e)
                    {
                        throw error()
                            << "error reading B vector for population index "
                            << p << ": " << e.what();
                    }
                }

                _validate(in);
            }
            catch (const std::exception & e)
            {
                throw error() << "failed to read forced-grouping file '"
                              << path << "': " << e.what();
            }
        }

        ///
        /// Initializes a new instance of the class based on values from the
        /// specified file.
        ///
        inline explicit basic_forced_grouping(
                const std::string & path) ///< The path to the file.
            : basic_forced_grouping(path.c_str())
        {
        }

        ///
        /// \return The number of individuals.
        ///
        inline size_t get_i() const
        {
            return _i;
        }

        ///
        /// \return The number of components.
        ///
        inline size_t get_k() const
        {
            return _k;
        }

        ///
        /// \return The maximum value for a specified individual and component.
        ///
        inline value_type get_max(
                const size_t i, ///< The individual index.
                const size_t k) ///< The component index.
                const
        {
            assert(i < _i);
            assert(k < _k);
            return _b[_a[i]].get_value(k + _k);
        }

        ///
        /// \return The minimim value for a specified individual and component.
        ///
        inline value_type get_min(
                const size_t i, ///< The individual index.
                const size_t k) ///< The component index.
                const
        {
            assert(i < _i);
            assert(k < _k);
            return _b[_a[i]].get_value(k);
        }

        ///
        /// \return A new Q matrix from random values.
        ///
        matrix_type randomize_q(
                randomizer_type & rnd) ///< The randomizer.
                const
        {
            static const auto epsilon = value_type(0.000001);

            matrix_type q (_i, _k);

            for (size_t k = 0; k < _k; k++)
                for (size_t i = 0; i < _i; i++)
                    q(i, k) = _lerp(get_min(i, k), get_max(i, k), q(i, k));

            typedef std::uniform_int_distribution<size_t>  k_dist_type;
            typedef std::uniform_real_distribution<value_type> q_dist_type;

            k_dist_type k_dist (0, _k - 1);

            auto & engine = rnd.get_engine();

            for (size_t i = 0; i < _i; i++)
            {
                for (;;)
                {
                    const auto row_sum = q.get_row_sum(i);
                    if (std::fabs(value_type(1) - row_sum) < epsilon)
                        break;

                    const auto k = k_dist(engine);
                    const auto q_ik = q(i, k);

                    if (row_sum > value_type(1))
                    {
                        const auto min_ik   = get_min(i, k);
                        const auto distance = row_sum - value_type(1);
                        const auto boundary = std::max(min_ik, q_ik - distance);

                        q_dist_type q_dist (boundary, q_ik);
                        q(i, k) = q_dist(engine);
                    }
                    else
                    {
                        const auto max_ik   = get_max(i, k);
                        const auto distance = value_type(1) - row_sum;
                        const auto boundary = std::min(q_ik + distance, max_ik);

                        q_dist_type q_dist (q_ik, boundary);
                        q(i, k) = q_dist(engine);
                    }
                }
            }

            return q;
        }

        ///
        /// Validates the Q matrix and throws an exception if validation fails.
        /// \return True.
        ///
        bool validate_q(
                const matrix_type & q) ///< The Q matrix.
                const
        {
            if (_k != q.get_width())
                throw error()
                    << "inconsistent number of components specified in "
                    << "forced-grouping file (" << _k << ") and "
                    << q.get_size_str() << " Q matrix";

            if (_i != q.get_height())
                throw error()
                    << "inconsistent number of individuals specified in "
                    << "forced-grouping file (" << _i << ") and "
                    << q.get_size_str() << " Q matrix";

            for (size_t i = 0; i < _i; i++)
            {
                for (size_t k = 0; k < _k; k++)
                {
                    const auto q_ik = q(i, k);
                    const auto min  = get_min(i, k);
                    const auto max  = get_max(i, k);
                    if (q_ik >= min && q_ik <= max)
                        continue;

                    throw error()
                        << "inconsistent Q matrix cell [" << i+1 << "," << k+1
                        << "] (" << q_ik << ") is outside the range specified "
                        << "in the forced-grouping file " << min << " to "
                        << max;
                }
            }

            return true;
        }

    private:
        // ------------------------------------------------------------------------
        static value_type _lerp(
                const value_type min,
                const value_type max,
                const value_type percent)
        {
            assert(min >= value_type(0));
            assert(max <= value_type(1));
            assert(percent >= value_type(0) && percent <= value_type(1));

            return min + (max - min) * percent;
        }

        // --------------------------------------------------------------------
        static std::string _strip_comments(const char * const path)
        {
            assert(path != nullptr);

            std::ifstream in (path);
            if (!in.good())
                throw jade::error() << "error opening file";

            std::ostringstream out;

            for (auto n = 1;; n++)
            {
                std::string line;
                std::getline(in, line);

                if (!line.empty() && line[0] != '#')
                    out << line << std::endl;

                if (in.eof())
                    break;

                if (!in.good())
                    throw jade::error() << "error reading file at line " << n;
            }

            return out.str();
        }

        // ------------------------------------------------------------------------
        void _validate(std::istringstream & in) const
        {
            std::string tmp;
            if (in >> tmp)
                throw error()
                    << "invalid token encountered at end of file: " << tmp;

            if (_i < 2)
                throw error()
                    << "invalid number of individuals: " << _i
                    << "; expected at least 2";

            if (_k < 2)
                throw error()
                    << "invalid number of components: " << _k
                    << "; expected at least 2";

            const auto k2 = 2 * _k;

            for (size_t k = 0; k < _k; k++)
            {
                const auto & b_k = _b[k];

                if (b_k.get_height() != k2 || b_k.get_width() != 1)
                    throw error()
                        << "invalid B vector for population index " << k
                        << ": size " << b_k.get_size_str()
                        << " does not match expected [" << k2 << "x1]";

                auto min_sum = value_type(0);
                for (size_t kk = 0; kk < _k; kk++)
                    min_sum += b_k[kk];
                if (min_sum > value_type(1))
                    throw error()
                        << "invalid B vector for population index " << k
                        << ": the sum of the first " << _k
                        << " values is greater than 1";

                auto max_sum = value_type(0);
                for (size_t kk = 0; kk < _k; kk++)
                    max_sum += b_k[_k + kk];
                if (max_sum < value_type(1))
                    throw error()
                        << "invalid B vector for population index " << k
                        << ": the sum of the last " << _k
                        << " values is less than 1";

                for (size_t kk = 0; kk < k2; kk++)
                    if (b_k[kk] < value_type(0) || b_k[kk] > value_type(1))
                        throw error()
                            << "invalid B vector for population index " << k
                            << ": cell " << kk+1 << " (" << b_k[kk]
                            << ") is not between 0 and 1";

                for (size_t kk = 0; kk < _k; kk++)
                    if (!(b_k[kk] <= b_k[_k + kk]))
                        throw error()
                            << "invalid B vector for population " << k
                            << ": cell " << kk+1 << " (" << b_k[kk]
                            << ") is greater than cell " << _k+kk+1 << " ("
                            << b_k[_k + kk] << ")";
            }
        }

        std::vector<size_t>      _a; // [K] component assignments
        std::vector<matrix_type> _b; // [K][2K x 1] proportion ranges
        size_t                   _i; // number of individuals
        size_t                   _k; // number of components
    };
}

#endif // JADE_FORCED_GROUPING_HPP__
