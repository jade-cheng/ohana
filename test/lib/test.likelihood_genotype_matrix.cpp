/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.likelihood_genotype_matrix.hpp"

namespace
{
    const auto file_data =
        "3 4\n"
        "1\t0\t0\t1\n"
        "0\t0\t0\t1\n"
        "0\t1\t1\t0\n"
        "\n"
        "3 4\n"
        "0\t0\t1\t0\n"
        "1\t1\t1\t0\n"
        "1\t0\t0\t0\n"
        "\n"
        "3 4\n"
        "0\t1\t0\t0\n"
        "0\t0\t0\t0\n"
        "0\t0\t0\t1\n";

    typedef double value_type;
    typedef jade::basic_likelihood_genotype_matrix<value_type> lgm_type;
    typedef typename lgm_type::matrix_type matrix_type;

    // ------------------------------------------------------------------------
    void compute_derivatives_f()
    {
        const matrix_type aa {
            { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f },
            { 0.2f, 0.3f, 0.4f, 0.5f, 0.6f },
            { 0.3f, 0.4f, 0.5f, 0.6f, 0.7f },
            { 0.4f, 0.5f, 0.6f, 0.7f, 0.8f }
        };

        const matrix_type Aa {
            { 0.5f, 0.1f, 0.2f, 0.3f, 0.4f },
            { 0.6f, 0.2f, 0.3f, 0.4f, 0.5f },
            { 0.7f, 0.3f, 0.4f, 0.5f, 0.6f },
            { 0.8f, 0.4f, 0.5f, 0.6f, 0.7f }
        };

        const matrix_type AA {
            { 0.4f, 0.5f, 0.1f, 0.2f, 0.3f },
            { 0.5f, 0.6f, 0.2f, 0.3f, 0.4f },
            { 0.6f, 0.7f, 0.3f, 0.4f, 0.5f },
            { 0.7f, 0.8f, 0.4f, 0.5f, 0.6f }
        };

        const lgm_type g (aa, Aa, AA);

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
            { 1.10132f },
            { 1.03569f },
            { 1.60621f }
        };

        const matrix_type golden_h_mat {
            { -2.14220f, -1.01856f, -1.33123f },
            { -1.01856f, -1.16898f, -1.49324f },
            { -1.33123f, -1.49324f, -2.65673f }
        };

        TEST_EQUAL(golden_d_vec.str(), d_vec.str());
        TEST_EQUAL(golden_h_mat.str(), h_mat.str());
    }

    // ------------------------------------------------------------------------
    void compute_derivatives_q()
    {
        const matrix_type aa {
            { 0.1f, 0.2f, 0.3f, 0.4f, 0.5f },
            { 0.2f, 0.3f, 0.4f, 0.5f, 0.6f },
            { 0.3f, 0.4f, 0.5f, 0.6f, 0.7f },
            { 0.4f, 0.5f, 0.6f, 0.7f, 0.8f }
        };

        const matrix_type Aa {
            { 0.5f, 0.1f, 0.2f, 0.3f, 0.4f },
            { 0.6f, 0.2f, 0.3f, 0.4f, 0.5f },
            { 0.7f, 0.3f, 0.4f, 0.5f, 0.6f },
            { 0.8f, 0.4f, 0.5f, 0.6f, 0.7f }
        };

        const matrix_type AA {
            { 0.4f, 0.5f, 0.1f, 0.2f, 0.3f },
            { 0.5f, 0.6f, 0.2f, 0.3f, 0.4f },
            { 0.6f, 0.7f, 0.3f, 0.4f, 0.5f },
            { 0.7f, 0.8f, 0.4f, 0.5f, 0.6f }
        };

        const lgm_type g (aa, Aa, AA);

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
            { 10.3317f },
            { 9.66340f },
            { 10.0693f }
        };

        const matrix_type golden_h_mat {
            { -11.1683f, -10.2220f, -10.0629f },
            { -10.2220f, -9.06087f, -9.80150f },
            { -10.0629f, -9.80150f, -10.2325f }
        };

        TEST_EQUAL(golden_d_vec.str(), d_vec.str());
        TEST_EQUAL(golden_h_mat.str(), h_mat.str());
    }

    // ------------------------------------------------------------------------
    void constructor()
    {
        {
            const matrix_type aa {
                { 1.0f, 0.0f, 0.0f, 1.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f },
                { 0.0f, 1.0f, 1.0f, 0.0f }
            };

            const matrix_type Aa {
                { 0.0f, 0.0f, 1.0f, 0.0f },
                { 1.0f, 1.0f, 1.0f, 0.0f },
                { 1.0f, 0.0f, 0.0f, 0.0f }
            };

            const matrix_type AA {
                { 0.0f, 1.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 0.0f },
                { 0.0f, 0.0f, 0.0f, 1.0f }
            };

            const lgm_type m (aa, Aa, AA);
            TEST_EQUAL(std::string(file_data), m.str());
        }

        {
            std::istringstream in (file_data);
            const lgm_type m (in);
            TEST_EQUAL(std::string(file_data), m.str());
        }
    }

    // ------------------------------------------------------------------------
    void constructor_mismatched_sizes()
    {
        const auto bad_data =
            "3 4\n"
            "1 0 0 1\n"
            "0 0 0 1\n"
            "0 1 1 0\n"
            "\n"
            "3 4\n"
            "0 0 1 0\n"
            "1 1 1 0\n"
            "1 0 0 0\n"
            "\n"
            "3 3\n"
            "0 1 0\n"
            "0 0 0\n"
            "0 0 0\n";

        std::istringstream in (bad_data);
        TEST_THROWS(lgm_type m (in));
    }

    // ------------------------------------------------------------------------
    void constructor_invalid_data()
    {
        const auto bad_data =
            "3 4\n"
            "1 0 0 1\n"
            "0 0 0 1\n"
            "0 1 1 0\n"
            "\n"
            "3 4\n"
            "0 0 1 0\n"
            "1 1 1 0\n"
            "1 0 0 0\n"
            "\n"
            "3 4\n"
            "0 1 0 0\n"
            "0 0 0 0\n"
            "0 0 X 1\n";

        std::istringstream in (bad_data);
        TEST_THROWS(lgm_type m (in));
    }
}

namespace test
{
    test_group likelihood_genotype_matrix {
        TEST_CASE(compute_derivatives_f),
        TEST_CASE(compute_derivatives_q),
        TEST_CASE(constructor),
        TEST_CASE(constructor_invalid_data),
        TEST_CASE(constructor_mismatched_sizes)
    };
}
