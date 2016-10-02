#include "test.main.hpp"
#include "jade.rema.hpp"
#include "jade.matrix.hpp"

namespace
{
    // ------------------------------------------------------------------------
    void test_dgm()
    {
        const auto s = R"(
           2 3
           0 1 2
           1 2 3
        )";

        std::istringstream in1 (s);
        std::ostringstream out1;
        jade::args args1 { "", "-s", "0", "2" };
        jade::rema rema1 (args1);
        rema1.execute(in1, out1);

        std::istringstream in2 (s);
        std::ostringstream out2;
        jade::args args2 { "", "-s", "0", "2" };
        jade::rema rema2 (args2);
        rema2.execute(in2, out2);

        TEST_EQUAL(out1.str(), out2.str());

        std::istringstream in3 (out1.str());
        const jade::basic_matrix<int> m (in3);

        TEST_TRUE(m.is_size(2, 2));

        const auto n0 = m(0, 0);
        const auto n1 = m(0, 1);
        TEST_TRUE(n0 < n1);

        TEST_EQUAL(n0 + 1, m(1, 0));
        TEST_EQUAL(n1 + 1, m(1, 1));
    }

    // ------------------------------------------------------------------------
    void test_error_reading_matrix_dimensions()
    {
        struct s {
            static void fn() {
                jade::args args { "", "1" };
                jade::rema rema (args);
                std::istringstream in ("row col bad data");
                std::ostringstream out;
                rema.execute(in, out);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn());
    }

    // ------------------------------------------------------------------------
    void test_lgm()
    {
        const auto s = R"(
           2 3
           0 1 2
           1 2 3

           2 3
           2 3 4
           3 4 5

           2 3
           4 5 6
           5 6 7
        )";

        std::istringstream in1 (s);
        std::ostringstream out1;
        jade::args args1 { "", "-s", "0", "2" };
        jade::rema rema1 (args1);
        rema1.execute(in1, out1);

        std::istringstream in2 (s);
        std::ostringstream out2;
        jade::args args2 { "", "-s", "0", "2" };
        jade::rema rema2 (args2);
        rema2.execute(in2, out2);

        TEST_EQUAL(out1.str(), out2.str());

        std::istringstream in3 (out1.str());
        const jade::basic_matrix<int> m1 (in3);
        const jade::basic_matrix<int> m2 (in3);
        const jade::basic_matrix<int> m3 (in3);

        TEST_TRUE(m1.is_size(2, 2));
        TEST_TRUE(m2.is_size(2, 2));
        TEST_TRUE(m3.is_size(2, 2));

        const auto n0 = m1(0, 0);
        const auto n1 = m1(0, 1);
        TEST_TRUE(n0 < n1);

        TEST_EQUAL(n0 + 1, m1(1, 0));
        TEST_EQUAL(n0 + 2, m2(0, 0));
        TEST_EQUAL(n0 + 3, m2(1, 0));
        TEST_EQUAL(n0 + 4, m3(0, 0));
        TEST_EQUAL(n0 + 5, m3(1, 0));

        TEST_EQUAL(n1 + 1, m1(1, 1));
        TEST_EQUAL(n1 + 2, m2(0, 1));
        TEST_EQUAL(n1 + 3, m2(1, 1));
        TEST_EQUAL(n1 + 4, m3(0, 1));
        TEST_EQUAL(n1 + 5, m3(1, 1));
    }

    // ------------------------------------------------------------------------
    void test_error_reading_second_matrix_dimensions()
    {
        struct s {
            static void fn() {
                jade::args args { "", "1" };
                jade::rema rema (args);
                std::istringstream in ("1 1 0 x");
                std::ostringstream out;
                rema.execute(in, out);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn());
    }

    // ------------------------------------------------------------------------
    void test_error_reading_third_matrix_dimensions()
    {
        struct s {
            static void fn() {
                jade::args args { "", "1" };
                jade::rema rema (args);
                std::istringstream in ("1 1 0 1 1 1");
                std::ostringstream out;
                rema.execute(in, out);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn());
    }

    // ------------------------------------------------------------------------
    void test_invalid_argument()
    {
        struct s {
            static void fn(char const * const n) {
                jade::args args { "", n };
                jade::rema rema (args);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn("x"));
        TEST_THROWS(s::fn("--foo"));
    }

    // ------------------------------------------------------------------------
    void test_missing_argument_for_option()
    {
        struct s {
            static void fn(char const * const n) {
                jade::args args { "", n };
                jade::rema rema (args);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn("--seed"));
    }

    // ------------------------------------------------------------------------
    void test_inconsistent_second_matrix_dimensions()
    {
        struct s {
            static void fn() {
                jade::args args { "", "1" };
                jade::rema rema (args);
                std::istringstream in ("1 1 0 2 2");
                std::ostringstream out;
                rema.execute(in, out);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn());
    }

    // ------------------------------------------------------------------------
    void test_inconsistent_third_matrix_dimensions()
    {
        struct s {
            static void fn() {
                jade::args args { "", "1" };
                jade::rema rema (args);
                std::istringstream in ("1 1 0 1 1 0 2 2 0");
                std::ostringstream out;
                rema.execute(in, out);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn());
    }

    // ------------------------------------------------------------------------
    void test_invalid_number_of_markers()
    {
        struct s {
            static void fn(char const * const n) {
                jade::args args { "", n };
                jade::rema rema (args);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn("0"));
    }

    // ------------------------------------------------------------------------
    void test_unexpected_end_of_matrix_data()
    {
        struct s {
            static void fn() {
                jade::args args { "", "1" };
                jade::rema rema (args);
                std::istringstream in ("1 1");
                std::ostringstream out;
                rema.execute(in, out);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn());
    }

    // ------------------------------------------------------------------------
    void test_unexpected_symbol_after_matrix_data()
    {
        struct s {
            static void fn() {
                jade::args args { "", "1" };
                jade::rema rema (args);
                std::istringstream in ("1 1 0 1 1 0 1 1 0 x");
                std::ostringstream out;
                rema.execute(in, out);
                TEST_FAIL("expected exception");
            }
        };

        TEST_THROWS(s::fn());
    }
}

namespace test
{
    test::test_group rema {
        TEST_CASE(::test_dgm),
        TEST_CASE(::test_error_reading_matrix_dimensions),
        TEST_CASE(::test_error_reading_second_matrix_dimensions),
        TEST_CASE(::test_error_reading_third_matrix_dimensions),
        TEST_CASE(::test_inconsistent_second_matrix_dimensions),
        TEST_CASE(::test_inconsistent_third_matrix_dimensions),
        TEST_CASE(::test_invalid_argument),
        TEST_CASE(::test_invalid_number_of_markers),
        TEST_CASE(::test_lgm),
        TEST_CASE(::test_missing_argument_for_option),
        TEST_CASE(::test_unexpected_end_of_matrix_data),
        TEST_CASE(::test_unexpected_symbol_after_matrix_data)
    };
}
