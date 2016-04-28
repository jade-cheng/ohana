/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.simplex.hpp"

namespace
{
    typedef double value_type;

    const value_type epsilon = value_type(0.0001);

    // ------------------------------------------------------------------------
    template <typename TValue>
    struct basic_himmelblau
    {
        typedef TValue                                value_type;
        typedef jade::basic_simplex<value_type>       simplex_type;
        typedef typename simplex_type::container_type container_type;

        // --------------------------------------------------------------------
        static value_type objfunc(const container_type & params)
        {
            assert(params.size() == 2);

            const auto x = params[0];
            const auto y = params[1];
            const auto a = x * x + y - 11;
            const auto b = y * y + x - 7;
            return a * a + b * b;
        }
    };

    // ------------------------------------------------------------------------
    template <typename TValue>
    struct basic_distance
    {
        typedef TValue                                value_type;
        typedef jade::basic_simplex<value_type>       simplex_type;
        typedef typename simplex_type::container_type container_type;

        // --------------------------------------------------------------------
        static value_type objfunc(const container_type & params)
        {
            value_type sum = 0;
            for (size_t i = 0; i < params.size(); i++)
                sum += std::fabs(params[i] - value_type(i));
            return sum;
        }
    };

    // ------------------------------------------------------------------------
    value_type c_style_objfunc(const std::vector<value_type> & params)
    {
        typedef basic_distance<value_type> distance_type;
        return distance_type::objfunc(params);
    }

    // ------------------------------------------------------------------------
    template <typename TValue>
    std::string str(const std::vector<TValue> & params)
    {
        std::ostringstream out;
        out << "{";
        auto iter = params.begin();
        if (iter != params.end())
        {
            out << *iter++;
            while (iter != params.end())
                out << "," << *iter++;
        }
        out << "}";
        return out.str();
    }

    // ------------------------------------------------------------------------
    template <typename TValue>
    std::string str(
            const jade::basic_simplex<TValue> & simplex,
            const size_t                        index)
    {
        return str(simplex.get_vertex(index));
    }

    // ------------------------------------------------------------------------
    template <typename TValue>
    std::string str(const std::initializer_list<TValue> & params)
    {
        return str(std::vector<TValue>(params));
    }

    // ------------------------------------------------------------------------
    void constructor()
    {
        {
            typedef basic_distance<value_type>          objfunc_type;
            typedef typename objfunc_type::simplex_type simplex_type;

            simplex_type s (objfunc_type::objfunc, 3);

            TEST_EQUAL(str({ 0, 1, 0 }), str(s, 0));
            TEST_EQUAL(str({ 0, 0, 1 }), str(s, 1));
            TEST_EQUAL(str({ 0, 0, 0 }), str(s, 2));
            TEST_EQUAL(str({ 1, 0, 0 }), str(s, 3));

            TEST_ALMOST(value_type(2.0), s.get_objval(0), epsilon);
            TEST_ALMOST(value_type(2.0), s.get_objval(1), epsilon);
            TEST_ALMOST(value_type(3.0), s.get_objval(2), epsilon);
            TEST_ALMOST(value_type(4.0), s.get_objval(3), epsilon);

            const auto & stats = s.get_stats();
            TEST_EQUAL(size_t(4), stats.evaluations);
            TEST_EQUAL(size_t(0), stats.expansions);
            TEST_EQUAL(size_t(0), stats.contractions_in);
            TEST_EQUAL(size_t(0), stats.contractions_out);
            TEST_EQUAL(size_t(0), stats.iterations);
            TEST_EQUAL(size_t(0), stats.reflections);
            TEST_EQUAL(size_t(0), stats.shrinkages);
        }

        {
            typedef jade::basic_simplex<value_type> simplex_type;

            simplex_type s (c_style_objfunc, 3);

            TEST_EQUAL(str({ 0, 1, 0 }), str(s, 0));
            TEST_EQUAL(str({ 0, 0, 1 }), str(s, 1));
            TEST_EQUAL(str({ 0, 0, 0 }), str(s, 2));
            TEST_EQUAL(str({ 1, 0, 0 }), str(s, 3));

            TEST_ALMOST(value_type(2.0), s.get_objval(0), epsilon);
            TEST_ALMOST(value_type(2.0), s.get_objval(1), epsilon);
            TEST_ALMOST(value_type(3.0), s.get_objval(2), epsilon);
            TEST_ALMOST(value_type(4.0), s.get_objval(3), epsilon);

            const auto & stats = s.get_stats();
            TEST_EQUAL(size_t(4), stats.evaluations);
            TEST_EQUAL(size_t(0), stats.expansions);
            TEST_EQUAL(size_t(0), stats.contractions_in);
            TEST_EQUAL(size_t(0), stats.contractions_out);
            TEST_EQUAL(size_t(0), stats.iterations);
            TEST_EQUAL(size_t(0), stats.reflections);
            TEST_EQUAL(size_t(0), stats.shrinkages);
        }
    }

