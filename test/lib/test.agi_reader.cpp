/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.agi_reader.hpp"

namespace
{
    typedef double                              float_type;
    typedef jade::basic_agi_reader<float_type>  agi_reader_type;
    typedef agi_reader_type::shunting_yard_type shunting_yard_type;
    typedef shunting_yard_type::args_type       args_type;

    static const float_type epsilon = 1.0e-6;
}

namespace
{
    // ------------------------------------------------------------------------
    void main()
    {
        std::istringstream in (R"(
# Branch length parameters, range: [0, inf)
a b c d e f g

# Admixture proportion parameters, range: [0, 1]
p

# K value
3

# Matrix entries, total number should be: K*(K-1)/2
# They map to a C matrix, e.g. K=3 it maps to:
#   0 1
#   1 2
(1 - p) * (b + e + g + f + a) + p * (b + d + a)
p * a + (1 - p) * (g + f + a)
c + g + f + a
)");
        const agi_reader_type reader (in);

        TEST_EQUAL(size_t(7), reader.get_branch_names().size());
        TEST_EQUAL(std::string("a"), reader.get_branch_names().at(0));
        TEST_EQUAL(std::string("b"), reader.get_branch_names().at(1));
        TEST_EQUAL(std::string("c"), reader.get_branch_names().at(2));
        TEST_EQUAL(std::string("d"), reader.get_branch_names().at(3));
        TEST_EQUAL(std::string("e"), reader.get_branch_names().at(4));
        TEST_EQUAL(std::string("f"), reader.get_branch_names().at(5));
        TEST_EQUAL(std::string("g"), reader.get_branch_names().at(6));

        TEST_EQUAL(size_t(1), reader.get_proportion_names().size());
        TEST_EQUAL(std::string("p"), reader.get_proportion_names().at(0));

        TEST_EQUAL(size_t(3), reader.get_k());

        TEST_EQUAL(size_t(3), reader.get_entries().size());

        const args_type args = {
            { "a", 1.0 },
            { "b", 2.0 },
            { "c", 3.0 },
            { "d", 4.0 },
            { "e", 5.0 },
            { "f", 6.0 },
            { "g", 7.0 },
            { "p", 0.5 },
        };

        const auto & e = reader.get_entries();

        // e[0] = (1 - p) * (b + e + g + f + a) + p * (b + d + a)
        //      = (1 - 0.5) * (2 + 5 + 7 + 6 + 1) + 0.5 * (2 + 4 + 1)
        //      = 14
        TEST_ALMOST(float_type(14.0), e[0].evaluate(args), epsilon);

        // e[1] = p * a + (1 - p) * (g + f + a)
        //      = 0.5 * 1 + (1 - 0.5) * (7 + 6 + 1)
        //      = 7.5
        TEST_ALMOST(float_type(7.5), e[1].evaluate(args), epsilon);

        // e[2] = c + g + f + a
        //      = 3 + 7 + 6 + 1
        //      = 17
        TEST_ALMOST(float_type(17.0), e[2].evaluate(args), epsilon);
    }
}

namespace test
{
    test_group agi_reader {
        TEST_CASE(main)
    };
}
