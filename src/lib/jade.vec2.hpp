/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_VEC2_HPP__
#define JADE_VEC2_HPP__

#include "jade.system.hpp"

namespace jade
{
    ///
    /// A template for a class that implements rank-two vector operations.
    ///
    template <typename TValue>
    class basic_vec2 {
    public:
        /// The value type.
        typedef TValue value_type;

        value_type x; ///< The X coordinate.
        value_type y; ///< The Y coordinate.

        ///
        /// Implements a new insatnce of the class.
        ///
        inline basic_vec2()
            : x (0)
            , y (0)
        {
        }

        ///
        /// Implements a new insatnce of the class with the specified values.
        ///
        inline basic_vec2(
                const value_type x_, ///< The initial X coordinate.
                const value_type y_) ///< The initial Y coordinate.
            : x (x_)
            , y (y_)
        {
        }

        ///
        /// \return The cross product for two vectors.
        ///
        inline static value_type cross(
                const basic_vec2 & lhs, ///< The first vector.
                const basic_vec2 & rhs) ///< The second vector.
        {
            return (lhs.x * rhs.y) - (lhs.y * rhs.x);
        }

        ///
        /// \return The distance between two vectors.
        ///
        inline static value_type distance(
                const basic_vec2 & a, ///< The first vector.
                const basic_vec2 & b) ///< The second vector.
        {
            return (a - b).get_length();
        }

        ///
        /// \return The distance between two vectors.
        ///
        inline static value_type distance_squared(
                const basic_vec2 & a, ///< The first vector.
                const basic_vec2 & b) ///< The second vector.
        {
            return (a - b).get_length_squared();
        }

        ///
        /// \return The dot product of two vectors.
        ///
        inline static basic_vec2 dot(
                const basic_vec2 & a, ///< The first vector.
                const basic_vec2 & b) ///< The second vector.
        {
            return basic_vec2(a.x * b.x, a.y * b.y);
        }

        ///
        /// \return The length of the vector.
        ///
        inline value_type get_length() const
        {
            return std::sqrt(get_length_squared());
        }

        ///
        /// \return The length of the vector.
        ///
        inline value_type get_length_squared() const
        {
            return (x * x) + (y * y);
        }

        ///
        /// \return A new vector from linearly interpolating between two
        /// other vectors.
        ///
        inline static basic_vec2 lerp(
                const basic_vec2 & src,     ///< The first vector.
                const basic_vec2 & dst,     ///< The second vector.
                const value_type   percent) ///< The percentage.
        {
            return src + (dst - src) * percent;
        }

        ///
        /// \return The maximum of two vectors.
        ///
        inline static basic_vec2 max(
                const basic_vec2 & a, ///< The first vector.
                const basic_vec2 & b) ///< The second vector.
        {
            return basic_vec2(std::max(a.x, b.x), std::max(a.y, b.y));
        }

        ///
        /// \return The minimum of two vectors.
        ///
        inline static basic_vec2 min(
                const basic_vec2 & a, ///< The first vector.
                const basic_vec2 & b) ///< The second vector.
        {
            return basic_vec2(std::min(a.x, b.x), std::min(a.y, b.y));
        }

        ///
        /// \return The normal for the specified vector.
        ///
        inline static basic_vec2 normalize(
                const basic_vec2 & v) ///< The vector.
        {
            return v / v.get_length();
        }

        ///
        /// \return This instance after the operation with the specified value.
        ///
        basic_vec2 & operator += (
                const basic_vec2 & rhs) ///< The right operand.
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        ///
        /// \return This instance after the operation with the specified value.
        ///
        basic_vec2 & operator += (
                const value_type & rhs) ///< The right operand.
        {
            x += rhs;
            y += rhs;
            return *this;
        }

        ///
        /// \return This instance after the operation with the specified value.
        ///
        basic_vec2 & operator -= (
                const basic_vec2 & rhs) ///< The right operand.
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        ///
        /// \return This instance after the operation with the specified value.
        ///
        basic_vec2 & operator -= (
                const value_type & rhs) ///< The right operand.
        {
            x -= rhs;
            y -= rhs;
            return *this;
        }

        ///
        /// \return This instance after the operation with the specified value.
        ///
        basic_vec2 & operator *= (
                const value_type & rhs) ///< The right operand.
        {
            x *= rhs;
            y *= rhs;
            return *this;
        }

        ///
        /// \return This instance after the operation with the specified value.
        ///
        basic_vec2 & operator /= (
                const value_type & rhs) ///< The right operand.
        {
            x /= rhs;
            y /= rhs;
            return *this;
        }

        ///
        /// \returns A new vector based on the specified operator.
        ///
        inline basic_vec2 operator + (
                const value_type & rhs) ///< The right operand.
                const
        {
            return basic_vec2(*this) += rhs;
        }

        ///
        /// \returns A new vector based on the specified operator.
        ///
        inline basic_vec2 operator + (
                const basic_vec2 & rhs) ///< The right operand.
                const
        {
            return basic_vec2(*this) += rhs;
        }

        ///
        /// \returns A new vector based on the specified operator.
        ///
        inline basic_vec2 operator - (
                const value_type & rhs) ///< The right operand.
                const
        {
            return basic_vec2(*this) -= rhs;
        }

        ///
        /// \returns A new vector based on the specified operator.
        ///
        inline basic_vec2 operator - (
                const basic_vec2 & rhs) ///< The right operand.
                const
        {
            return basic_vec2(*this) -= rhs;
        }

        ///
        /// \returns A new vector based on the specified operator.
        ///
        inline basic_vec2 operator * (
                const value_type & rhs) ///< The right operand.
                const
        {
            return basic_vec2(*this) *= rhs;
        }

        ///
        /// \returns A new vector based on the specified operator.
        ///
        inline basic_vec2 operator / (
                const value_type & rhs) ///< The right operand.
                const
        {
            return basic_vec2(*this) /= rhs;
        }
    };
}

///
/// \returns A new vector based on the product of a value and another vector.
///
template <typename TValue>
jade::basic_vec2<TValue> operator * (
    const TValue &                   lhs, ///< The left operand.
    const jade::basic_vec2<TValue> & rhs) ///< The left operand.
{
    return rhs * lhs;
}

///
/// Reads the specified vector from the input stream.
///
/// \return The input stream.
///
template <typename TValue>
std::istream & operator << (
        std::istream &             lhs, ///< The input stream.
        jade::basic_vec2<TValue> & rhs) ///< The vector.
{
    return lhs >> rhs.x >> rhs.y;
}

///
/// Writes the specified vector to the output stream.
///
/// \return The output stream.
///
template <typename TValue>
std::ostream & operator << (
        std::ostream &                   lhs, ///< The output stream.
        const jade::basic_vec2<TValue> & rhs) ///< The vector.
{
    return lhs << rhs.x << ' ' << rhs.y;
}

#endif // JADE_VEC2_HPP__
