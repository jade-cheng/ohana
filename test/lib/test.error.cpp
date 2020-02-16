/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.error.hpp"

namespace
{
    // ------------------------------------------------------------------------
    void constructor()
    {
        {
            jade::error e;
            TEST_EQUAL(std::string(""), e.str());
        }

        {
            jade::error e ("abc");
            TEST_EQUAL(std::string("abc"), e.str());
        }

        {
            jade::error e (std::string("abc"));
            TEST_EQUAL(std::string("abc"), e.str());
        }
    }

    // ------------------------------------------------------------------------
    void stream_operator()
    {
        jade::error e;
        e << "abc" << 42 << " " << 1.5 << " " << std::string("x");
        TEST_EQUAL(std::string("abc42 1.5 x"), e.str());
    }

    // ------------------------------------------------------------------------
    void str()
    {
        jade::error e ("abc");
        TEST_EQUAL(std::string("abc"), e.str());

        e << "def";
        TEST_EQUAL(std::string("abcdef"), e.str());

        e << 42;
        TEST_EQUAL(std::string("abcdef42"), e.str());
    }

    // ------------------------------------------------------------------------
    void what()
    {
        jade::error e ("abc");
        TEST_EQUAL(std::string("abc"), std::string(e.what()));

        e << "def";
        TEST_EQUAL(std::string("abcdef"), std::string(e.what()));

        e << 42;
        TEST_EQUAL(std::string("abcdef42"), std::string(e.what()));
    }
}

namespace test
{
    test_group error {
        TEST_CASE(constructor),
        TEST_CASE(str),
        TEST_CASE(stream_operator),
        TEST_CASE(what)
    };
}
