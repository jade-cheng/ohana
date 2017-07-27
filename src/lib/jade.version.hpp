/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_VERSION_HPP__
#define JADE_VERSION_HPP__

#include "jade.system.hpp"

namespace jade
{
    ///
    /// A template for a class that displays version and build information.
    ///
    template <typename TChar>
    class basic_version {
    public:
        /// The character type.
        typedef TChar char_type;

        /// The output stream type.
        typedef std::basic_ostream<char_type> ostream_type;

        ///
        /// Writes the string displayed to the user.
        ///
        static void write(
                char_type const * const title, ///< The program name.
                ostream_type &          out)   ///< The output stream.
        {
            static const values current { 0, 0, 6416, 41394, 2015, 2017 };

            out << "ohana/" << title << ' '
                << current.major << '.'
                << current.minor << '.'
                << current.build << '.'
                << current.revision << std::endl
                << "Copyright (c) " << current.year_min << "-"
                << current.year_max << " Jade Cheng" << std::endl
                << "Jade Cheng <info@jade-cheng.com>" << std::endl;
        }

    private:
        // --------------------------------------------------------------------
        struct values
        {
            int major;
            int minor;
            int build;
            int revision;
            int year_min;
            int year_max;
        };
    };

    ///
    /// A class that displays version and build information.
    ///
    typedef basic_version<char> version;
}

#endif // JADE_VERSION_HPP__
