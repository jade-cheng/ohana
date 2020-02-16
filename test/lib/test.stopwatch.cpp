/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.stopwatch.hpp"

namespace
{
    const double epsilon = 0.000001;

    // ------------------------------------------------------------------------
    struct mock
    {
        typedef double time_point;

        static time_point now()
        {
            static int n = 0;
            return static_cast<double>(++n);
        }
    };

    typedef jade::basic_stopwatch<mock> mock_stopwatch;

    // ------------------------------------------------------------------------
    void constructor()
    {
        mock_stopwatch sw;
    }

    // ------------------------------------------------------------------------
    void compare()
    {
        {
            mock_stopwatch sw;
            const auto t = 2.5;

            TEST_TRUE(sw.compare(t) < 0); // elapsed = 1.0
            TEST_FALSE(sw.compare(t) > 0); // elapsed = 2.0
            TEST_TRUE(sw.compare(t) > 0); // elapsed = 3.0
            TEST_FALSE(sw.compare(t) < 0); // elapsed = 4.0
        }

        {
            mock_stopwatch sw;
            const auto t = 2.5;

            TEST_TRUE(sw < t); // elapsed = 1.0
            TEST_FALSE(sw > t); // elapsed = 2.0
            TEST_TRUE(sw > t); // elapsed = 3.0
            TEST_FALSE(sw < t); // elapsed = 4.0
        }
    }

    // ------------------------------------------------------------------------
    void elapsed()
    {
        mock_stopwatch sw;
        TEST_ALMOST(1.0, sw.get_elapsed(), epsilon);
        TEST_ALMOST(2.0, sw.get_elapsed(), epsilon);
        TEST_ALMOST(3.0, sw.get_elapsed(), epsilon);
        TEST_EQUAL(std::string("4.000000"), sw.str());

        std::ostringstream out;
        out << sw;
        TEST_EQUAL(std::string("5.000000"), out.str());
    }
}

namespace test
{
    test_group stopwatch {
        TEST_CASE(constructor),
        TEST_CASE(compare),
        TEST_CASE(elapsed)
    };
}
