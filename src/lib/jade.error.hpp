/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_ERROR_HPP__
#define JADE_ERROR_HPP__

#include "jade.assert.hpp"

namespace jade
{
    ///
    /// A template for a class representing an exception thrown from this
    /// namespace.
    ///
    template <typename TChar>
    class basic_error : public std::exception
    {
    public:
        /// The character type.
        typedef TChar char_type;

        /// The string type.
        typedef std::basic_string<char_type> string_type;

        ///
        /// Initializes a new instance of the class with no message.
        ///
        inline basic_error()
            : _m ()
        {
        }

        ///
        /// Initializes a new instance of the class with the specified message.
        ///
        inline explicit basic_error(
                const string_type & message) ///< The initial message.
            : _m (message)
        {
        }

        ///
        /// Initializes a new instance of the class with the specified message.
        ///
        inline explicit basic_error(
                char_type const * const message) ///< The initial message.
            : _m (message)
        {
            assert(message != nullptr);
        }

        ///
        /// \return The message.
        ///
        inline const string_type & str() const
        {
            return _m;
        }

        ///
        /// \return An explanatory string.
        ///
        inline virtual const char * what() const throw();

        ///
        /// Concatenates the specified value with the current message.
        ///
        /// \param rhs The value to concatenate.
        /// \return    This instance.
        ///
        template <typename TArgValue>
        basic_error & operator << (TArgValue rhs)
        {
            std::basic_ostringstream<char_type> out;
            out << rhs;
            _m += out.str();
            return *this;
        }

    private:
        string_type _m;
    };

    #ifndef DOXYGEN_IGNORE

    // ------------------------------------------------------------------------
    template <>
    inline const char * basic_error<char>::what() const throw()
    {
        return _m.c_str();
    }

    #endif // DOXYGEN_IGNORE

    /// A class representing an exception thrown from this namespace.
    typedef basic_error<char> error;
}

#endif // JADE_ERROR_HPP__
