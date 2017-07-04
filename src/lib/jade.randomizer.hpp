/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_RANDOMIZER_HPP__
#define JADE_RANDOMIZER_HPP__

#include "jade.matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that randomizes matrices.
    ///
    template <typename TValue>
    class basic_randomizer
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        ///
        /// Initializes a new instance of the class.
        ///
        inline basic_randomizer()
            : _engine ()
        {
        }

        ///
        /// \return The random number engine.
        ///
        inline std::default_random_engine & get_engine()
        {
            return _engine;
        }

        ///
        /// \return A random F matrix.
        ///
        matrix_type randomize_f(
                const size_t        K,  ///< The component count.
                const matrix_type & mu) ///< The mu vector.
        {
            static const auto sigma   = value_type(0.1);
            static const auto epsilon = value_type(1.0e-6);
            static const auto min     = value_type(0.0) + epsilon;
            static const auto max     = value_type(1.0) - epsilon;

            assert(K > 0);
            assert(mu.is_column_vector());
            assert(mu.all_of([](const value_type n)
                { return n >= min && n <= max; }));

            const auto J = mu.get_length();

            matrix_type f (K, J);

            for (size_t j = 0; j < J; j++)
            {
                static std::normal_distribution<value_type>
                    dist (mu[j], sigma);

                for (size_t k = 0; k < K; k++)
                    f(k, j) = std::min(std::max(
                        min, dist(_engine)), max);
            }

            return f;
        }

        ///
        /// \return A random Q matrix.
        ///
        matrix_type randomize_q(
                const size_t I, ///< The number of individuals.
                const size_t K) ///< The number of components.
        {
            static const auto K_0 = value_type(0);
            static const auto K_1 = value_type(1);

            static std::uniform_real_distribution<value_type>
                distribution (K_0, K_1);

            matrix_type q (I, K);

            for (size_t i = 0; i < I; i++)
            {
                auto sum = K_0;
                for (size_t k = 0; k < K; k++)
                    sum += (q(i, k) = distribution(_engine));

                const auto factor = K_1 / sum;
                for (size_t k = 0; k < K; k++)
                    q(i, k) *= factor;
            }

            return q;
        }

    private:
        std::default_random_engine _engine;
    };
}

#endif // JADE_RANDOMIZER_HPP__
