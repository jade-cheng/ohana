/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.matrix.hpp"

namespace
{
    typedef int int_type;
    typedef jade::basic_matrix<int_type> int_matrix;

    typedef double real_type;
    typedef jade::basic_matrix<real_type> real_matrix;

    const auto epsilon = real_type(0.0001);

    // ------------------------------------------------------------------------
    std::string str(const int_matrix::initializer_list_type & lhs)
    {
        int_matrix m (lhs);
        return m.str();
    }

    // ------------------------------------------------------------------------
    void clamp()
    {
        int_matrix m {
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        };

        m.clamp(2, 8);
        TEST_EQUAL(str({
            { 2, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 8 }
        }), m.str());

        m.clamp_column(0, 3, 6);
        TEST_EQUAL(str({
            { 3, 2, 3 },
            { 4, 5, 6 },
            { 6, 8, 8 }
        }), m.str());

        m.clamp_row(1, 5, 5);
        TEST_EQUAL(str({
            { 3, 2, 3 },
            { 5, 5, 5 },
            { 6, 8, 8 }
        }), m.str());
    }

    // ------------------------------------------------------------------------
    void column()
    {
        int_matrix m {
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        };

        TEST_EQUAL(str({
            { 2 },
            { 5 },
            { 8 }
        }), m.copy_column(1).str());

        m.multiply_column(0, 2);

        TEST_EQUAL(str({
            { 2, 2, 3 },
            { 8, 5, 6 },
            { 14, 8, 9 }
        }), m.str());

        const int_matrix q { { 1, 2, 3 } };
        TEST_EQUAL(int_type(60), m.multiply_column(0, q));
    }

    // ------------------------------------------------------------------------
    void constructor()
    {
        const int_matrix m1 {
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        };

        TEST_EQUAL(size_t(3), m1.get_height());
        TEST_EQUAL(size_t(3), m1.get_width());
        TEST_FALSE(m1.is_empty());
        TEST_EQUAL(size_t(9), m1.get_length());
        TEST_TRUE(m1.is_length(9));
        TEST_TRUE(m1.is_size(3, 3));
        TEST_FALSE(m1.is_column_vector());
        TEST_FALSE(m1.is_row_vector());
        TEST_FALSE(m1.is_vector());
        TEST_TRUE(m1.is_square());

        TEST_EQUAL(int_type(1), m1(0, 0));
        TEST_EQUAL(int_type(2), m1(0, 1));
        TEST_EQUAL(int_type(3), m1(0, 2));
        TEST_EQUAL(int_type(4), m1(1, 0));
        TEST_EQUAL(int_type(5), m1(1, 1));
        TEST_EQUAL(int_type(6), m1(1, 2));
        TEST_EQUAL(int_type(7), m1(2, 0));
        TEST_EQUAL(int_type(8), m1(2, 1));
        TEST_EQUAL(int_type(9), m1(2, 2));

        std::istringstream in ("3 3 1 2 3 4 5 6 7 8 9");
        const int_matrix m2 (in);
        TEST_EQUAL(m1.str(), m2.str());
    }

    // ------------------------------------------------------------------------
    void contains()
    {
        {
            const real_matrix m1 {
                { std::numeric_limits<real_type>::infinity(), 0, 0 },
                { 0, 0, 0 }
            };

            TEST_TRUE(m1.contains_inf());
            TEST_FALSE(m1.contains_nan());
        }

        {
            const real_matrix m2 {
                { std::numeric_limits<real_type>::quiet_NaN(), 0, 0 },
                { 0, 0, 0 }
            };

            TEST_TRUE(m2.contains_nan());
            TEST_FALSE(m2.contains_inf());
        }
    }

    // ------------------------------------------------------------------------
    void copy()
    {
        {
            int_matrix m {
                { 1, 2, 3 },
                { 4, 5, 6 },
                { 7, 8, 9 }
            };

            m.copy_lower_to_upper();

            TEST_EQUAL(str({
                { 1, 4, 7 },
                { 4, 5, 8 },
                { 7, 8, 9 }
            }), m.str());
        }

        {
            int_matrix m {
                { 1, 2, 3 },
                { 4, 5, 6 },
                { 7, 8, 9 }
            };

            m.copy_upper_to_lower();

            TEST_EQUAL(str({
                { 1, 2, 3 },
                { 2, 5, 6 },
                { 3, 6, 9 }
            }), m.str());
        }
    }

