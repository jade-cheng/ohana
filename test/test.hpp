/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef TEST_HPP__
#define TEST_HPP__

#include <cmath>
#include <cstring>
#include <exception>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <vector>

namespace test
{
    // ------------------------------------------------------------------------
    class test_error
    {
    public:
        // --------------------------------------------------------------------
        inline explicit test_error(const std::string & message)
            : _message (message)
        {
        }

        // --------------------------------------------------------------------
        inline const std::string & str() const
        {
            return _message;
        }

    private:
        std::string _message;
    };

    typedef void (* test_function_type)();

    // ------------------------------------------------------------------------
    template <typename TFunction>
    class basic_test_case
    {
    public:
        typedef TFunction function_type;

        // --------------------------------------------------------------------
        inline virtual ~basic_test_case()
        {
        }

        // --------------------------------------------------------------------
        basic_test_case(
                const function_type function,
                char const * const  name,
                char const * const  file,
                const int           line)
            : _function (function)
            , _name     (name)
            , _file     (file)
            , _line     (line)
        {
        }

        // --------------------------------------------------------------------
        void execute(const bool is_verbose, int & passed)
        {
            try
            {
                if (is_verbose)
                    std::cout << _file << "(" << _line << "): " << _name;

                _function();

                if (is_verbose)
                    std::cout << " -- PASS\n";

                passed++;
            }
            catch (const test_error & e)
            {
                if (is_verbose)
                    std::cout << " -- FAIL\n";

                std::cout
                    << _file << "(" << _line << "): "
                    << e.str() << "\n";
            }
            catch (const std::exception & e)
            {
                if (is_verbose)
                    std::cout << " -- FAIL\n";

                std::cout
                    << _file << "(" << _line << "): "
                    << "unexpected exception [" << e.what() << "]\n";
            }
        }

    private:
        function_type _function;
        std::string   _name;
        std::string   _file;
        int           _line;
    };

    typedef basic_test_case<test_function_type> test_case;

    // ------------------------------------------------------------------------
    template <typename TFunction>
    class basic_test_group
    {
    public:
        typedef basic_test_case<TFunction> test_case_type;
        typedef std::initializer_list<test_case_type> initializer_list_type;

        // --------------------------------------------------------------------
        explicit basic_test_group(
                const initializer_list_type & test_cases)
            : _test_cases (test_cases)
        {
        }

        // --------------------------------------------------------------------
        void execute(const bool is_verbose, int & executed, int & passed)
        {
            for (auto x : _test_cases)
            {
                executed++;
                x.execute(is_verbose, passed);
            }
        }

    private:
        typedef std::vector<test_case_type> vector_type;

        vector_type _test_cases;
    };

    typedef basic_test_group<test_function_type> test_group;

    // ------------------------------------------------------------------------
    template <typename TFunction>
    int execute(
        const int argc,
        const char * argv[],
        const std::initializer_list< basic_test_group<TFunction> > & test_groups)
    {
        auto is_verbose = false;
        for (auto i = 1; i < argc; i++)
            if (0 == strcmp("--verbose", argv[i]))
                is_verbose = true;

        auto executed = 0;
        auto passed = 0;

        for (auto group : test_groups)
            group.execute(is_verbose, executed, passed);

        const auto failed = executed - passed;

        std::cout
            << "\ntests executed: " << executed
            << "\ntests passed:   " << passed
            << "\ntests failed:   " << failed
            << "\n";

        return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    // ------------------------------------------------------------------------
    inline void _fail(
        char const * const message,
        char const * const file,
        const int          line)
    {
        std::ostringstream out;
        out << file << "(" << line << "): " << message;
        throw test_error(out.str());
    }

    // ------------------------------------------------------------------------
    template <typename TValue>
    void _almost(
        const TValue       expected,
        const TValue       actual,
        const TValue       epsilon,
        char const * const file,
        const int          line)
    {
        if (std::abs(expected - actual) > epsilon)
        {
            std::ostringstream out;
            out << "expected '" << expected
                << "' but encountered '" << actual << "'"
                << " using epsilon '" << epsilon << "'";
            _fail(out.str().c_str(), file, line);
        }
    }

    // ------------------------------------------------------------------------
    template <typename TValue>
    void _equal(
        const TValue       expected,
        const TValue       actual,
        char const * const file,
        const int          line)
    {
        if (expected != actual)
        {
            std::ostringstream out;
            out << "expected '" << expected
                << "' but encountered '" << actual << "'";
            _fail(out.str().c_str(), file, line);
        }
    }

    // ------------------------------------------------------------------------
    inline void _false(
        const bool         expression,
        char const * const message,
        char const * const file,
        const int          line)
    {
        if (expression)
            _fail(message, file, line);
    }

    // ------------------------------------------------------------------------
    inline void _true(
        const bool         expression,
        char const * const message,
        char const * const file,
        const int          line)
    {
        if (!expression)
            _fail(message, file, line);
    }
}

#define TEST_ALMOST(EXPECTED, ACTUAL, EPSILON) \
    test::_almost(EXPECTED, ACTUAL, EPSILON, __FILE__, __LINE__)

#define TEST_EQUAL(EXPECTED, ACTUAL) \
    test::_equal(EXPECTED, ACTUAL, __FILE__, __LINE__)

#define TEST_FAIL(MESSAGE) \
    test::_fail(MESSAGE, __FILE__, __LINE__)

#define TEST_FALSE(EXPRESSION) \
    test::_false(EXPRESSION, #EXPRESSION, __FILE__, __LINE__)

#define TEST_TRUE(EXPRESSION) \
    test::_true(EXPRESSION, #EXPRESSION, __FILE__, __LINE__)

#define TEST_THROWS(STATEMENT) \
    { \
        bool is_thrown = false; \
        try { \
            STATEMENT; \
        } catch (const std::exception &) { \
            is_thrown = true; \
        } \
        if (!is_thrown) { \
            std::ostringstream out; \
            out << "expected exception when executing '"; \
            out << #STATEMENT << "'"; \
            test::_fail(out.str().c_str(), __FILE__, __LINE__); \
        } \
    }

#define TEST_CASE(E) test::test_case(E, #E, __FILE__, __LINE__)

#endif // TEST_HPP__
