/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.shunting_yard.hpp"

namespace
{
    typedef double                                float_type;
    typedef jade::basic_shunting_yard<float_type> evaluator_type;
    typedef evaluator_type::args_type             args_type;

    static const float_type epsilon = 1.0e-6;

    // ------------------------------------------------------------------------
    void test_evaluation(
            const float_type    expected,
            const std::string & expression,
            const args_type   & args = args_type())
    {
        const evaluator_type e (expression);
        const auto actual = e.evaluate(args);
        TEST_ALMOST(expected, actual, epsilon);
    }

    // ------------------------------------------------------------------------
    void test_evaluation(
            const float_type    expected,
            const std::string & expression,
            const float_type    x)
    {
        const args_type args { { "x", x } };
        test_evaluation(expected, expression, args);
    }

    // ------------------------------------------------------------------------
    void test_evaluation(
            const float_type    expected,
            const std::string & expression,
            const float_type    x,
            const float_type    y)
    {
        const args_type args { { "x", x }, { "y", y } };
        test_evaluation(expected, expression, args);
    }
}

namespace
{
    // ------------------------------------------------------------------------
    void constructor()
    {
        evaluator_type e ("0.1 + (1.0 * (a + b)) - c * d / ef1_2");

        const auto args = e.get_args();
        TEST_TRUE(args.end() != args.find("a"));
        TEST_TRUE(args.end() != args.find("b"));
        TEST_TRUE(args.end() != args.find("c"));
        TEST_TRUE(args.end() != args.find("d"));
        TEST_TRUE(args.end() != args.find("ef1_2"));
        TEST_TRUE(args.end() == args.find("g"));
    }

    // ------------------------------------------------------------------------
    void addition()
    {
        ::test_evaluation(3.0, "1+2");
        ::test_evaluation(4.6, "1.2 + 3.4");
        ::test_evaluation(8.7, "1.2 + 3 + 4.5");
    }

    // ------------------------------------------------------------------------
    void division()
    {
        ::test_evaluation(2.000000, "2/1");
        ::test_evaluation(0.521739, "1.2 / 2.3");
        ::test_evaluation(0.115942, "1.2 / 2.3 / 4.5");
    }

    // ------------------------------------------------------------------------
    void main()
    {
        ::test_evaluation(1.00, "1.0");
        ::test_evaluation(1.00, "x", 1.0);
        ::test_evaluation(3.00, "x+y", 1.0, 2.0);
        ::test_evaluation(1.00, "y-x", 1.0, 2.0);
        ::test_evaluation(7.00, "3*x+2*y", 1.0, 2.0);
        ::test_evaluation(1.00, "x/3-y/2", 9.0, 4.0);
        ::test_evaluation(18.0, "3*(x+2)*y", 1.0, 2.0);
        ::test_evaluation(1.00, "3/(x-2)/y", 8.0, 0.5);
        ::test_evaluation(1.00, "(1)");
        ::test_evaluation(3.00, "(((3)))");
    }

    // ------------------------------------------------------------------------
    void multiplication()
    {
        ::test_evaluation(2.0000, "1*2");
        ::test_evaluation(4.0800, "1.2 * 3.4");
        ::test_evaluation(22.848, "1.2 * 3.4 * 5.6");
    }

    // ------------------------------------------------------------------------
    void subtraction()
    {
        ::test_evaluation(8.0, "9-1");
        ::test_evaluation(6.8, "9.1 - 2.3");
        ::test_evaluation(2.3, "9.1 - 2.3 - 4.5");
    }
}

namespace test
{
    test_group shunting_yard {
        TEST_CASE(addition),
        TEST_CASE(constructor),
        TEST_CASE(division),
        TEST_CASE(main),
        TEST_CASE(multiplication),
        TEST_CASE(subtraction)
    };
}
