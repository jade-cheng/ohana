/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.vec2.hpp"

namespace
{
    typedef double real_type;
    typedef int    int_type;
    typedef jade::basic_vec2<int_type>  int_vec2_type;
    typedef jade::basic_vec2<real_type> real_vec2_type;

    const auto epsilon = real_type(0.0001);

    // ------------------------------------------------------------------------
    void constructor()
    {
        typedef int_type      value_type;
        typedef int_vec2_type vec2_type;

        vec2_type v1;
        TEST_EQUAL(value_type(0), v1.x);
        TEST_EQUAL(value_type(0), v1.y);

        vec2_type v2 (1, 2);
        TEST_EQUAL(value_type(1), v2.x);
        TEST_EQUAL(value_type(2), v2.y);
    }

    // ------------------------------------------------------------------------
    void cross()
    {
        typedef int_type      value_type;
        typedef int_vec2_type vec2_type;

        const vec2_type lhs (2, 1);
        const vec2_type rhs (3, 4);

        TEST_EQUAL(value_type(5), vec2_type::cross(lhs, rhs));
    }

    // ------------------------------------------------------------------------
    void distance()
    {
        typedef real_type      value_type;
        typedef real_vec2_type vec2_type;

        const vec2_type lhs (1, 2);
        const vec2_type rhs (5, 5);

        TEST_ALMOST(
                value_type(25.0),
                vec2_type::distance_squared(lhs, rhs),
                epsilon);

        TEST_ALMOST(
                value_type(5.0),
                vec2_type::distance(lhs, rhs),
                epsilon);
    }

    // ------------------------------------------------------------------------
    void dot()
    {
        typedef int_type      value_type;
        typedef int_vec2_type vec2_type;

        const int_vec2_type lhs (1, 2);
        const int_vec2_type rhs (5, 5);

        TEST_EQUAL(value_type(5), vec2_type::dot(lhs, rhs).x);
        TEST_EQUAL(value_type(10), vec2_type::dot(lhs, rhs).y);
    }

    // ------------------------------------------------------------------------
    void length()
    {
        typedef real_type      value_type;
        typedef real_vec2_type vec2_type;

        const vec2_type lhs (3, 4);

        TEST_ALMOST(
                value_type(25.0),
                vec2_type(3, 4).get_length_squared(),
                epsilon);

        TEST_ALMOST(
                value_type(5.0),
                vec2_type(3, 4).get_length(),
                epsilon);
    }

    // ------------------------------------------------------------------------
    void lerp()
    {
        typedef real_type      value_type;
        typedef real_vec2_type vec2_type;

        const vec2_type src (1, 2);
        const vec2_type dst (5, 6);

        const auto v00 = vec2_type::lerp(src, dst, value_type(0.0));
        TEST_ALMOST(value_type(1.0), v00.x, epsilon);
        TEST_ALMOST(value_type(2.0), v00.y, epsilon);

        const auto v50 = vec2_type::lerp(src, dst, value_type(0.5));
        TEST_ALMOST(value_type(3.0), v50.x, epsilon);
        TEST_ALMOST(value_type(4.0), v50.y, epsilon);

        const auto v99 = vec2_type::lerp(src, dst, value_type(1.0));
        TEST_ALMOST(value_type(5.0), v99.x, epsilon);
        TEST_ALMOST(value_type(6.0), v99.y, epsilon);
    }

    // ------------------------------------------------------------------------
    void max()
    {
        typedef int_type      value_type;
        typedef int_vec2_type vec2_type;

        const vec2_type lhs (1, 5);
        const vec2_type rhs (2, 4);

        const auto v = vec2_type::max(lhs, rhs);
        TEST_EQUAL(value_type(2), v.x);
        TEST_EQUAL(value_type(5), v.y);
    }

    // ------------------------------------------------------------------------
    void min()
    {
        typedef int_type      value_type;
        typedef int_vec2_type vec2_type;

        const vec2_type lhs (1, 5);
        const vec2_type rhs (2, 4);

        const auto v = vec2_type::min(lhs, rhs);
        TEST_EQUAL(value_type(1), v.x);
        TEST_EQUAL(value_type(4), v.y);
    }

    // ------------------------------------------------------------------------
    void normalize()
    {
        typedef real_type      value_type;
        typedef real_vec2_type vec2_type;

        const auto v1 = vec2_type(100, 100);
        const auto u1 = vec2_type::normalize(v1);
        TEST_ALMOST(value_type(0.707107), u1.x, epsilon);
        TEST_ALMOST(value_type(0.707107), u1.y, epsilon);

        const auto v2 = vec2_type(0, 100);
        const auto u2 = vec2_type::normalize(v2);
        TEST_ALMOST(value_type(0.0), u2.x, epsilon);
        TEST_ALMOST(value_type(1.0), u2.y, epsilon);

        const auto v3 = vec2_type(100, 0);
        const auto u3 = vec2_type::normalize(v3);
        TEST_ALMOST(value_type(1.0), u3.x, epsilon);
        TEST_ALMOST(value_type(0.0), u3.y, epsilon);
    }

    // ------------------------------------------------------------------------
    void operators()
    {
        typedef int_type      value_type;
        typedef int_vec2_type vec2_type;

        const auto v1 = vec2_type(1, 2) + vec2_type(3, 4);
        TEST_EQUAL(value_type(4), v1.x);
        TEST_EQUAL(value_type(6), v1.y);

        const auto v2 = vec2_type(3, 4) - vec2_type(1, 2);
        TEST_EQUAL(value_type(2), v2.x);
        TEST_EQUAL(value_type(2), v2.y);

        const auto v3 = vec2_type(3, 4) * 2;
        TEST_EQUAL(value_type(6), v3.x);
        TEST_EQUAL(value_type(8), v3.y);

        const auto v4 = vec2_type(6, 4) / 2;
        TEST_EQUAL(value_type(3), v4.x);
        TEST_EQUAL(value_type(2), v4.y);

        vec2_type a (1, 2);

        a += 2;
        TEST_EQUAL(value_type(3), a.x);
        TEST_EQUAL(value_type(4), a.y);

        a += vec2_type(3, 4);
        TEST_EQUAL(value_type(6), a.x);
        TEST_EQUAL(value_type(8), a.y);

        a -= vec2_type(3, 4);
        TEST_EQUAL(value_type(3), a.x);
        TEST_EQUAL(value_type(4), a.y);

        a -= 2;
        TEST_EQUAL(value_type(1), a.x);
        TEST_EQUAL(value_type(2), a.y);

        a *= 4;
        TEST_EQUAL(value_type(4), a.x);
        TEST_EQUAL(value_type(8), a.y);

        a /= 4;
        TEST_EQUAL(value_type(1), a.x);
        TEST_EQUAL(value_type(2), a.y);
    }
}

namespace test
{
    test_group vec2 {
        TEST_CASE(constructor),
        TEST_CASE(cross),
        TEST_CASE(distance),
        TEST_CASE(dot),
        TEST_CASE(length),
        TEST_CASE(lerp),
        TEST_CASE(max),
        TEST_CASE(min),
        TEST_CASE(normalize),
        TEST_CASE(operators)
    };
}
