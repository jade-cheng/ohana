/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_BGL2LGM_HPP__
#define JADE_BGL2LGM_HPP__

#include "jade.bgl_reader.hpp"

namespace jade
{
    ///
    /// A template for a class that converts BEAGLE-formatted data to likelihood
    /// genotype matrices.
    ///
    template <typename TValue>
    class basic_bgl2lgm
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The reader type.
        typedef jade::basic_bgl_reader<value_type> reader_type;

        ///
        /// Executes the program through the specified streams.
        ///
        static void execute(
                std::istream & in,  ///< The input stream.
                std::ostream & out) ///< The output stream.
        {
            reader_type reader (in);
            reader.write(out);
        }
    };
}

#endif // JADE_BGL2LGM_HPP__
