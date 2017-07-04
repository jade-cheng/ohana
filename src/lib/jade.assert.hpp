/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_ASSERT_HPP__
#define JADE_ASSERT_HPP__

#include "jade.system.hpp"

#ifndef NDEBUG

#include <cxxabi.h>
#include <execinfo.h>

namespace jade
{
    ///
    /// A template for a class that asserts valid conditions.
    ///
    template <typename TChar>
    class basic_assertion
    {
    public:
        /// The character type.
        typedef TChar char_type;

        /// The string type.
        typedef std::basic_string<char_type> string_type;

        ///
        /// Validates an expression is true; if not, this method throws an
        /// exception.
        ///
        static void validate(
            const bool              is_true,    ///< The validated expression.
            char_type const * const expression, ///< The expression text.
            char_type const * const file,       ///< The name of the file.
            const int               line)       ///< The line number.
        {
            if (is_true)
                return;

            auto & err = _err();
            err << file << char_type('(') << line << char_type(')')
                << char_type(':') << char_type(' ') << expression
                << std::endl;

            void * addresses[256];
            const auto n = ::backtrace(
                addresses,
                std::extent<decltype(addresses)>::value);

            typedef std::unique_ptr<
                char_type *,
                decltype(&std::free)
            > symbols_ptr_type;

            const symbols_ptr_type symbols_ptr (
                _backtrace_symbols(addresses, n),
                &std::free);
            const auto symbols = symbols_ptr.get();

            for (auto i = 1; i < n; i++)
            {
                const auto symbol = symbols[i];

                //
                // Look for the last '+' symbol; if found subract white-space
                // off the end of the mangled function name.
                //
                auto end = symbol;
                while (*end != char_type('\0'))
                    ++end;
                while (end != symbol && *end != char_type('+'))
                    --end;
                while (end > symbol && end[-1] == char_type(' '))
                    --end;

                //
                // Loop back until the first white-space symbol or parenthesis
                // symbol is found.
                //
                auto begin = std::max(symbol, end - 1);
                while (
                        begin > symbol &&
                        begin[-1] != char_type(' ') &&
                        begin[-1] != char_type('('))
                    --begin;

                //
                // If the mangled name wasn't found, then print the mangled
                // version; otherwise, replace the manged name with the
                // demangled version and print the reformatted line.
                //
                if (begin <= symbol || end <= symbol)
                {
                    err << symbol << std::endl;
                }
                else
                {
                    for (auto p = symbol; p != begin; p++)
                        err << *p;
                    const auto replaced = *end;
                    *end++ = char_type('\0');

                    err << _demangle(begin) << replaced << end << std::endl;
                }
            }

            throw std::runtime_error("assertion failure");
        }

    private:
        // --------------------------------------------------------------------
        static char_type ** _backtrace_symbols(
            void * const * buffer,
            int            size);

        // --------------------------------------------------------------------
        static char_type * _cxa_demangle(
                const char_type * mangled_name,
                char_type *       output_buffer,
                size_t *          length,
                int *             status);

        // --------------------------------------------------------------------
        static string_type _demangle(char_type const * const symbol)
        {
            typedef std::unique_ptr<
                char_type,
                decltype(&std::free)
            > ptr_type;

            const ptr_type demangled (
                _cxa_demangle(symbol, nullptr, nullptr, nullptr),
                &std::free);

            return demangled ? demangled.get() : symbol;
        }

        // --------------------------------------------------------------------
        static std::basic_ostream<char_type> & _err();
    };

    // ------------------------------------------------------------------------
    template <>
    inline char ** basic_assertion<char>::_backtrace_symbols(
            void * const * buffer,
            int            size)
    {
        return ::backtrace_symbols(buffer, size);
    }

    // ------------------------------------------------------------------------
    template <>
    inline char * basic_assertion<char>::_cxa_demangle(
            const char_type * mangled_name,
            char_type *       output_buffer,
            size_t *          length,
            int *             status)
    {
        return abi::__cxa_demangle(
                mangled_name,
                output_buffer,
                length,
                status);
    }
    // ------------------------------------------------------------------------
    template <>
    inline std::ostream & basic_assertion<char>::_err()
    {
        return std::cerr;
    }

    ///
    /// A class that asserts valid conditions.
    ///
    typedef basic_assertion<char> assertion;
}

#define assert(E) jade::assertion::validate(E, #E, __FILE__, __LINE__)

#else // NDEBUG
#define assert(E)
#endif

#endif // JADE_ASSERT_HPP__
