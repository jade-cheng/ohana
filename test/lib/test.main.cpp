/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"

// ----------------------------------------------------------------------------
int main(const int argc, const char * argv[])
{
    return test::execute(argc, argv, {
        test::agi_reader,
        test::args,
        test::discrete_genotype_matrix,
        test::error,
        test::lemke,
        test::likelihood_genotype_matrix,
        test::matrix,
        test::neighbor_joining,
        test::newick,
        test::scanner,
        test::shunting_yard,
        test::simplex,
        test::stopwatch,
        test::svg_tree,
        test::vec2
    });
}
