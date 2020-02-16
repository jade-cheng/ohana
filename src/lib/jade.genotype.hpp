/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_GENOTYPE_HPP__
#define JADE_GENOTYPE_HPP__

#include "jade.system.hpp"

namespace jade
{
    ///
    /// A genotype, encoded as a one-byte integer value.
    ///
    typedef char genotype;

    ///
    /// The major-major genotype.
    ///
    static const genotype genotype_major_major = '0';

    ///
    /// The major-minor genotype.
    ///
    static const genotype genotype_major_minor = '1';

    ///
    /// The minor-minor genotype.
    ///
    static const genotype genotype_minor_minor = '2';

    ///
    /// An indication that a genotype is missing.
    ///
    static const genotype genotype_missing = '3';
}

#endif // JADE_GENOTYPE_HPP__
