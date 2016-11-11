/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_AGI_READER_HPP__
#define JADE_AGI_READER_HPP__

#include "jade.shunting_yard.hpp"

namespace jade
{
    ///
    /// A template for a class that reads an AGI (Admixture Graph Input) file.
    /// The file provides the following information: A set of branch length
    /// variable names, a set of admixture proportion parameter names, a
    /// population size (K), and K*(K-1)/2 number of expressions representing
    /// C matrix entries.
    ///
    /// An example of the file format is as follows:
    /// <pre>
    /// # Branch length parameters, range: [0, inf)
    /// a b c d e f g
    ///
    /// # Admixture proportion parameters, range: [0, 1]
    /// p
    ///
    /// # K value
    /// 3
    ///
    /// # Matrix entries, total number should be: K*(K-1)/2
    /// # They map to a C matrix, e.g. K=3 maps to:
    /// #   0 1
    /// #   1 2
    /// (1 - p) * (b + e + g + f + a) + p * (b + d + a)
    /// p * a + (1 - p) * (g + f + a)
    /// c + g + f + a
    /// </pre>
    ///
    template <typename TValue>
    class basic_agi_reader
    {
    public:
        ///
        /// The value type.
        ///
        typedef TValue value_type;

        ///
        /// The shunting yard algorithm used to parse and evaluate expressions.
        ///
        typedef jade::basic_shunting_yard<value_type> shunting_yard_type;

        ///
        /// The arguments used to evaluate the shunting yard algorithm.
        ///
        typedef typename shunting_yard_type::args_type args_type;

        ///
        /// Initializes a new instance of the class by reading and parsing the
        /// specified input stream.
        /// \param in The input stream.
        ///
        explicit basic_agi_reader(std::istream & in)
            : _args             ()
            , _branch_names     (_read_names(in))
            , _proportion_names (_read_names(in))
            , _k                (_read_size(in))
            , _entries          (_read_entries(in, _k))
        {
            std::string token;
            if (!!(in >> token))
                throw jade::error() << "unexpected token: " << token;

            //
            // Require that all variable names across both sets are unique. In
            // doing so, build a set of all defined variable names.
            //
            std::set<std::string> defined;
            const auto vectors = { &_branch_names, &_proportion_names };
            for (const auto vector : vectors)
                for (const auto & name : *vector)
                    if (!defined.insert(name).second)
                        throw jade::error()
                            << "duplicate variable name '" << name << "'";

            //
            // Require that all variables defined in the expressions are found
            // in the set of defined variable names.
            //
            for (const auto & entry : _entries)
                for (const auto & pair : entry.get_args())
                    if (defined.insert(pair.first).second)
                        throw jade::error()
                            << "undefined variable name '"
                            << pair.first << "' in expression";

            //
            // Create a set of the variables used in the expressions.
            //
            std::set<std::string> used;
            for (const auto & entry : _entries)
                for (const auto & pair : entry.get_args())
                    used.insert(pair.first);

            //
            // Remove unused variables from the branch and proportion sets.
            //
            {
                const auto remove = [&used](std::vector<std::string> & names)
                {
                    const auto end = std::remove_if(
                        names.begin(),
                        names.end(),
                        [&used](const std::string & name)
                        {
                            return used.find(name) == used.end();
                        });

                    names.erase(end, names.end());
                };

                remove(_branch_names);
                remove(_proportion_names);
            }

            //
            // Now that it is verified that all names are unique and that all
            // variables in the expressions are defined as either branch or
            // proportions, initialize the set of default arguments based on
            // the variables used in the expressions.
            //
            for (const auto & name : used)
                _args[name] = value_type(0);
        }

        ///
        /// Initializes a new instance of the class by reading and parsing the
        /// file with the specified path.
        /// \param path The path to the file to read and parse.
        ///
        explicit basic_agi_reader(const std::string & path)
            : _args             ()
            , _branch_names     ()
            , _proportion_names ()
            , _k                (0)
            , _entries          ()
        {
            try
            {
                std::ifstream in (path);
                *this = basic_agi_reader(in);
            }
            catch (const std::exception & e)
            {
                throw jade::error()
                        << "error reading admixture graph input from file '"
                        << path
                        << "': "
                        << e.what();
            }
        }

        ///
        /// Returns a table of arguments that maps variables to values. The
        /// table reference returned has all values set to zero. The table can
        /// be used to evaluate all expression entries.
        /// \return A table mapping variables to values.
        ///
        const args_type & get_args() const
        {
            return _args;
        }

        ///
        /// Returns the vector of branch names.
        /// \return The vector of branch names.
        ///
        const std::vector<std::string> & get_branch_names() const
        {
            return _branch_names;
        }

        ///
        /// Returns the vector of proportion names.
        /// \return The vector of proportion names.
        ///
        const std::vector<std::string> & get_proportion_names() const
        {
            return _proportion_names;
        }

        ///
        /// Returns the number of populations.
        /// \return The number of populations.
        ///
        std::size_t get_k() const
        {
            return _k;
        }

        ///
        /// Returns the vector of expressions for the C matrix entries.
        /// \return The vector of expressions for the C matrix entries.
        ///
        const std::vector<shunting_yard_type> & get_entries() const
        {
            return _entries;
        }

    private:
        // --------------------------------------------------------------------
        static std::vector<shunting_yard_type> _read_entries(
                std::istream & in,
                const size_t   k)
        {
            const auto n = k * (k - 1) / 2;
            std::vector<shunting_yard_type> entries;
            entries.reserve(n);

            for (size_t i = 0; i < n; i++)
                entries.emplace_back(_read_line(in));

            return entries;
        }

        // --------------------------------------------------------------------
        static std::string _read_line(std::istream & in)
        {
            for (;;)
            {
                std::string line;
                if (!std::getline(in, line))
                    throw jade::error("unexpected end of file");

                if (!line.empty() && line[0] != '#')
                    return line;
            }
        }

        // --------------------------------------------------------------------
        static std::vector<std::string> _read_names(std::istream & in)
        {
            std::vector<std::string> names;

            std::istringstream line (_read_line(in));

            for (;;)
            {
                std::string name;
                if (!(line >> name))
                    return names;

                names.push_back(_validate_name(name));
            }
        }

        // --------------------------------------------------------------------
        static size_t _read_size(std::istream & in)
        {
            const auto line = _read_line(in);
            std::istringstream line_in (line);

            size_t size;
            if (!(line_in >> size) || size < 2)
                throw jade::error("invalid size: ") << line;

            std::string token;
            if (!(line_in >> token))
                return size;

            throw jade::error() << "unexpected token: " << token;
        }

        // --------------------------------------------------------------------
        static const std::string & _validate_name(const std::string & name)
        {
            static const auto fn = [](const char ch)
            {
                return std::isalnum(ch) || ch == '_';
            };

            if (!name.empty() && std::isalpha(name[0]))
                if (std::all_of(name.begin() + 1, name.end(), fn))
                    return name;

            throw jade::error() << "invalid name: '" << name << "'";
        }

        args_type                       _args;
        std::vector<std::string>        _branch_names;
        std::vector<std::string>        _proportion_names;
        std::size_t                     _k;
        std::vector<shunting_yard_type> _entries;
    };
}

#endif // JADE_AGI_READER_HPP__
