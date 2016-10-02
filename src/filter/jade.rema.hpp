/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_REMA_HPP__
#define JADE_REMA_HPP__

#include "jade.args.hpp"
#include "jade.matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that reduces columns of matrices given to it.
    /// The columns to keep are chosen at random, but their relative order is
    /// not changed.
    ///
    template <typename TChar>
    class basic_rema
    {
    public:
        typedef TChar                         char_type; ///< A character.
        typedef std::basic_istream<char_type> istream;   ///< An input stream.
        typedef std::basic_ostream<char_type> ostream;   ///< An output stream.

        ///
        /// Initializes a new instance of the class, reading the command-line
        /// arguments.
        ///
        explicit basic_rema(
                jade::args & a) ///< The command-line arguments.
            : _engine      ()
            , _seed        (_read_seed(a))
            , _num_markers (_read_num_markers(a))
        {
        }

        ///
        /// Executes the filter through the specified streams.
        ///
        void execute(
                istream & in,  ///< The input stream.
                ostream & out) ///< The output stream.
        {
            //
            // Read the dimensions of the matrix; this must be successful for
            // the first matrix.
            //
            size_t row_count, col_count;
            if (!(in >> row_count >> col_count))
                throw jade::error("error reading matrix dimensions");

            //
            // Do not try to keep more markers than the number of columns in
            // the input matrix.
            //
            const auto marker_count = std::min(col_count, _num_markers);

            //
            // Using the appropriate seed, randomly shuffle a vector that
            // contains all the possible indices of the matrix; then record a
            // set of bits to mark the columns to keep.
            //
            std::vector<bool> filter_flags;
            filter_flags.resize(col_count, true);
            {
                typedef std::vector<size_t> vector_type;
                vector_type indices (col_count);
                std::iota(indices.begin(), indices.end(), 0);
                _engine.seed(_seed);
                std::shuffle(indices.begin(), indices.end(), _engine);
                indices.resize(marker_count);
                for (const auto index : indices)
                    filter_flags[index] = false;
            }

            //
            // Filter the first matrix.
            //
            _filter(in, out, row_count, col_count, marker_count, filter_flags);

            //
            // Attempt to read a row; if this fails, assume this is the end of
            // the data (a discrete genotype matrix), but otherwise, require
            // the column, require consistent dimensions, and then read the
            // second matrix.
            //
            size_t r, c;
            if (!(in >> r) && in.eof())
                return;
            if (!(in >> c))
                throw jade::error("error reading second matrix dimensions");
            if (r != row_count || c != col_count)
                throw jade::error("inconsistent second matrix dimensions");

            out << char_type('\n');
            _filter(in, out, row_count, col_count, marker_count, filter_flags);

            //
            // Require the third matrix of a consistent size.
            //
            if (!(in >> r >> c))
                throw jade::error("error reading third matrix dimensions");
            if (r != row_count || c != col_count)
                throw jade::error("inconsistent third matrix dimensions");

            out << char_type('\n');
            _filter(in, out, row_count, col_count, marker_count, filter_flags);

            //
            // Require the end of the data.
            //
            for (;;)
            {
                const auto ch = in.get();
                if (ch < 0)
                    return;
                if (!std::isspace(ch))
                    throw jade::error("unexpected symbol after matrix data");
            }
        }

    private:
        ///
        /// The random number engine.
        ///
        typedef std::default_random_engine engine_type;

        ///
        /// The random number generator seed type.
        ///
        typedef engine_type::result_type seed_type;

        // --------------------------------------------------------------------
        void _filter(
                istream &                 in,
                ostream &                 out,
                const size_t              row_count,
                const size_t              col_count,
                const size_t              marker_count,
                const std::vector<bool> & filter_flags)
        {
            out << row_count    << char_type(' ')
                << marker_count << char_type('\n');

            //
            // Loop over the rows and columns of the input matrix.
            //
            for (size_t r = 0; r < row_count; r++)
            {
                size_t m = 0;
                for (size_t c = 0; c < col_count; c++)
                {
                    const auto is_unfiltered = !filter_flags[c];

                    //
                    // Skip whitespace; do not allow an end of stream error.
                    //
                    int ch;
                    do
                        if ((ch = in.get()) < 0)
                            throw jade::error("unexpected end of matrix data");
                    while (std::isspace(ch));

                    //
                    // Read all characters for the column, and write them to
                    // the output stream if this column is not discarded; allow
                    // an end of stream error in case the source stream does
                    // not end with an end-of-line character.
                    //
                    do
                        if (is_unfiltered)
                            out.put(static_cast<char_type>(ch));
                    while ((ch = in.get()) >= 0 && !std::isspace(ch));

                    //
                    // Add white-space or the end-of-line after each unfiltered
                    // marker written to the output stream.
                    //
                    if (is_unfiltered)
                        out << (++m == _num_markers
                            ? char_type('\n')
                            : char_type('\t'));
                }
            }
        }

        ///
        /// Returns the number of markers.
        /// \return The number of markers.
        ///
        static size_t _read_num_markers(
                jade::args & a) ///< The command-line arguments.
        {
            auto n = a.pop<size_t>();
            if (n == 0)
                throw error("invalid number of markers");
            return n;
        }

        ///
        /// Returns the random number seed.
        /// \return The random number seed.
        ///
        static seed_type _read_seed(
                jade::args & a) ///< The command-line arguments.
        {
            return a.read("--seed", "-s", std::random_device()());
        }

        engine_type _engine;      ///< The random number engine.
        seed_type   _seed;        ///< The seed.
        size_t      _num_markers; ///< The number of markers to keep.
    };

    typedef basic_rema<char> rema;
}

#endif // JADE_REMA_HPP__