    // ------------------------------------------------------------------------
    void delta()
    {
        typedef basic_distance<value_type>          objfunc_type;
        typedef typename objfunc_type::simplex_type simplex_type;

        simplex_type s (objfunc_type::objfunc, 3);

        TEST_ALMOST(value_type(2.0), s.get_delta(), epsilon);
    }

    // ------------------------------------------------------------------------
    void execute()
    {
        typedef basic_himmelblau<value_type>        objfunc_type;
        typedef typename objfunc_type::simplex_type simplex_type;
        typedef typename simplex_type::execute_args execute_args_type;

        simplex_type s (objfunc_type::objfunc, 2);
        execute_args_type args;
        args.max_iterations = 50;
        s.execute(objfunc_type::objfunc, args);

        TEST_EQUAL(size_t(50), s.get_stats().iterations);

        const auto v = s.get_vertex();
        TEST_ALMOST(value_type(3.0), v[0], epsilon);
        TEST_ALMOST(value_type(2.0), v[1], epsilon);
    }

    // ------------------------------------------------------------------------
    void himmelblau()
    {
        typedef basic_himmelblau<value_type>        objfunc_type;
        typedef typename objfunc_type::simplex_type simplex_type;

        const auto max_iterations = 50;

        {
            simplex_type::options opts (2);
            opts.vertex[0] = +1;
            opts.vertex[1] = +1;
            simplex_type s (objfunc_type::objfunc, opts);

            for (auto i = max_iterations; i > 0; i--)
                s.iterate(objfunc_type::objfunc);

            const auto v = s.get_vertex();
            TEST_ALMOST(value_type(3.0), v[0], epsilon);
            TEST_ALMOST(value_type(2.0), v[1], epsilon);
        }

        {
            simplex_type::options opts (2);
            opts.vertex[0] = -1;
            opts.vertex[1] = +1;
            simplex_type s (objfunc_type::objfunc, opts);

            for (auto i = max_iterations; i > 0; i--)
                s.iterate(objfunc_type::objfunc);

            const auto v = s.get_vertex();
            TEST_ALMOST(value_type(-2.805118), v[0], epsilon);
            TEST_ALMOST(value_type(+3.131312), v[1], epsilon);
        }

        {
            simplex_type::options opts (2);
            opts.vertex[0] = +1;
            opts.vertex[1] = -1;
            simplex_type s (objfunc_type::objfunc, opts);

            for (auto i = max_iterations; i > 0; i--)
                s.iterate(objfunc_type::objfunc);

            const auto v = s.get_vertex();
            TEST_ALMOST(value_type(+3.584428), v[0], epsilon);
            TEST_ALMOST(value_type(-1.848126), v[1], epsilon);
        }

        {
            simplex_type::options opts (2);
            opts.vertex[0] = -2;
            opts.vertex[1] = -2;
            simplex_type s (objfunc_type::objfunc, opts);

            for (auto i = max_iterations; i > 0; i--)
                s.iterate(objfunc_type::objfunc);

            const auto v = s.get_vertex();
            TEST_ALMOST(value_type(-3.779310), v[0], epsilon);
            TEST_ALMOST(value_type(-3.283186), v[1], epsilon);
        }
    }

