/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.neoscan.hpp"

namespace
{
    typedef double                                           value_type;
    typedef jade::basic_neoscan<value_type>                  neoscan_type;
    typedef jade::basic_matrix<value_type>                   matrix_type;
    typedef jade::basic_discrete_genotype_matrix<value_type> dgm_type;
    typedef jade::basic_discrete_genotype_matrix<value_type> lgm_type;

    typedef neoscan_type::output    output_type;
    typedef std::queue<output_type> queue_type;

    const auto epsilon = 1.0e-6;

    const auto dgm_str = R"(
        3 4
        1 0 0 0
        0 1 2 0
        0 0 0 1
        )";

    const auto lgm_str = R"(
        3 4
        0.1  0.6  0.9  0.4
        0.5  0.2  0.7  0.1
        0.2  0.3  0.3  0.8

        3 4
        0.9  0.5  0.3  0.3
        0.8  0.6  0.2  0.4
        0.9  0.6  0.7  0.1

        3 4
        0.1  0.6  0.7  0.3
        0.2  0.5  0.8  0.2
        0.3  0.4  0.9  0.1
        )";

    const auto f_str = R"(
        2 4
        0.1  0.3  0.5  0.7
        0.2  0.4  0.6  0.8
        )";

    const auto q_str = R"(
        3 2
        0.2 0.8
        0.4 0.6
        0.5 0.5
        )";

    const auto y_str = R"(
        3 1
        0.0
        1.0
        2.0
        )";

    const auto expected_dgm_30 = []()
    {
        queue_type out;
        out.push({ +6.400005e-01, -2.586994e+00, -1.735003e+00 });
        out.push({ -2.999948e-02, -3.285576e+00, -3.284459e+00 });
        out.push({ -3.000055e-02, -4.491654e+00, -4.489274e+00 });
        out.push({ -2.222653e-01, -7.556465e+00, -7.188448e+00 });
        return out;
    }();

    const auto expected_lgm_30 = []()
    {
        queue_type out;
        out.push({ -1.999980e-01, -3.969019e-01, -1.667654e-01 });
        out.push({ +6.999980e-01, -8.615658e-01, -5.063768e-02 });
        out.push({ -1.599993e-01, -3.054189e+00, -3.028255e+00 });
        out.push({ -7.999980e-01, -3.028255e+00, -9.560749e-01 });
        return out;
    }();

    void test_dgm()
    {
        std::istringstream g_in (dgm_str);
        std::istringstream q_in (q_str);
        std::istringstream f_in (f_str);
        std::istringstream y_in (y_str);

        const dgm_type    g (g_in);
        const matrix_type q (q_in);
        const matrix_type f (f_in);
        const matrix_type y (y_in);

        neoscan_type neoscan (g, q, f, y, 30);

        auto expected = expected_dgm_30;

        neoscan.execute([&expected](const neoscan_type::output & actual)
        {
            TEST_FALSE(expected.empty());
            const auto & expect = expected.front();

            TEST_ALMOST(expect.delta,      actual.delta,      epsilon);
            TEST_ALMOST(expect.global_lle, actual.global_lle, epsilon);
            TEST_ALMOST(expect.local_lle,  actual.local_lle,  epsilon);

            expected.pop();
        });

        TEST_TRUE(expected.empty());
    }

    void test_lgm()
    {
        std::istringstream g_in (lgm_str);
        std::istringstream q_in (q_str);
        std::istringstream f_in (f_str);
        std::istringstream y_in (y_str);

        const lgm_type    g (g_in);
        const matrix_type q (q_in);
        const matrix_type f (f_in);
        const matrix_type y (y_in);

        neoscan_type neoscan (g, q, f, y, 30);

        auto expected = expected_lgm_30;

        neoscan.execute([&expected](const neoscan_type::output & actual)
        {
            TEST_FALSE(expected.empty());
            const auto & expect = expected.front();

            TEST_ALMOST(expect.delta,      actual.delta,      epsilon);
            TEST_ALMOST(expect.global_lle, actual.global_lle, epsilon);
            TEST_ALMOST(expect.local_lle,  actual.local_lle,  epsilon);

            expected.pop();
        });

        TEST_TRUE(expected.empty());
    }
}

namespace test
{
    test_group neoscan {
        TEST_CASE(test_dgm),
        TEST_CASE(test_lgm)
    };
}