    // ------------------------------------------------------------------------
    void gemm()
    {
        {
            const real_matrix lhs {
                { 1, 2, 3 },
                { 4, 5, 6 }
            };

            const real_matrix rhs {
                { 1, 2 },
                { 4, 5 },
                { 7, 8 }
            };

            const real_matrix mul {
                { 30, 36 },
                { 66, 81 }
            };

            real_matrix out (2, 2);
            real_matrix::gemm(lhs, rhs, out, 1.0, 0.0);
            TEST_EQUAL(mul.str(), out.str());
        }

        {
            const real_matrix lhs { { 1, 2 }, { 3, 4 } };
            const real_matrix rhs { { 1, 2 }, { 3, 4 } };
            const real_matrix mul { { 7, 10 }, { 15, 22 } };

            real_matrix out = lhs * rhs;
            TEST_EQUAL(mul.str(), out.str());

            out = lhs;
            out *= rhs;
            TEST_EQUAL(mul.str(), out.str());
        }

        {
            const real_matrix lhs { { 1, 2, 3 }, { 4, 5, 6 } };
            const real_matrix rhs { { 1, 2 }, { 3, 4 }, { 5, 6 } };
            const real_matrix mul { { 22, 28 }, { 49, 64 } };

            real_matrix out = lhs * rhs;
            TEST_EQUAL(mul.str(), out.str());

            out = lhs;
            out *= rhs;
            TEST_EQUAL(mul.str(), out.str());
        }
    }

    // ------------------------------------------------------------------------
    void gesv()
    {
        real_matrix m {
            { 4, 3, 2, 16, 32 },
            { 2, 5, 6, 30, 60 },
            { 5, 4, 9, 40, 80 }
        };

        m.gesv();

        TEST_EQUAL(str({
            { 1 },
            { 2 },
            { 3 }
        }), m.copy_column(3).str());

        TEST_EQUAL(str({
            { 2 },
            { 4 },
            { 6 }
        }), m.copy_column(4).str());
    }

    // ------------------------------------------------------------------------
    void negate()
    {
        const int_matrix m {
            { +1, +2, +3 },
            { -4, -5, -6 }
        };

        const auto q = -m;

        TEST_EQUAL(str({
            { -1, -2, -3 },
            { +4, +5, +6 }
        }), q.str());
    }

    // ------------------------------------------------------------------------
    void potrf()
    {
        real_matrix m {
            {   4,  12, -16 },
            {  12,  37, -43 },
            { -16, -43,  98 }
        };

        m.potrf_lower();

        TEST_ALMOST(real_type(+2.0), m(0, 0), epsilon);
        TEST_ALMOST(real_type(+6.0), m(1, 0), epsilon);
        TEST_ALMOST(real_type(+1.0), m(1, 1), epsilon);
        TEST_ALMOST(real_type(-8.0), m(2, 0), epsilon);
        TEST_ALMOST(real_type(+5.0), m(2, 1), epsilon);
        TEST_ALMOST(real_type(+3.0), m(2, 2), epsilon);
    }

    // ------------------------------------------------------------------------
    void potri()
    {
        const real_matrix m0 {
            {   4,  12, -16 },
            {  12,  37, -43 },
            { -16, -43,  98 }
        };

        real_matrix m = m0;

        m.potrf_lower();
        m.potri_lower();
        m.copy_lower_to_upper();

        const auto q = m0 * m;

        for (size_t r = 0; r < 3; r++)
            for (size_t c = 0; c < 3; c++)
                TEST_ALMOST(real_type(r == c ? 1.0 : 0.0), q(r, c), epsilon);
    }

