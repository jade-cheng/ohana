/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_LIKELIHOOD_HPP__
#define JADE_LIKELIHOOD_HPP__

#include "jade.matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that efficiently computes a log-likelihood based
    /// on 1) the log of the determinant for a covariance matrix and 2) the
    /// inverse of the covariance matrix. The class caches values based on a
    /// [K-1 x J] rooted F matrix for major allele frequencies and a [J x 1] mu
    /// vector.
    ///
    template <typename TValue>
    class basic_likelihood
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        ///
        /// Initializes the class based on a rooted F matrix for major allele
        /// frequencies and a mu vector; the constructor caches values based on
        /// the supplied arguments.
        ///
        basic_likelihood(
                const matrix_type & rf, ///< The rooted F matrix.
                const matrix_type & mu) ///< The mu vector.
            : _rf      (rf)
            , _mux     (_init_mux(mu))
            , _rkltmux (_init_rkltmux(rf.get_height(), _mux))
            , _mul     (rf.get_height(), rf.get_width())
        {
            assert(mu.get_height() == rf.get_width());
        }

        ///
        /// \return The log-likelihood using values cached during the
        /// initializaiton of this instance and the supplied arguments.
        ///
        value_type operator () (
                const matrix_type & c_inv,     ///< The inverted C matrix.
                const value_type    log_c_det) ///< The log of det(C).
        {
            const auto RK = _rf.get_height();
            const auto J  = _rf.get_width();

            assert(c_inv.is_size(RK, RK));

            //
            // Use LAPACK to multiply c_inv and _rf; store the result in the
            // pre-allocated _mul matrix, which is of size [RK x J].
            //
            matrix_type::gemm(c_inv, _rf, _mul);

            auto sum = value_type(0);

            //
            // Loop over the J columns of _mul.
            //
            auto       rkltmux_ptr = _rkltmux.get_data();
            auto       mux_ptr     = _mux.get_data();
            const auto mux_end     = _mux.get_data() + J;
            auto       rf_ptr      = _rf.get_data();
            auto       mul_ptr     = _mul.get_data();
            while (mux_ptr != mux_end)
            {
                //
                // Do not process columns with non-positive mux[j].
                //
                const auto rkltmux = *rkltmux_ptr++;
                const auto mux     = *mux_ptr++;
                if (mux <= value_type(0))
                    continue;

                auto zip = value_type(0);

                //
                // Loop over the RK rows of _mul and _rf and sum the products of
                // rf[rk, j] * mul[rk, j].
                //
                const auto rf_anchor  = rf_ptr;
                const auto mul_anchor = mul_ptr;
                const auto mul_end    = mul_ptr + (RK * J);
                while (mul_ptr != mul_end)
                {
                    zip     += *rf_ptr * *mul_ptr;
                    rf_ptr  += J;
                    mul_ptr += J;
                }

                //
                // For this column of J, add
                // RK*log(2*pi * mux[j]) + (zip / mux[j]).
                //
                sum += rkltmux + (zip / mux);

                //
                // Advance to the next columns.
                //
                rf_ptr  = rf_anchor + 1;
                mul_ptr = mul_anchor + 1;
            }

            //
            // The log-likelihood: -0.5 * (J * log(det(C)) + sum).
            //
            return value_type(-0.5) * ((value_type(J) * log_c_det) + sum);
        }

    private:
        // --------------------------------------------------------------------
        static matrix_type _init_mux(const matrix_type & mu)
        {
            assert(mu.is_column_vector());

            //
            // Declare a vector of the same size.
            //
            const auto J = mu.get_height();
            matrix_type mux (J, 1);

            //
            // Loop over all values in the mu vector.
            //
            const auto mu_end  = mu.get_data() + J;
            auto       mu_ptr  = mu.get_data();
            auto       mux_ptr = mux.get_data();
            while (mu_ptr != mu_end)
            {
                //
                // Cache the value for mu[j] * (1.0 - mu[j]).
                //
                const auto mu_j = *mu_ptr++;
                *mux_ptr++ = mu_j * (value_type(1) - mu_j);
            }

            return mux;
        }

        // --------------------------------------------------------------------
        static matrix_type _init_rkltmux(
                const size_t        RK,
                const matrix_type & mux)
        {
            static const auto tau = value_type(2.0 * std::acos(-1.0));

            assert(mux.is_column_vector());

            //
            // Declare a vector of the same size.
            //
            const auto J = mux.get_height();
            matrix_type rkltmux (J, 1);

            //
            // Loop over all values in the mux vector.
            //
            const auto mux_end     = mux.get_data() + J;
            auto       mux_ptr     = mux.get_data();
            auto       rkltmux_ptr = rkltmux.get_data();

            //
            // Cache the values for RK * log(2*pi * mux[j]).
            //
            while (mux_ptr != mux_end)
                *rkltmux_ptr++ = value_type(RK) * std::log(tau * *mux_ptr++);

            return rkltmux;
        }

        const matrix_type & _rf;      // rooted F matrix
        const matrix_type   _mux;     // cache of mu[j] * (1-mu[j])
        const matrix_type   _rkltmux; // cached of (K-1) * log(2*pi * mu[j]).
        matrix_type         _mul;     // placeholder for matrix multiplication.
    };
}

#endif // JADE_LIKELIHOOD_HPP__
