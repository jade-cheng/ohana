/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_PED2DGM_HPP__
#define JADE_PED2DGM_HPP__

#include "jade.ped_reader.hpp"

namespace jade
{
    ///
    /// A template for a class that converts PED-formatted data to discrete
    /// genotype matrices.
    ///
    template <typename TValue>
    class basic_ped2dgm
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The PED reader type.
        typedef jade::basic_ped_reader<value_type> reader_type;

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

#endif // JADE_PED2DGM_HPP__
