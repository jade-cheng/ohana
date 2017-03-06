#include "test.main.hpp"
#include "jade.settings.hpp"

namespace
{
    typedef double                           value_type;
    typedef jade::basic_settings<value_type> settings_type;
    typedef jade::basic_matrix<value_type>   matrix_type;

    const auto epsilon = value_type(0.00001);

    // ------------------------------------------------------------------------
    void test_main()
    {
        const matrix_type rf
        {
            { +0.1, -0.9, +0.5 },
            { -0.2, +0.3, -0.6 },
            { +0.8, -0.7, +0.4 }
        };

        const matrix_type mu
        {
            { 0.1 },
            { 0.3 },
            { 0.2 },
        };

        const auto actual = settings_type::create_c(rf, mu);

        const matrix_type expected
        {
            { +2.87103, -1.73843, +2.79200 },
            { -1.73843, +1.11150, -1.39622 },
            { +2.79200, -1.39622, +4.18689 }
        };

        TEST_EQUAL(size_t(3), actual.get_width());
        TEST_EQUAL(size_t(3), actual.get_height());

        TEST_ALMOST(expected(0, 0), actual(0, 0), epsilon);
        TEST_ALMOST(expected(0, 1), actual(0, 1), epsilon);
        TEST_ALMOST(expected(0, 2), actual(0, 2), epsilon);
        TEST_ALMOST(expected(1, 0), actual(1, 0), epsilon);
        TEST_ALMOST(expected(1, 1), actual(1, 1), epsilon);
        TEST_ALMOST(expected(1, 2), actual(1, 2), epsilon);
        TEST_ALMOST(expected(2, 0), actual(2, 0), epsilon);
        TEST_ALMOST(expected(2, 1), actual(2, 1), epsilon);
        TEST_ALMOST(expected(2, 2), actual(2, 2), epsilon);
    }
}

namespace test
{
    test::test_group nemeco {
        TEST_CASE(::test_main)
    };
}
