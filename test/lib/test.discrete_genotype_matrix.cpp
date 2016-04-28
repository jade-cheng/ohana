/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.discrete_genotype_matrix.hpp"

namespace
{
    const auto AA = jade::genotype_major_major;
    const auto Aa = jade::genotype_major_minor;
    const auto aa = jade::genotype_minor_minor;

    typedef double value_type;
    typedef jade::basic_discrete_genotype_matrix<value_type> dgm_type;
    typedef typename dgm_type::matrix_type matrix_type;

    // ------------------------------------------------------------------------
    void compute_derivatives_f()
    {
        const dgm_type g {
            { AA, AA, Aa, Aa, aa },
            { AA, aa, Aa, AA, AA },
            { Aa, Aa, aa, AA, Aa },
            { AA, AA, aa, Aa, AA }
        };

        const matrix_type q {
            { 0.2f, 0.3f, 0.5f },
            { 0.3f, 0.4f, 0.3f },
            { 0.9f, 0.1f, 0.0f },
            { 0.3f, 0.1f, 0.6f }
        };

        const matrix_type fa {
            { 0.7f, 0.8f, 0.6f, 0.9f, 0.8f },
            { 0.2f, 0.1f, 0.3f, 0.4f, 0.2f },
            { 0.2f, 0.4f, 0.2f, 0.1f, 0.5f }
        };

        const auto K = 3;

        matrix_type fb (K, 5);
        fb.set_values(1);
        fb -= fa;

        const auto qfa = q * fa;
        const auto qfb = q * fb;

        matrix_type d_vec (K, 1);
        matrix_type h_mat (K, K);
        g.compute_derivatives_f(q, fa, fb, qfa, qfb, 0, d_vec, h_mat);

        const matrix_type golden_d_vec {
            { -3.60440f },
            { -2.52747f },
            { -4.19780f }
        };

        const matrix_type golden_h_mat {
            { -9.54474f, -1.90267f, -1.68627f },
            { -1.90267f, -1.27738f, -1.46432f },
            { -1.68627f, -1.46432f, -3.15059f }
        };

        TEST_EQUAL(golden_d_vec.str(), d_vec.str());
        TEST_EQUAL(golden_h_mat.str(), h_mat.str());
    }

    // ------------------------------------------------------------------------
    void compute_derivatives_q()
    {
        const dgm_type g {
            { AA, AA, Aa, Aa, aa },
            { AA, aa, Aa, AA, AA },
            { Aa, Aa, aa, AA, Aa },
            { AA, AA, aa, Aa, AA }
        };

        const matrix_type q {
            { 0.2f, 0.3f, 0.5f },
            { 0.3f, 0.4f, 0.3f },
            { 0.9f, 0.1f, 0.0f },
            { 0.3f, 0.1f, 0.6f }
        };

        const matrix_type fa {
            { 0.7f, 0.8f, 0.6f, 0.9f, 0.8f },
            { 0.2f, 0.1f, 0.3f, 0.4f, 0.2f },
            { 0.2f, 0.4f, 0.2f, 0.1f, 0.5f }
        };

        const auto K = 3;

        matrix_type fb (K, 5);
        fb.set_values(1);
        fb -= fa;

        const auto qfa = q * fa;
        const auto qfb = q * fb;

        matrix_type d_vec (K, 1);
        matrix_type h_mat (K, K);
        g.compute_derivatives_q(q, fa, fb, qfa, qfb, 0, d_vec, h_mat);

        const matrix_type golden_d_vec {
            { 10.1576f },
            { 10.1358f },
            { 9.85550f }
        };

        const matrix_type golden_h_mat {
            { -17.0949f, -8.93764f, -8.11467f },
            { -8.93764f, -11.4520f, -9.82529f },
            { -8.11467f, -9.82529f, -10.5700f }
        };

        TEST_EQUAL(golden_d_vec.str(), d_vec.str());
        TEST_EQUAL(golden_h_mat.str(), h_mat.str());
    }

    // ------------------------------------------------------------------------
    void constructor()
    {
        const dgm_type m {
            { AA, AA, AA },
            { Aa, Aa, Aa },
            { aa, aa, aa }
        };

        TEST_EQUAL(std::string("3 3\n0\t0\t0\n1\t1\t1\n2\t2\t2\n"), m.str());

        {
            std::istringstream in ("3 3\n0 0 0\n1 1 1\n2 2 2\n");
            dgm_type q (in);
            TEST_EQUAL(m.str(), q.str());
        }
    }
}

namespace test
{
    test_group discrete_genotype_matrix {
        TEST_CASE(compute_derivatives_f),
        TEST_CASE(compute_derivatives_q),
        TEST_CASE(constructor)
    };
}
