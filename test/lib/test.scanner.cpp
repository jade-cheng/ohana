/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.scanner.hpp"

namespace
{
    typedef double value_type;

    const auto epsilon = value_type(0.0001);

    // ------------------------------------------------------------------------
    void constructor()
    {
        {
            std::istringstream in (" abc def ");
            jade::scanner scanner (in);
            TEST_FALSE(scanner.try_char('x'));
            TEST_TRUE(scanner.try_char('a'));
            TEST_TRUE(scanner.try_char('b'));
            TEST_TRUE(scanner.try_char('c'));
            TEST_TRUE(scanner.try_char('d'));
            TEST_TRUE(scanner.try_char('e'));
            TEST_TRUE(scanner.try_char('f'));
            TEST_FALSE(scanner.try_char('x'));
        }

        {
            jade::scanner scanner (" abc def ");
            TEST_FALSE(scanner.try_char('x'));
            TEST_TRUE(scanner.try_char('a'));
            TEST_TRUE(scanner.try_char('b'));
            TEST_TRUE(scanner.try_char('c'));
            TEST_TRUE(scanner.try_char('d'));
            TEST_TRUE(scanner.try_char('e'));
            TEST_TRUE(scanner.try_char('f'));
            TEST_FALSE(scanner.try_char('x'));
        }
    }

    // ------------------------------------------------------------------------
    void expect()
    {
        jade::scanner scanner (" abc def ");
        TEST_THROWS(scanner.expect('x'));
        scanner.expect('a');
        TEST_THROWS(scanner.expect('x'));
        scanner.expect('b');
        TEST_THROWS(scanner.expect('x'));
        scanner.expect('c');
        TEST_THROWS(scanner.expect('x'));
        scanner.expect('d');
        TEST_THROWS(scanner.expect('x'));
        scanner.expect('e');
        TEST_THROWS(scanner.expect('x'));
        scanner.expect('f');
        TEST_THROWS(scanner.expect('x'));
    }

    // ------------------------------------------------------------------------
    void is_end_of_data()
    {
        jade::scanner scanner ("123 456");

        TEST_FALSE(scanner.is_end_of_data());

        scanner.read_token();
        TEST_FALSE(scanner.is_end_of_data());

        scanner.skip_whitespace();
        TEST_FALSE(scanner.is_end_of_data());

        scanner.read_token();
        TEST_TRUE(scanner.is_end_of_data());
    }

    // ------------------------------------------------------------------------
    void read_digits()
    {
        jade::scanner scanner ("123 456");

        std::ostringstream val1;
        scanner.read_digits(val1);
        TEST_EQUAL(std::string("123"), val1.str());

        scanner.skip_whitespace();

        std::ostringstream val2;
        scanner.read_digits(val2);
        TEST_EQUAL(std::string("456"), val2.str());

        std::ostringstream val3;
        scanner.read_digits(val3);
        TEST_EQUAL(std::string(""), val3.str());
    }

    // ------------------------------------------------------------------------
    void read_real()
    {
        jade::scanner pos (" 1 12 123. .1234 1234.5 1234.56");
        TEST_ALMOST(value_type(1.0), pos.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(12.0), pos.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(123.0), pos.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(0.1234), pos.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(1234.5), pos.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(1234.56), pos.read_real<value_type>(), epsilon);

        jade::scanner neg (" -1 -12 -123. -.1234 -1234.5 -1234.56");
        TEST_ALMOST(value_type(-1.0), neg.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(-12.0), neg.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(-123.0), neg.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(-0.1234), neg.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(-1234.5), neg.read_real<value_type>(), epsilon);
        TEST_ALMOST(value_type(-1234.56), neg.read_real<value_type>(), epsilon);

        TEST_THROWS(jade::scanner("").read_real<value_type>());
        TEST_THROWS(jade::scanner("a").read_real<value_type>());
        TEST_THROWS(jade::scanner(".x").read_real<value_type>());
        TEST_THROWS(jade::scanner("-x").read_real<value_type>());
    }

    // ------------------------------------------------------------------------
    void read_token()
    {
        {
            jade::scanner scanner (" abc def* ");
            TEST_EQUAL(std::string(""), scanner.read_token());

            scanner.skip_whitespace();
            TEST_EQUAL(std::string("abc"), scanner.read_token());

            TEST_EQUAL(std::string(" def"), scanner.read_token("*"));
            TEST_EQUAL(std::string("*"), scanner.read_token());
        }
    }

    // ------------------------------------------------------------------------
    void skip_whitespace()
    {
        TEST_EQUAL(
            std::string("  abc"),
            jade::scanner("  abc").read_token("*"));

        jade::scanner scanner ("  abc");
        scanner.skip_whitespace();

        TEST_EQUAL(
            std::string("abc"),
            scanner.read_token("*"));

        scanner.skip_whitespace();
    }

    // ------------------------------------------------------------------------
    void try_char()
    {
        jade::scanner scanner (" abc def ");
        TEST_FALSE(scanner.try_char('x'));
        TEST_TRUE(scanner.try_char('a'));
        TEST_FALSE(scanner.try_char('x'));
        TEST_TRUE(scanner.try_char('b'));
        TEST_FALSE(scanner.try_char('x'));
        TEST_TRUE(scanner.try_char('c'));
        TEST_FALSE(scanner.try_char('x'));
        TEST_TRUE(scanner.try_char('d'));
        TEST_FALSE(scanner.try_char('x'));
        TEST_TRUE(scanner.try_char('e'));
        TEST_FALSE(scanner.try_char('x'));
        TEST_TRUE(scanner.try_char('f'));
        TEST_FALSE(scanner.try_char('x'));
    }
}

namespace test
{
    test_group scanner {
        TEST_CASE(constructor),
        TEST_CASE(expect),
        TEST_CASE(is_end_of_data),
        TEST_CASE(read_digits),
        TEST_CASE(read_real),
        TEST_CASE(read_token),
        TEST_CASE(skip_whitespace),
        TEST_CASE(try_char)
    };
}
