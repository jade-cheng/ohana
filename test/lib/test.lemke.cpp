/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.lemke.hpp"

namespace
{
    typedef double                         value_type;
    typedef jade::basic_lemke<value_type>  lemke_type;
    typedef jade::basic_matrix<value_type> matrix_type;

    static const auto epsilon = value_type(0.0001);

    // ------------------------------------------------------------------------
    void constructor()
    {
        const matrix_type q {
            { 2, 0 },
            { 0, 2 }
        };

        const matrix_type a {
            { +1, +0 },
            { +0, +1 },
            { -1, -1 },
            { +1, +1 },
            { -1, -1 }
        };

        const matrix_type c {
            { -8 },
            { -6 }
        };

        const matrix_type b {
            { +0 },
            { +0 },
            { -5 },
            { +2 },
            { -2 }
        };

        const matrix_type t {
            { 1, 0, 0, 0, 0, 0, 0, -2, -0, +1, +0, -1, +1, -1, -1, -8 },
            { 0, 1, 0, 0, 0, 0, 0, -0, -2, +0, +1, -1, +1, -1, -1, -6 },
            { 0, 0, 1, 0, 0, 0, 0, -1, -0, -0, -0, -0, -0, -0, -1, -0 },
            { 0, 0, 0, 1, 0, 0, 0, -0, -1, -0, -0, -0, -0, -0, -1, -0 },
            { 0, 0, 0, 0, 1, 0, 0, +1, +1, -0, -0, -0, -0, -0, -1, +5 },
            { 0, 0, 0, 0, 0, 1, 0, -1, -1, -0, -0, -0, -0, -0, -1, -2 },
            { 0, 0, 0, 0, 0, 0, 1, +1, +1, -0, -0, -0, -0, -0, -1, +2 }
        };

        lemke_type lemke (q, a, c, b);
        const auto & actual = lemke.get_tableau();

        TEST_EQUAL(t.get_height(), actual.get_height());
        TEST_EQUAL(t.get_width(), actual.get_width());

        for (size_t i = 0; i < t.get_height(); i++)
            for (size_t j = 0; j < t.get_height(); j++)
                TEST_ALMOST(t(i, j), actual(i, j), epsilon);

        const auto & labels = lemke.get_labels();
        TEST_EQUAL(t.get_height(), labels.size());
        for (size_t i = 0; i < labels.size(); i++)
            TEST_EQUAL(i, labels[i]);
    }

    // ------------------------------------------------------------------------
    void iterate()
    {
        const matrix_type t {
            { +1, +0, -2, -1, -1, -6 },
            { +0, +1, +1, +0, -1, +4 }
        };

        lemke_type lemke (t);

        TEST_TRUE(lemke.is_executing());
        TEST_EQUAL(lemke_type::state::executing, lemke.get_state());

        lemke.iterate();
        TEST_TRUE(lemke.is_executing());
        TEST_EQUAL(lemke_type::state::executing, lemke.get_state());

        lemke.iterate();
        TEST_FALSE(lemke.is_executing());
        TEST_EQUAL(lemke_type::state::completed, lemke.get_state());
    }

    // ------------------------------------------------------------------------
    void output()
    {
        const matrix_type t {
            { 1, 0, 0, 0, 0, 0, 0, -2, -0, +1, +0, -1, +1, -1, -1, -8 },
            { 0, 1, 0, 0, 0, 0, 0, -0, -2, +0, +1, -1, +1, -1, -1, -6 },
            { 0, 0, 1, 0, 0, 0, 0, -1, -0, -0, -0, -0, -0, -0, -1, -0 },
            { 0, 0, 0, 1, 0, 0, 0, -0, -1, -0, -0, -0, -0, -0, -1, -0 },
            { 0, 0, 0, 0, 1, 0, 0, +1, +1, -0, -0, -0, -0, -0, -1, +5 },
            { 0, 0, 0, 0, 0, 1, 0, -1, -1, -0, -0, -0, -0, -0, -1, -2 },
            { 0, 0, 0, 0, 0, 0, 1, +1, +1, -0, -0, -0, -0, -0, -1, +2 }
        };

        matrix_type output;
        const bool is_solved = lemke_type::solve(output, t);

        TEST_TRUE(is_solved);
        TEST_EQUAL(size_t(7), output.get_length());
        TEST_ALMOST(value_type(1.5), output[0], epsilon);
        TEST_ALMOST(value_type(0.5), output[1], epsilon);
        TEST_ALMOST(value_type(0.0), output[2], epsilon);
        TEST_ALMOST(value_type(0.0), output[3], epsilon);
        TEST_ALMOST(value_type(0.0), output[4], epsilon);
        TEST_ALMOST(value_type(0.0), output[5], epsilon);
        TEST_ALMOST(value_type(5.0), output[6], epsilon);
    }

    // ------------------------------------------------------------------------
    void solve()
    {
        const matrix_type t {
            { +1, +0, -2, -1, -1, -6 },
            { +0, +1, +1, +0, -1, +4 }
        };

        lemke_type lemke (t);
        TEST_TRUE(lemke.is_executing());
        TEST_EQUAL(lemke_type::state::executing, lemke.get_state());

        const auto completed = lemke.solve();

        TEST_TRUE(completed);
        TEST_FALSE(lemke.is_executing());
        TEST_EQUAL(lemke_type::state::completed, lemke.get_state());

        const auto out = lemke.get_output();
        TEST_EQUAL(size_t(2), out.get_height());
        TEST_EQUAL(size_t(1), out.get_width());
        TEST_ALMOST(value_type(3.0), out[0], epsilon);
        TEST_ALMOST(value_type(0.0), out[1], epsilon);

        matrix_type out2;
        const auto solved = lemke_type::solve(out2, t);
        TEST_TRUE(solved);
        TEST_ALMOST(out[0], out2[0], epsilon);
        TEST_ALMOST(out[1], out2[1], epsilon);
    }
}

namespace test
{
    test_group lemke {
        TEST_CASE(constructor),
        TEST_CASE(iterate),
        TEST_CASE(output),
        TEST_CASE(solve)
    };
}
