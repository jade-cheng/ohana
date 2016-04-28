/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_ARGS_HPP__
#define JADE_ARGS_HPP__

#include "jade.error.hpp"

namespace jade
{
    ///
    /// A template for a class that helps process command-line arguments.
    ///
    template <typename TChar>
    class basic_args
    {
    public:
        typedef TChar char_type; ///< The character type.

        ///
        /// Initializes a new instance of the class based on the specified
        /// arguments. The first argument is ignored.
        ///
        /// \param values The command line arguments.
        ///
        template <typename TValue>
        explicit basic_args(const std::initializer_list<TValue> & values)
            : _m ()
            #ifndef NDEBUG
            , is_flag_read (false)
            #endif // NDEBUG
        {
            auto iter = values.begin();

            if (iter == values.end())
                throw error("invalid command-line arguments");

            while (++iter != values.end())
            {
                if (*iter == nullptr)
                    throw error("invalid command-line arguments");

                _m.emplace_back(*iter);
            }
        }

        ///
        /// Initializes a new instance of the class based on the specified
        /// arguments. The first argument is ignored.
        ///
        basic_args(
                const int         argc,   ///< The argument count.
                const char_type * argv[]) ///< The argument values.
            : _m ()
            #ifndef NDEBUG
            , is_flag_read (false)
            #endif // NDEBUG
        {
            if (argc < 1 || argv == nullptr)
                throw error("invalid command-line arguments");

            for (auto i = 1; i < argc; i++)
            {
                const auto s = argv[i];
                if (s == nullptr)
                    throw error("invalid command-line arguments");

                _m.emplace_back(s);
            }
        }

        ///
        /// \return The number of arguments remaining to be processed.
        ///
        inline size_t get_length() const
        {
            return _m.size();
        }

        ///
        /// \return True if there are no more arguments to be processed.
        ///
        inline bool is_empty() const
        {
            return _m.empty();
        }

        ///
        /// \return The next argument from the stack or throws an exception if
        /// there are no more arguments to be processed.
        ///
        template <typename TValue>
        TValue pop()
        {
            if (_m.empty())
                throw error("not enough arguments");

            const auto value = _parse<TValue>(0);
            _m.erase(_m.begin());
            return value;
        }

        ///
        /// Reads and removes an option with one argument. If the option was
        /// not specified, then the fallback value is returned.
        ///
        /// \param long_name  The long option name.
        /// \param short_name The short option name.
        /// \param fallback   The optional fallback value.
        ///
        /// \return           The value after the flag or the fallback value.
        ///
        template <typename TValue>
        TValue read(
            char_type const * const long_name,
            char_type const * const short_name,
            const TValue            fallback = TValue())
        {
            assert(!is_flag_read);

            const auto index = _find(long_name, short_name);
            if (index < 0)
                return fallback;

            if (index + 1 >= int(get_length()))
                throw error() << "missing argument for option " << long_name;

            const auto value = _parse<TValue>(size_t(index + 1));
            _m.erase(_m.begin() + index, _m.begin() + index + 2);
            return value;
        }

        ///
        /// Reads and removes an option with no arguments. If the options was
        /// not specified, then false is returned.
        ///
        /// \return True if the flag is found; otherwise, false.
        ///
        bool read_flag(
            char_type const * const long_name,  ///< The long option name.
            char_type const * const short_name) ///< The short option name.
        {
            const auto index = _find(long_name, short_name);
            if (index < 0)
                return false;

            #ifndef NDEBUG
            is_flag_read = true;
            #endif // NDEBUG

            _m.erase(_m.begin() + index);
            return true;
        }

        ///
        /// Throws an exception if there are more arguments to be processed.
        ///
        void validate_empty() const
        {
            if (_m.empty())
                return;

            if (_m[0][0] == '-')
                throw error() << "unexpected option " << _m[0];

            throw error() << "unexpected argument '" << _m[0] << "'";
        }

        ///
        /// Throws an exception if the number of remaining arguments is not the
        /// expected number.
        ///
        void validate_length(
                const size_t length) ///< The number of remaining arguments.
                const
        {
            if (_m.size() != length)
                throw error("invalid syntax; try --help");
        }

    private:
        typedef std::basic_string<char_type> string_type;
        typedef std::vector<string_type>     vector_type;

        vector_type _m;

        #ifndef NDEBUG
        bool is_flag_read;
        #endif // NDEBUG

        // --------------------------------------------------------------------
        int _find(
            char_type const * const long_name,
            char_type const * const short_name)
        {
            const auto n = get_length();

            for (size_t i = 0; i < n; i++)
            {
                if (!_match(long_name, short_name, i))
                    continue;

                for (size_t j = i + 1; j < n; j++)
                    if (_match(long_name, short_name, j))
                        throw error() << "duplicate option for " << long_name;

                return int(i);
            }

            return -1;
        }

        // --------------------------------------------------------------------
        bool _match(
            char_type const * const long_name,
            char_type const * const short_name,
            const size_t            index)
        {
            const auto & s = _m[index];
            return s == long_name || s == short_name;
        }

        // --------------------------------------------------------------------
        template <typename TValue>
        TValue _parse(const size_t index)
        {
            const auto & s = _m[index];

            typedef std::basic_istringstream<char_type> stream_type;
            stream_type in (s);

            TValue value;
            if (!(in >> value))
                throw error() << "invalid argument '" << s << "'";

            return value;
        }
    };

    ///
    /// A class that helps process command-line arguments.
    ///
    typedef basic_args<char> args;
}

#endif // JADE_ARGS_HPP__
