/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SCANNER_HPP__
#define JADE_SCANNER_HPP__

#include "jade.error.hpp"

namespace jade
{
    ///
    /// A template class that parses text and throws meaningful error messages.
    ///
    template <typename TChar>
    class basic_scanner
    {
    public:
        /// The character type.
        typedef TChar char_type;

        /// The character traits type.
        typedef std::char_traits<char_type> char_traits_type;

        /// The input stream type.
        typedef std::basic_istream<char_type> istream_type;

        /// The output stream type.
        typedef std::basic_ostream<char_type> ostream_type;

        /// The string type.
        typedef std::basic_string<char_type> string_type;

        /// The input stream type.
        typedef std::basic_istringstream<char_type> istringstream_type;

        /// The output stream type.
        typedef std::basic_ostringstream<char_type> ostringstream_type;

        ///
        /// Initializes a new instance of the class to scan the specified
        /// stream.
        ///
        explicit basic_scanner(
                istream_type & in) ///< The input stream.
            : _ptr ()
            , _in  (in)
        {
        }

        ///
        /// Initializes a new instance of the class to scan the specified
        /// string.
        ///
        explicit basic_scanner(
                const string_type & in) ///< The input string.
            : _ptr (new istringstream_type(in))
            , _in  (*_ptr)
        {
        }

        ///
        /// Skips whitespace and validates the next symbol in the stream matches
        /// the specified character. If the validation fails, this method throws
        /// an exception with a meaningful error message.
        ///
        /// \throw jade::error Thrown if the stream provides an unexpected
        /// symbol.
        ///
        void expect(
                const char_type ch) ///< The expected character.
        {
            //
            // Skip whitespace before validating the character.
            //
            skip_whitespace();

            //
            // Throw an excepiton if encountering the end of the stream.
            //
            const auto actual = _in.peek();
            if (actual < 0)
                throw error()
                    << "expected symbol '" << char_type(ch) << "' "
                    << "but encountered end of stream";

            //
            // Validate the symbol matches the expected value.
            //
            if (actual == ch)
            {
                _in.get();
                return;
            }

            if (::isprint(ch))
                throw error()
                    << "expected symbol '" << char_type(ch) << "' "
                    << "but encountered symbol '"
                    << char_type(actual) << "'";

            throw error()
                << "expected symbol '" << char_type(ch) << "' "
                << "but encountered ASCII code " << actual;
        }

        ///
        /// \return True if all data has been read from the scanner.
        ///
        inline bool is_end_of_data() const
        {
            return _in.peek() < 0;
        }

        ///
        /// Reads a series of digits from the stream and copies them to the
        /// specified output stream. This method does not skip whitespace before
        /// or after reading the digits.
        ///
        void read_digits(
                ostream_type & out) ///< The output stream.
        {
            while (::isdigit(_in.peek()))
                out << char_type(_in.get());
        }

        ///
        /// Skips whitespace and then parses and returns a floating-point
        /// value from the specified stream. If there is an error parsing the
        /// value, the method throws an exception with a meaningful error
        /// message.
        ///
        /// \return A floating-point value.
        ///
        /// \throw jade::error Thrown if there is an error parsing the length.
        ///
        inline double read_double()
        {
            return read_real<double>();
        }

        ///
        /// Skips whitespace and then parses and returns a floating-point
        /// value from the specified stream. If there is an error parsing the
        /// value, the method throws an exception with a meaningful error
        /// message.
        ///
        /// \return A floating-point value.
        ///
        /// \throw jade::error Thrown if there is an error parsing the length.
        ///
        inline float read_float()
        {
            return read_real<float>();
        }

        ///
        /// Skips whitespace and then parses and returns a floating-point
        /// value from the specified stream. If there is an error parsing the
        /// value, the method throws an exception with a meaningful error
        /// message.
        ///
        /// \return A floating-point value.
        ///
        /// \throw jade::error Thrown if there is an error parsing the length.
        ///
        template <typename TValue>
        TValue read_real()
        {
            static const auto hyphen = char_type('-');
            static const auto period = char_type('.');

            //
            // Initially skip whitespace.
            //
            skip_whitespace();

            ostringstream_type out;

            //
            // Check for negative values.
            //
            if (try_char(hyphen))
                out << hyphen;

            //
            // Read values before the decimal place.
            //
            read_digits(out);

            //
            // Check for a decimal, possibly reading additional digits.
            //
            if (try_char(period))
            {
                out << period;
                read_digits(out);
            }

            //
            // If no symbols exist in the output buffer, this is invalid.
            //
            const auto length_str = out.str();
            if (length_str.empty())
                throw error(
                    "expected a floating-point value but "
                    "did not encounter any digits");

            //
            // Parse and return the digits as a floating-point value, throwing
            // an exception in the case of an error.
            //
            istringstream_type length_in (length_str);
            TValue length;
            if (length_in >> length)
                return length;
            throw error()
                << "expected a length but encountered '"
                << length_str << "'";
        }

        ///
        /// Reads and returns a series of characters terminated by the end of
        /// the input stream or a specified delimeter. If no delimeters are
        /// unspecified, then the method uses whitespace as the delimeters.
        ///
        /// \param delimeters The delimeters, or nullptr.
        /// \return           The token.
        ///
        string_type read_token(char_type const * const delimeters = nullptr)
        {
            static const char_type fallback[] =
            {
                char_type(' '),
                char_type('\n'),
                char_type('\t'),
                char_type('\0')
            };

            //
            // Use whitespace as the delimeters unless specified otherwise.
            //
            const auto delims = delimeters == nullptr ? fallback : delimeters;
            const auto length = char_traits_type::length(delims);

            ostringstream_type out;

            for (;;)
            {
                auto ch = _in.peek();

                if (ch < 0 || nullptr != char_traits_type::find(
                    delims, length, char_type(ch)))
                    return out.str();

                out << char_type(_in.get());
            }
        }

        ///
        /// Skips whitespace from the input stream. If the input stream
        /// indicates the end of the stream, the method returns without an
        /// error.
        ///
        void skip_whitespace()
        {
            while (::isspace(_in.peek()))
                _in.get();
        }

        ///
        /// Skips whitespace and then checks if the next character from the
        /// stream matches the specified character. If it does, the method
        /// advances the stream past the character and returns true; otherwise,
        /// the method returns false.
        ///
        /// \return True if encountering the character; otherwise, false.
        ///
        bool try_char(
                const char_type ch) ///< The character to test.
        {
            //
            // Skip whitespace before testing the next symbol.
            //
            skip_whitespace();

            //
            // Return false if the symbol does not match or if there are no more
            // symbols available in the input stream.
            //
            if (ch != _in.peek())
                return false;

            //
            // Advance past the character and return true to indicate a match.
            //
            _in.get();
            return true;
        }

    private:
        basic_scanner(const basic_scanner &) = delete;
        basic_scanner & operator = (const basic_scanner &) = delete;

        std::unique_ptr<istream_type> _ptr;
        istream_type &                _in;
    };

    /// A class that parses text and throws meaningful error messages.
    typedef basic_scanner<char> scanner;
}

#endif // JADE_SCANNER_HPP__
