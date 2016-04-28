/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.args.hpp"

namespace
{
    const double epsilon = 0.000001;

    // ------------------------------------------------------------------------
    void constructor()
    {
        {
            jade::args args {"", "B", "C"};

            TEST_EQUAL(size_t(2), args.get_length());
            TEST_EQUAL(std::string("B"), args.pop<std::string>());
            TEST_EQUAL(std::string("C"), args.pop<std::string>());

            TEST_THROWS(jade::args x ({nullptr, nullptr}));
        }

        {
            const int argc = 3;
            const char * argv[] = {nullptr, "B", "C"};
            jade::args args (argc, argv);

            TEST_EQUAL(size_t(2), args.get_length());
            TEST_EQUAL(std::string("B"), args.pop<std::string>());
            TEST_EQUAL(std::string("C"), args.pop<std::string>());

            TEST_THROWS(jade::args x (0, argv));

            argv[2] = nullptr;
            TEST_THROWS(jade::args x (argc, argv));
        }
    }

    // ------------------------------------------------------------------------
    void get_length()
    {
        jade::args args {"a", "b"};
        TEST_EQUAL(size_t(1), args.get_length());

        args.pop<std::string>();
        TEST_EQUAL(size_t(0), args.get_length());
    }

    // ------------------------------------------------------------------------
    void is_empty()
    {
        jade::args args {"a", "b"};
        TEST_FALSE(args.is_empty());

        args.pop<std::string>();
        TEST_TRUE(args.is_empty());
    }

    // ------------------------------------------------------------------------
    void pop()
    {
        jade::args args {"a", "abc", "1", "2.0"};

        TEST_THROWS(args.pop<int>());

        TEST_EQUAL(std::string("abc"), args.pop<std::string>());
        TEST_EQUAL(1, args.pop<int>());
        TEST_ALMOST(2.0, args.pop<double>(), epsilon);

        TEST_THROWS(args.pop<std::string>());
    }

    // ------------------------------------------------------------------------
    void read()
    {
        {
            jade::args args {"", "x", "-b", "3", "y", "--c", "4", "-d"};

            TEST_EQUAL(3, args.read<int>("--b", "-b", -1));
            TEST_EQUAL(size_t(5), args.get_length());

            TEST_EQUAL(4, args.read<int>("--c", "-c", -1));
            TEST_EQUAL(size_t(3), args.get_length());

            TEST_EQUAL(std::string("x"), args.pop<std::string>());
            TEST_EQUAL(std::string("y"), args.pop<std::string>());

            TEST_EQUAL(42, args.read<int>("--h", "-h", 42));

            TEST_THROWS(args.read<int>("--d", "-d", -1))
        }

        {
            jade::args args {"", "-a", "1", "--a", "2", "-b", "-b", "3"};
            TEST_THROWS(args.read<int>("--a", "-a", 0));
            TEST_THROWS(args.read<int>("--b", "-b", 0));
        }
    }

    // ------------------------------------------------------------------------
    void read_flag()
    {
        {
            jade::args args {"", "-a", "42", "--b", "501"};

            TEST_TRUE(args.read_flag("--a", "-a"));
            TEST_EQUAL(size_t(3), args.get_length());

            TEST_TRUE(args.read_flag("--b", "-b"));
            TEST_EQUAL(size_t(2), args.get_length());

            TEST_FALSE(args.read_flag("--c", "-c"));
            TEST_EQUAL(size_t(2), args.get_length());

            TEST_EQUAL(42, args.pop<int>());
            TEST_EQUAL(501, args.pop<int>());
            TEST_TRUE(args.is_empty());
        }

        {
            jade::args args {"", "-a", "1", "-a"};
            TEST_THROWS(args.read_flag("-a", "-a"));
        }
    }

    // ------------------------------------------------------------------------
    void validate_empty()
    {
        jade::args args {"", "1"};

        TEST_THROWS(args.validate_empty());
        args.pop<int>();
        args.validate_empty();
    }

    // ------------------------------------------------------------------------
    void validate_length()
    {
        jade::args args {"", "1"};

        args.validate_length(1);
        TEST_THROWS(args.validate_length(2));
    }
}

namespace test
{
    test_group args {
        TEST_CASE(constructor),
        TEST_CASE(get_length),
        TEST_CASE(is_empty),
        TEST_CASE(pop),
        TEST_CASE(read),
        TEST_CASE(read_flag),
        TEST_CASE(validate_empty),
        TEST_CASE(validate_length)
    };
}
