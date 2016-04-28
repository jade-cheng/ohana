/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.svg_tree.hpp"

namespace
{
    typedef double                              value_type;
    typedef jade::basic_svg_tree<value_type>    svg_tree_type;
    typedef jade::basic_newick_node<value_type> node_type;

    // ------------------------------------------------------------------------
    void constructor()
    {
        const node_type tree (
            "((((0:0.03):0.06,1:0.08,bears:0.03,monkeys:0.03):0.09,(4:0.05,"
            "(5:0.07):0.04):0.03):0.03,(pandas:0.04):0.09);");

        char const * const path = "tmp/test.svg";
        svg_tree_type svg (tree);
        std::ofstream out (path);
        svg.write(out);
        remove(path);
    }
}

namespace test
{
    test_group svg_tree {
        TEST_CASE(constructor)
    };
}
