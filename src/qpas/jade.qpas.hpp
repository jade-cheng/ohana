/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_QPAS_HPP__
#define JADE_QPAS_HPP__

#include "jade.matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that implements the Quadratic Programming for
    /// Active Set algorithm.
    ///
    template <typename TValue>
    class basic_qpas
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        ///
        /// Loops over the active set and computes a delta vector and a new
        /// active set.
        ///
        /// \param b_vec            The B vector.
        /// \param coefficients_mat The coefficients matrix.
        /// \param hessian_mat      The Hessian matrix.
        /// \param derivative_vec   The derivative vector.
        /// \param fixed_active_set The fixed active set.
        /// \param active_set       The active set.
        /// \param delta_vec        The delta vector.
        ///
        static void loop_over_active_set(
                const matrix_type         & b_vec,
                const matrix_type         & coefficients_mat,
                const matrix_type         & hessian_mat,
                const matrix_type         & derivative_vec,
                const std::vector<size_t> & fixed_active_set,
                std::vector<size_t>       & active_set,
                matrix_type               & delta_vec)
        {
            assert(!fixed_active_set.empty() || !active_set.empty());

            const auto K = hessian_mat.get_height();
            assert(K > 0);

            const auto inequality_constraint_count =
                    b_vec.get_length() - fixed_active_set.size();

            assert(b_vec.is_vector());
            assert(coefficients_mat.get_height() == b_vec.get_length());
            assert(fixed_active_set.size() <= K);
            assert(delta_vec.is_vector());
            assert(delta_vec.get_height() == K);

            std::set<unsigned long> visited_sets;

            const auto insert_key = [&visited_sets, &active_set]() -> bool
            {
                auto key = 0UL;
                for (const auto i : active_set)
                    key |= 1UL << i;
                if (visited_sets.find(key) != visited_sets.end())
                    return false;
                visited_sets.insert(key);
                return true;
            };

            while (insert_key())
            {
                std::vector<size_t> merged_active_set;
                merged_active_set.insert(
                            merged_active_set.end(),
                            active_set.begin(),
                            active_set.end());
                merged_active_set.insert(
                            merged_active_set.end(),
                            fixed_active_set.begin(),
                            fixed_active_set.end());

                matrix_type lagrangian_vec (
                    merged_active_set.size(),
                    merged_active_set.empty() ? 0 : 1);

                matrix_type try_delta_vec (K, 1);

                _kkt(b_vec,
                    coefficients_mat,
                    hessian_mat,
                    derivative_vec,
                    merged_active_set,
                    try_delta_vec,
                    lagrangian_vec);

                std::vector<size_t> violated_indices;

                if (active_set.size() < K - fixed_active_set.size())
                {
                    for (size_t i = 0; i < inequality_constraint_count; i++)
                    {
                        if (merged_active_set.end() != std::find(
                                merged_active_set.begin(),
                                merged_active_set.end(),
                                i))
                            continue;

                        const auto lhs = coefficients_mat
                                .multiply_row(i, try_delta_vec);

                        if (lhs > b_vec[i])
                            violated_indices.push_back(i);
                    }
                }

                if (violated_indices.empty())
                {
                    delta_vec = try_delta_vec;

                    auto lagrangian_index = index_not_found;
                    for (size_t i = 0; i < active_set.size(); i++)
                    {
                        const auto lms_i = lagrangian_vec[i];
                        if (lms_i < value_type(0))
                            continue;
                        if (lagrangian_index == index_not_found ||
                            lms_i > lagrangian_vec[lagrangian_index])
                            lagrangian_index = i;
                    }

                    if (lagrangian_index == index_not_found)
                        return;

                    active_set.erase(
                        active_set.begin() +
                        int(lagrangian_index));
                }
                else
                {
                    const auto k_violated = _backtrack(
                            b_vec,
                            coefficients_mat,
                            delta_vec,
                            try_delta_vec,
                            violated_indices,
                            delta_vec);

                    if (k_violated == index_not_found)
                        continue;

                    assert(std::none_of(
                               active_set.begin(),
                               active_set.end(),
                               [k_violated](const size_t index) -> bool
                    {
                        return index == k_violated;
                    }));

                    assert(k_violated != index_not_found);
                    assert(k_violated < b_vec.get_length());
                    active_set.push_back(k_violated);
                }
            }
        }

    private:
        // --------------------------------------------------------------------
        static constexpr auto index_not_found =
            std::numeric_limits<size_t>::max();

        static constexpr auto epsilon =
            value_type(0.000001);

        // --------------------------------------------------------------------
        static size_t _backtrack(
                const matrix_type         & b_vec,
                const matrix_type         & coefficients_mat,
                const matrix_type         & old_delta_vec,
                const matrix_type         & violated_delta_vec,
                const std::vector<size_t> & violated_indices,
                matrix_type               & new_delta_vec)
        {
            assert(violated_delta_vec.is_size(old_delta_vec));
            assert(b_vec.get_length() == coefficients_mat.get_height());

            assert(std::all_of(
                       violated_indices.begin(),
                       violated_indices.end(),
                       [&b_vec](const size_t i) -> bool
                   {
                       return i < b_vec.get_length();
                   }));

            auto min_i = index_not_found;
            auto min_t = value_type(0);

            const auto diff_vec = violated_delta_vec - old_delta_vec;

            for (size_t i = 0; i < violated_indices.size(); i++)
            {
                const auto violated_index = violated_indices[i];
                const auto b_scalar = b_vec[violated_index];

                const auto coefficient_vec =
                    coefficients_mat.copy_row(violated_index);

                const auto denominator =
                    (coefficient_vec * diff_vec).get_value(0, 0);

                const auto numerator = b_scalar -
                    (coefficient_vec * old_delta_vec).get_value(0, 0);

                if (std::fabs(denominator) < epsilon)
                    continue;

                const auto t = numerator / denominator;
                if (min_i == index_not_found || t < min_t)
                {
                    min_i = i;
                    min_t = t;
                }
            }

            if (min_i == index_not_found)
                return index_not_found;

            new_delta_vec = (min_t * diff_vec) + old_delta_vec;
            assert(!new_delta_vec.contains_inf());
            assert(!new_delta_vec.contains_nan());
            return violated_indices[min_i];
        }

        // --------------------------------------------------------------------
        static void _create_kkt_mat(
                const matrix_type         & b_vec,
                const matrix_type         & coefficients_mat,
                const matrix_type         & hessian_mat,
                const matrix_type         & derivative_vec,
                const std::vector<size_t> & active_set,
                matrix_type               & kkt_mat)
        {
            const auto K            = hessian_mat.get_height();
            const auto active_count = active_set.size();

            assert(b_vec.get_length() == coefficients_mat.get_height());
            assert(hessian_mat.is_square());
            assert(kkt_mat.get_height() == K + active_count);
            assert(kkt_mat.get_width() == K + active_count + 1);
            assert(coefficients_mat.get_width() == K);
            assert(derivative_vec.is_vector());
            assert(derivative_vec.get_length() == K);

            assert(std::all_of(
                       active_set.begin(),
                       active_set.end(),
                       [&b_vec](size_t i) -> bool {
                       return i < b_vec.get_length();
                   }));

            assert(std::all_of(
                       active_set.begin(),
                       active_set.end(),
                       [&coefficients_mat](size_t i) -> bool {
                       return i < coefficients_mat.get_height();
                   }));

            kkt_mat.set_values(value_type(0));
            for (size_t k1 = 0; k1 < K; k1++)
                for (size_t k2 = 0; k2 < K; k2++)
                    kkt_mat(k1, k2) = hessian_mat(k1, k2);

            for (size_t i = 0; i < active_count; i++)
                for (size_t k = 0; k < K; k++)
                    kkt_mat(i + K, k) = kkt_mat(k, i + K) =
                            coefficients_mat(active_set[i], k);

            const auto right_column_index = kkt_mat.get_width() - 1;

            for (size_t k = 0; k < K; k++)
                kkt_mat(k, right_column_index) = -derivative_vec[k];

            for (size_t i = 0; i < active_count; i++)
                kkt_mat(K + i, right_column_index) = b_vec[active_set[i]];
        }

        // --------------------------------------------------------------------
        static void _kkt(
                const matrix_type         & b_vec,
                const matrix_type         & coefficients_mat,
                const matrix_type         & hessian_mat,
                const matrix_type         & derivative_vec,
                const std::vector<size_t> & active_set,
                matrix_type               & delta_vec,
                matrix_type               & lagrangian_vec)
        {
            const auto active_count = active_set.size();
            const auto K            = derivative_vec.get_length();

            assert(delta_vec.is_vector());
            assert(delta_vec.get_length() == K);
            assert(lagrangian_vec.is_empty() || lagrangian_vec.is_vector());
            assert(lagrangian_vec.get_length() == active_count);

            matrix_type kkt_mat (K + active_count, K + active_count + 1);
            _create_kkt_mat(
                        b_vec,
                        coefficients_mat,
                        hessian_mat,
                        derivative_vec,
                        active_set,
                        kkt_mat);

            kkt_mat.gesv();

            assert(!kkt_mat.contains_inf());
            assert(!kkt_mat.contains_nan());

            const auto last_column = kkt_mat.get_width() - 1;

            for (size_t k = 0; k < K; k++)
                delta_vec[k] = kkt_mat(k, last_column);

            for (size_t i = 0; i < active_count; i++)
                lagrangian_vec[i] = kkt_mat(K + i, last_column);
        }
    };
}

#endif // JADE_QPAS_HPP__