    // ------------------------------------------------------------------------
    void read()
    {
        std::istringstream in ("2 3 1 2 3 4 5 6");

        int_matrix m;
        m.read(in);

        TEST_EQUAL(str({
            { 1, 2, 3 },
            { 4, 5, 6 }
        }), m.str());

        std::istringstream bad1 ("0 1");
        TEST_THROWS(m.read(bad1));

        std::istringstream bad2 ("1 1 a");
        TEST_THROWS(m.read(bad2));

        TEST_EQUAL(str({
            { 1, 2, 3 },
            { 4, 5, 6 }
        }), m.str());
    }

    // ------------------------------------------------------------------------
    void resize()
    {
        int_matrix m {
            { 1, 2, 3 },
            { 4, 5, 6 }
        };

        m.resize(3, 2);

        TEST_EQUAL(str({
            { 1, 2 },
            { 3, 4 },
            { 5, 6 }
        }), m.str());
    }

    // ------------------------------------------------------------------------
    void row()
    {
        int_matrix m {
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        };

        TEST_EQUAL(str({
            { 4, 5, 6 }
        }), m.copy_row(1).str());

        m.multiply_row(0, 2);

        TEST_EQUAL(str({
            { 2, 4, 6 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        }), m.str());

        const int_matrix q { { 1, 2, 3 } };
        TEST_EQUAL(int_type(28), m.multiply_row(0, q));
    }

    // ------------------------------------------------------------------------
    void sum()
    {
        const int_matrix m {
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        };

        TEST_EQUAL(int_type(12), m.get_column_sum(0));
        TEST_EQUAL(int_type(15), m.get_column_sum(1));
        TEST_EQUAL(int_type(18), m.get_column_sum(2));
        TEST_EQUAL(int_type(6), m.get_row_sum(0));
        TEST_EQUAL(int_type(15), m.get_row_sum(1));
        TEST_EQUAL(int_type(24), m.get_row_sum(2));
        TEST_EQUAL(int_type(45), m.get_sum());
    }

    // ------------------------------------------------------------------------
    void transpose()
    {
        {
            const int_matrix m {
                { 1, 2, 3 },
                { 4, 5, 6 }
            };

            TEST_EQUAL(str({
                { 1, 4 },
                { 2, 5 },
                { 3, 6 }
            }), m.create_transpose().str());
        }

        {
            const int_matrix m {
                { 1, 2, 3 }
            };

            TEST_EQUAL(str({
                { 1 },
                { 2 },
                { 3 }
            }), m.create_transpose().str());
        }

        {
            const int_matrix m {
                { 1, 2, 3 },
                { 4, 5, 6 },
                { 7, 8, 9 }
            };

            TEST_EQUAL(str({
                { 1, 4, 7 },
                { 2, 5, 8 },
                { 3, 6, 9 }
            }), m.create_transpose().str());
        }
    }

    // ------------------------------------------------------------------------
    void write()
    {
        const real_matrix m1 {
            {real_type(1.1234561), real_type(2), real_type(2.9999999)},
            {real_type(4.1234559), real_type(5), real_type(6.0000001)}
        };

        std::ostringstream out;
        real_matrix::set_high_precision(out);
        out << m1;
        TEST_EQUAL(std::string(
            "2 3\n"
            "1.123456e+00\t2.000000e+00\t3.000000e+00\n"
            "4.123456e+00\t5.000000e+00\t6.000000e+00\n"),
            out.str());

        const int_matrix m2;
        TEST_EQUAL(std::string("0 0\n"), m2.str());
    }
}

namespace test
{
    test_group matrix {
        TEST_CASE(clamp),
        TEST_CASE(column),
        TEST_CASE(constructor),
        TEST_CASE(contains),
        TEST_CASE(copy),
        TEST_CASE(gemm),
        TEST_CASE(gesv),
        TEST_CASE(negate),
        TEST_CASE(potrf),
        TEST_CASE(potri),
        TEST_CASE(read),
        TEST_CASE(resize),
        TEST_CASE(row),
        TEST_CASE(sum),
        TEST_CASE(transpose),
        TEST_CASE(write)
    };
}
