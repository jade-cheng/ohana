/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_PED_READER_HPP__
#define JADE_PED_READER_HPP__

#include "jade.error.hpp"

namespace jade
{
    ///
    /// A template for a class that reads PED data.
    ///
    template <typename TValue>
    class basic_ped_reader
    {
    public:
        /// The value type.
        typedef TValue value_type;

        ///
        /// Initializes a new instance of the class.
        ///
        explicit basic_ped_reader(
                std::istream & in) ///< The input stream.
            : _buf  ()
            , _rows (0)
            , _cols (0)
        {
            size_t line = 1;
            size_t col  = 0;
            for (;;)
            {
                const auto ch = in.get();

                if (ch < 0)
                {
                    if (col > 0)
                        throw error("unexpected end of data");
                    break;
                }

                if (ch == '\n')
                {
                    if (col > 0)
                        throw error() << "premature end of line " << line;
                    line++;
                    continue;
                }

                if (ch != '\t')
                    continue;

                if (++col < 6)
                    continue;

                col = 1;
                for (;;)
                {
                    const auto ch1 = _read(in, line);

                    _require(in, line, ' ');

                    const auto ch2 = _read(in, line);

                    const auto symbol =
                        ch1 == '0' || ch2 == '0' ? '3' :
                        ch1 != ch2 ? '1' :
                        ch1 == '2' ? '0' :
                        '2';

                    _buf.push_back(symbol);

                    if (in.peek() < 0 || in.peek() == '\n')
                        break;

                    _require(in, line, '\t');

                    if (_rows > 0 && col == _cols)
                        throw error()
                            << "expected " << _cols << " pairs but encountered "
                            << "at least " << col + 1 << " on line " << line;

                    col++;
                }

                if (_rows == 0)
                    _cols = col;

                else if (col != _cols)
                    throw error()
                        << "expected " << _cols << " genotype pairs but "
                        << "encountered " << col << " on line " << line;

                col = 0;
                _rows++;
            }

            if (_cols == 0)
                _rows = 0;
        }

        ///
        /// \return A string representation of this instance.
        ///
        std::string str() const
        {
            std::ostringstream out;
            write(out);
            return out.str();
        }

        ///
        /// Writes the PED data to the specified output stream.
        ///
        void write(
            std::ostream & out) ///< The output stream.
            const
        {
            out << _rows << ' ' << _cols << std::endl;

            if (_cols == 0)
                return;

            size_t i = 0;
            for (const auto ch : _buf)
                out << ch << (++i % _cols == 0 ? '\n' : ' ');
        }

        ///
        /// Writes the PED data to the specified output file.
        ///
        void write(
            char const * const path) ///< The output path.
        {
            assert(path != nullptr);
            std::ofstream out (path);
            if (!out.good())
                throw error() << "error opening '" << path << "' for writing";
            write(out);
        }

        ///
        /// Writes the PED data to the specified output file.
        ///
        inline void write(
            const std::string & path) ///< The output path.
        {
            write(path.c_str());
        }

    private:
        // --------------------------------------------------------------------
        static std::string _format(const int ch)
        {
            std::ostringstream str;

            if (ch < 0)
                str << "end of data";
            else if (ch == '\'') str << "'\\''";
            else if (ch == '\n') str << "'\\n'";
            else if (ch == '\r') str << "'\\r'";
            else if (ch == '\t') str << "'\\t'";
            else if (0 != std::isprint(ch))
                str << "'" << char(ch) << "'";
            else
                str << "0x" << std::setfill('0') << std::hex
                    << std::setw(2) << ch;

            return str.str();
        }

        // --------------------------------------------------------------------
        static int _read(std::istream & in, const size_t line)
        {
            const auto ch = in.get();

            if (ch != '0' && ch != '1' && ch != '2')
                throw error()
                    << "expected genotype '0', '1', or '2' but encountered "
                    << _format(ch) << " on line " << line << "; correct this "
                    << "by regenerating your data using the -recode12 option "
                    << "from plink";

            return ch;
        }

        // --------------------------------------------------------------------
        static void _require(
                std::istream & in,
                const size_t   line,
                const int      expected)
        {
            const auto actual = in.get();
            if (actual == expected)
                return;

            throw error()
                << "expected " << _format(expected) << " but encountered "
                << _format(actual) << " on line " << line;
        }

        std::vector<char> _buf;
        size_t            _rows;
        size_t            _cols;
    };
}

#endif // JADE_PED_READER_HPP__
