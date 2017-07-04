/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.neighbor_joining.hpp"

namespace
{
    typedef double value_type;
    typedef jade::basic_neighbor_joining<value_type> neighbor_joining_type;
    typedef jade::basic_matrix<value_type> matrix_type;

    // ------------------------------------------------------------------------
    void constructor()
    {
        const matrix_type distances {
            { 0,  5,  9,  9, 8 },
            { 5,  0, 10, 10, 9 },
            { 9, 10,  0,  8, 7 },
            { 9, 10,  8,  0, 3 },
            { 8,  9,  7,  3, 0 }
        };

        const neighbor_joining_type neighbor_joining (distances);

        TEST_EQUAL(
            std::string("(3:2,(2:4,(1:3,0:2):3):2,4:1);"),
            neighbor_joining.str());
    }
}

namespace test
{
    test_group neighbor_joining {
        TEST_CASE(constructor)
    };
}