    // ------------------------------------------------------------------------
    void iterate()
    {
        //
        // NOTE This is "test-by-inspection" in which we visualize the
        // convergence of the simplex on Himmelblau's function.
        //
        typedef basic_himmelblau<value_type>          objfunc_type;
        typedef typename objfunc_type::simplex_type   simplex_type;
        typedef typename simplex_type::operation      operation_type;
        typedef typename simplex_type::container_type container_type;

        simplex_type::options opts (2);
        opts.vertex[0] = +1;
        opts.vertex[1] = -1;
        simplex_type s (objfunc_type::objfunc, opts);

        std::vector<value_type> x, y;
        std::vector<container_type> circles;

        std::ostringstream out;
        int op = -1;
        for (size_t j = 0; j < 50; j++)
        {
            circles.push_back(s.get_vertex());

            auto fill = "none";
            switch (op) {
                case operation_type::expansion:       fill = "green";  break;
                case operation_type::reflection:      fill = "blue";   break;
                case operation_type::shrinkage:       fill = "yellow"; break;
                case operation_type::contraction_in:  fill = "orange"; break;
                case operation_type::contraction_out: fill = "red";    break;
                default:                              fill = "none";   break;
            }

            out << "<path style='fill:" << fill << "' d='M";
            for (size_t i = 0; i < 3; i++)
            {
                const auto & vi = s.get_vertex(i);
                out << vi[0] << ' ' << vi[1] << ' ';
                x.push_back(vi[0]);
                y.push_back(vi[1]);
            }

            out << "Z' />\n";

            op = s.iterate(objfunc_type::objfunc);
        }

        const auto xx = std::minmax_element(x.begin(), x.end());
        const auto yy = std::minmax_element(y.begin(), y.end());
        const auto dx = *xx.second - *xx.first;
        const auto dy = *yy.second - *yy.first;
        const auto dd = std::max(dx, dy);
        const auto mg = dd * value_type(0.05);
        const auto sw = dd * value_type(0.001);
        const auto ra = dd * value_type(0.003);

        for (const auto & c : circles)
            out << "<circle cx='" << c[0] << "' cy='" << c[1]
                << "' r='" << ra << "' style='fill:black' />\n";

        std::ofstream svg ("tmp/wip.svg");

        svg << "<svg xmlns='http://www.w3.org/2000/svg' viewBox='"
            << *xx.first - mg << " " << *yy.first - mg << " "
            << dx + mg + mg << " " << dy + mg + mg << "'>\n"
            << "<g style='fill-opacity:0.25;"
            << "stroke-width:" << sw << ";" << "stroke:black'>\n"
            << out.str()
            << "</g>\n</svg>\n";
    }

    // ------------------------------------------------------------------------
    void length_squared()
    {
        typedef basic_distance<value_type>          objfunc_type;
        typedef typename objfunc_type::simplex_type simplex_type;

        simplex_type s (objfunc_type::objfunc, 3);

        TEST_ALMOST(value_type(2.0), s.get_length_squared(), epsilon);
    }

    // ------------------------------------------------------------------------
    void many_parameters()
    {
        typedef basic_distance<value_type>          objfunc_type;
        typedef typename objfunc_type::simplex_type simplex_type;
        typedef typename simplex_type::execute_args execute_args_type;
        typedef typename simplex_type::options      options_type;

        options_type o (10);
        o.unit = value_type(10.0);
        simplex_type s (objfunc_type::objfunc, o);

        execute_args_type a;
        a.max_iterations = 1000;
        s.execute(objfunc_type::objfunc, a);

        const auto & v = s.get_vertex();
        for (size_t i = 0; i < v.size(); i++)
            TEST_ALMOST(value_type(i), v[i], epsilon);
    }

    // ------------------------------------------------------------------------
    void options()
    {
        typedef basic_distance<value_type>          objfunc_type;
        typedef typename objfunc_type::simplex_type simplex_type;
        typedef typename simplex_type::options      options_type;

        {
            options_type opts (3);

            TEST_EQUAL(
                str({0, 0, 0}),
                str(opts.vertex));

            TEST_ALMOST(value_type(1.666667), opts.chi,   epsilon);
            TEST_ALMOST(value_type(0.583333), opts.gamma, epsilon);
            TEST_ALMOST(value_type(1.000000), opts.rho,   epsilon);
            TEST_ALMOST(value_type(0.666667), opts.sigma, epsilon);
            TEST_ALMOST(value_type(1.000000), opts.unit,  epsilon);
        }

        {
            options_type opts (1);

            TEST_EQUAL(
                str({0}),
                str(opts.vertex));

            TEST_ALMOST(value_type(2.000000), opts.chi,   epsilon);
            TEST_ALMOST(value_type(0.500000), opts.gamma, epsilon);
            TEST_ALMOST(value_type(1.000000), opts.rho,   epsilon);
            TEST_ALMOST(value_type(0.500000), opts.sigma, epsilon);
            TEST_ALMOST(value_type(1.000000), opts.unit,  epsilon);
        }
    }
}

namespace test
{
    test_group simplex {
        TEST_CASE(constructor),
        TEST_CASE(delta),
        TEST_CASE(::execute),
        TEST_CASE(iterate),
        TEST_CASE(himmelblau),
        TEST_CASE(length_squared),
        TEST_CASE(many_parameters),
        TEST_CASE(options)
    };
}
