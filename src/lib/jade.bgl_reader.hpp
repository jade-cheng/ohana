/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_BGL_READER_HPP__
#define JADE_BGL_READER_HPP__

#include "jade.error.hpp"

namespace jade
{
    ///
    /// A template for a class that reads BEAGLE data and is capable of writing
    /// it to a stream as a likelihood genotype matrix.
    ///
    template <typename TValue>
    class basic_bgl_reader
    {
    public:
        /// The value type.
        typedef TValue value_type;

        ///
        /// Initializes a new instance of the class.
        ///
        explicit basic_bgl_reader(
                std::istream & in) ///< The input stream.
            : _buf  ()
            , _rows (0)
            , _cols (0)
        {
            std::string temp;
            std::getline(in, temp);
            if (!in.good())
                throw error() << "failed to read header of beagle data.";

            std::istringstream header_in (temp);
            while (header_in >> temp)
                _rows++;
            if (_rows < 3 || _rows % 3 != 0)
                throw error() << "invalid number of columns "
                              << "in beagle header: " << _rows;

            _rows = (_rows - 3) / 3;

            for (;;)
            {
                std::string marker;
                if (!(in >> marker))
                    break;

                _cols++;

                if (!(in >> temp))
                    throw error()
                        << "cannot read first allele for marker '"
                        << marker << "' on line " << _cols + 1
                        << " of beagle data";

                if (!(in >> temp))
                    throw error()
                        << "cannot read second allele for marker '"
                        << marker << "' on line " << _cols + 1
                        << " of beagle data";

                for (size_t i = 0; i < _rows; i++)
                {
                    for (size_t j = 0; j < 3; j++)
                    {
                        value_type value;
                        if (!(in >> value) || value < value_type(0) ||
                                value > value_type(1))
                            throw error()
                                << "encountered invalid percentage in column "
                                << 3 + i * 3 + j << " for marker '" << marker
                                << "' on line " << _cols + 1
                                << " of beagle data";
                        _buf.push_back(value);
                    }
                }

                if (!in.good())
                    throw error() << "invalid data on line " << _cols + 1
                                  << " of beagle data";

                const auto ch = in.peek();
                if (ch >= 0 && ch != '\n')
                    throw error()
                        << "expected end of line but encountered "
                        << _format(ch) << " on line "
                        << _cols + 1 << " of beagle data";
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
        /// Writes the BEAGLE data to the specified output stream as a
        /// likelihood genotype matrix.
        ///
        void write(
            std::ostream & out) ///< The output stream.
            const
        {
            if (_cols == 0)
            {
                out << "0 0" << std::endl;
                return;
            }

            const auto r3  = _rows * 3;
            const auto cr3 = _cols * _rows * 3;

            auto       grp_ptr = _buf.data();
            const auto grp_end = grp_ptr + 3;
            for (;;)
            {
                out << _rows << ' ' << _cols << std::endl;

                auto       row_ptr = grp_ptr;
                const auto row_end = row_ptr + r3;
                while (row_ptr != row_end)
                {
                    auto       col_ptr = row_ptr;
                    const auto col_end = col_ptr + cr3;
                    for (;;)
                    {
                        out << *col_ptr;
                        col_ptr += r3;
                        if (col_ptr == col_end)
                            break;
                        out << '\t';
                    }

                    out << '\n';
                    row_ptr += 3;
                }

                if (++grp_ptr == grp_end)
                    break;
                out << std::endl;
            }
        }

        ///
        /// Writes the BEAGLE data to the specified output file.
        ///
        inline void write(
            char const * const path) ///< The output path.
        {
            assert(path != nullptr);
            std::ofstream out (path);
            if (!out.good())
                throw error() << "error opening '" << path << "' for writing";
            write(out);
        }

        ///
        /// Writes the BEAGLE data to the specified output file.
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

        std::vector<value_type> _buf;
        size_t                  _rows;
        size_t                  _cols;
    };
}

#endif // JADE_BGL_READER_HPP__
