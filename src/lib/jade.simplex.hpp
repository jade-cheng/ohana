/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SIMPLEX_HPP__
#define JADE_SIMPLEX_HPP__

#include "jade.assert.hpp"

namespace jade
{
    ///
    /// A template for a class that implements the Nelder-Mead Simplex Method,
    /// which attempts to minimize an objective function in a many-dimensional
    /// space by continually refining a simplex.
    ///
    template <typename TValue>
    class basic_simplex
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The container type.
        typedef std::vector<value_type> container_type;

        ///
        /// An indication of what caused the execute to temrinate.
        ///
        struct exit_condition
        {
            ///
            /// The exit condition.
            ///
            enum type
            {
                delta,     ///< The minimum delta occurred.
                epsilon,   ///< The minimum epsilon occurred.
                iteration, ///< The maximum iteration occurred.
                length,    ///< The minimum length occurred.
                timeout    ///< The maximum timeout occurred.
            };

            ///
            /// \return A string representation of the type.
            ///
            static const char * str(
                    const type value) ///< The type to format.
            {
                switch (value)
                {
                    #define CASE(E) case E: return #E
                    CASE(delta);
                    CASE(epsilon);
                    CASE(iteration);
                    CASE(length);
                    CASE(timeout);
                    #undef CASE
                }

                return nullptr;
            }
        };

        /// The exit condition type.
        typedef typename exit_condition::type exit_condition_type;

        ///
        /// An operation performed during an iteration.
        ///
        struct operation
        {
            ///
            /// The type of operation.
            ///
            enum type
            {
                expansion,       ///< An expansion.
                contraction_in,  ///< An inside contraction.
                contraction_out, ///< An outside contraction.
                reflection,      ///< A reflection.
                shrinkage        ///< A shrinkage.
            };

            ///
            /// \return A string representation of the type.
            ///
            static const char * str(
                    const type value) ///< The value to format.
            {
                switch (value)
                {
                    #define CASE(E) case E: return #E
                    CASE(expansion);
                    CASE(contraction_in);
                    CASE(contraction_out);
                    CASE(reflection);
                    CASE(shrinkage);
                    #undef CASE
                }

                return nullptr;
            }
        };

        /// The operation type.
        typedef typename operation::type operation_type;

        ///
        /// Arguments passed to the logging function.
        ///
        struct log_args
        {
            size_t                iteration; ///< The completed iteration.
            double                second;    ///< The seconds elapsed.
            const basic_simplex * simplex;   ///< The simplex instance.
            void *                user;      ///< User-supplied value.

            ///
            /// Initializes a new instance of the class.
            ///
            /// \param simplex_   The simplex generating the log event.
            /// \param second_    The number of seconds elapsed.
            /// \param iteration_ The completed iteration.
            /// \param user_      The user-supplied value, or nullptr.
            ///
            inline log_args(
                    const basic_simplex * const simplex_,
                    const double                second_,
                    const size_t                iteration_,
                    void * const                user_)
                : iteration (iteration_)
                , second    (second_)
                , simplex   (simplex_)
                , user      (user_)
            {
            }

            ///
            /// \return A string representation of the class.
            ///
            std::string str() const
            {
                std::ostringstream out;
                out << "iteration:        " << iteration << std::endl
                    << "second:           " << second    << std::endl;
                return out.str();
            }
        };

        ///
        /// Arguments to the execute method.
        ///
        struct execute_args
        {
            /// The C-style log function.
            typedef void (*logfunc_type)(const log_args &);

            /// The value assigned for no maximum iterations.
            static constexpr auto no_max_iterations =
                std::numeric_limits<size_t>::max();

            /// The value assigned for no maximum seconds.
            static constexpr auto no_max_seconds =
                std::numeric_limits<double>::quiet_NaN();

            /// The value assigned for no minimum delta.
            static constexpr auto no_min_delta =
                std::numeric_limits<value_type>::quiet_NaN();

            /// The value assigned for no minimum epsilon.
            static constexpr auto no_min_epsilon =
                std::numeric_limits<value_type>::quiet_NaN();

            /// The value assigned for no minimum length.
            static constexpr auto no_min_length =
                std::numeric_limits<value_type>::quiet_NaN();

            logfunc_type logfunc;        ///< The logging function.
            size_t       max_iterations; ///< The maximum iterations.
            double       max_seconds;    ///< The maximum seconds.
            value_type   min_delta;      ///< The minimum delta.
            value_type   min_epsilon;    ///< The minimum change in objval.
            value_type   min_length;     ///< The minimum length.
            void *       user;           ///< User-supplied value.

            ///
            /// Initializes a new instance of the class.
            ///
            inline execute_args()
                : logfunc        (nullptr)
                , max_iterations (no_max_iterations)
                , max_seconds    (no_max_seconds)
                , min_delta      (no_min_delta)
                , min_epsilon    (no_min_epsilon)
                , min_length     (no_min_length)
                , user           (nullptr)
            {
            }

            ///
            /// \return True if the logging function is assigned.
            ///
            bool is_logfunc_assigned() const
            {
                return logfunc != nullptr;
            }

            ///
            /// \return True if the maximum iterations is assigned.
            ///
            bool is_max_iterations_assigned() const
            {
                return max_iterations != no_max_iterations;
            }

            ///
            /// \return True if the maximum seconds is assigned.
            ///
            bool is_max_seconds_assigned() const
            {
                return !std::isnan(max_seconds);
            }

            ///
            /// \return True if the minimum delta is assigned.
            ///
            bool is_min_delta_assigned() const
            {
                return !std::isnan(min_delta);
            }

            ///
            /// \return True if the minimum epsilon is assigned.
            ///
            bool is_min_epsilon_assigned() const
            {
                return !std::isnan(min_epsilon);
            }

            ///
            /// \return True if the minimum length is assigned.
            ///
            bool is_min_length_assigned() const
            {
                return !std::isnan(min_length);
            }

            ///
            /// \return A string representation of the class.
            ///
            std::string str() const
            {
                std::ostringstream out;
                out << "logfunc:          "
                    << _str(logfunc != nullptr, "assigned")
                    << std::endl
                    << "user:             "
                    << _str(user != nullptr, "assigned")
                    << std::endl
                    << "max_iteration:    "
                    << _str(is_max_iterations_assigned(), max_iterations)
                    << std::endl
                    << "max_second:       "
                    << _str(is_max_seconds_assigned(), max_seconds)
                    << std::endl
                    << "min_delta:        "
                    << _str(is_min_delta_assigned(), min_delta)
                    << std::endl
                    << "min_epsilon:      "
                    << _str(is_min_epsilon_assigned(), min_epsilon)
                    << std::endl
                    << "min_length:       "
                    << _str(is_min_length_assigned(), min_length)
                    << std::endl;
                return out.str();
            }

        private:
            // ----------------------------------------------------------------
            template <typename T>
            static std::string _str(
                    const bool     is_assigned,
                    const T        value)
            {
                std::ostringstream out;
                if (is_assigned) out << value;
                else out << "<not-assigned>";
                return out.str();
            }
        };

        ///
        /// A data structure used to initialize the Nelder-Mead algorithm.
        ///
        /// The default values for rho, chi, gamma, and sigma are based on the
        /// 2010 paper 'Implementing the Nelder-Mead simplex algorithm with
        /// adaptive parameters' by Fuchang Gao and Lixing Han.
        ///
        struct options
        {
            ///
            /// A vertex in the initial formation of the simplex.
            ///
            container_type vertex;

            ///
            /// The expansion coefficient, which must be greater than the
            /// larger or rho or 1.0.
            ///
            value_type chi;

            ///
            /// The contraction coefficient, which must be greater than 0.0 and
            /// less than 1.0.
            ///
            value_type gamma;

            ///
            /// The reflection coefficient, which must be greater than 0.0.
            ///
            value_type rho;

            ///
            /// The shrinkage coefficient, which must be greater than 0.0 and
            /// less than 1.0.
            ///
            value_type sigma;

            ///
            /// A scaling coefficient used to construct the unit vectors in each
            /// dimension of the simplex, or undefined to use 1.0.
            ///
            value_type unit;

            ///
            /// Initializes a new instance of the class.
            ///
            explicit options(
                    const size_t n) ///< The container size.
                : vertex ()
                , chi    (_init_chi(n))
                , gamma  (_init_gamma(n))
                , rho    (1)
                , sigma  (_init_sigma(n))
                , unit   (1)
            {
                assert(n > 0);
                vertex.resize(n);
            }

            ///
            /// Initializes a new instance of the class.
            ///
            explicit options(
                    const container_type & vertex_) ///< The container size.
                : vertex (vertex_)
                , chi    (_init_chi(vertex.size()))
                , gamma  (_init_gamma(vertex.size()))
                , rho    (1)
                , sigma  (_init_sigma(vertex.size()))
                , unit   (1)
            {
                assert(vertex.size() > 0);
            }

            ///
            /// \return A string representation of the class.
            ///
            std::string str() const
            {
                std::ostringstream out;

                out << "vertex:           {";
                auto iter = vertex.begin();
                if (iter != vertex.end())
                {
                    out << *iter;
                    while (++iter != vertex.end())
                        out << "," << *iter;
                }
                out << "}" << std::endl;

                out
                    << "chi:              " << chi    << std::endl
                    << "gamma:            " << gamma  << std::endl
                    << "rho:              " << rho    << std::endl
                    << "sigma:            " << sigma  << std::endl
                    << "unit:             " << unit   << std::endl;

                return out.str();
            }

        private:
            static constexpr value_type k1_00 = value_type(1.00);
            static constexpr value_type k2_00 = value_type(2.00);
            static constexpr value_type k0_50 = value_type(0.50);
            static constexpr value_type k0_75 = value_type(0.75);

            // ----------------------------------------------------------------
            static value_type _init_chi(const size_t n)
            {
                return n >= 2 ? k1_00 + (k2_00 / value_type(n)) : k2_00;
            }

            // ----------------------------------------------------------------
            static value_type _init_gamma(const size_t n)
            {
                return n >= 2 ? k0_75 - (k1_00 /
                    (k2_00 * value_type(n))) : k0_50;
            }

            // ----------------------------------------------------------------
            static value_type _init_sigma(const size_t n)
            {
                return n >= 2 ? k1_00 - (k1_00 / value_type(n)) : k0_50;
            }
        };

        ///
        /// Statistics about the behavior of the algorithm.
        ///
        struct stats
        {
            ///
            /// The number of expansions that have occurred.
            ///
            size_t expansions;

            ///
            /// The number of evaluations of the objective function.
            ///
            size_t evaluations;

            ///
            /// The number of inside contractions that have occurred.
            ///
            size_t contractions_in;

            ///
            /// The number of outside contractions that have occurred.
            ///
            size_t contractions_out;

            ///
            /// The number of iterations that have executed.
            ///
            size_t iterations;

            ///
            /// The number of reflections that have occurred.
            ///
            size_t reflections;

            ///
            /// The number of shrinkages that have occurred.
            ///
            size_t shrinkages;

            ///
            /// Initializes a new instance of the class.
            ///
            inline stats()
                : expansions       (0)
                , evaluations      (0)
                , contractions_in  (0)
                , contractions_out (0)
                , iterations       (0)
                , reflections      (0)
                , shrinkages       (0)
            {
            }

            ///
            /// \return A string representation of the class.
            ///
            std::string str() const
            {
                std::ostringstream out;
                out << "evaluations:      " << evaluations      << std::endl
                    << "iterations:       " << iterations       << std::endl
                    << "reflections:      " << reflections      << std::endl
                    << "expansions:       " << expansions       << std::endl
                    << "contractions_in:  " << contractions_in  << std::endl
                    << "contractions_out: " << contractions_out << std::endl
                    << "shrinkages:       " << shrinkages       << std::endl;
                return out.str();
            }
        };

        ///
        /// Initializes a new instance of the class based on the specified
        /// required values.
        ///
        template <typename TObjfunc>
        inline basic_simplex(
                const TObjfunc objfunc, ///< The objective function.
                const size_t   n)       ///< The number of dimensions.
            : basic_simplex(objfunc, options(n))
        {
        }

        ///
        /// Initializes a new instance of the class based on the specified
        /// options.
        ///
        template <typename TObjfunc>
        basic_simplex(
                const TObjfunc  objfunc, ///< The objective function.
                const options & opts)    ///< The options.
            : _n     (opts.vertex.size())
            , _opts  (opts)
            , _stats ()
            , _x     ()
            , _xbar  ()
            , _xr    ()
            , _xc    ()
            , _xcc   ()
            , _xe    ()
        {
            assert(_n > 0);

            assert(opts.rho > 0);
            assert(opts.chi >= 1 && opts.chi >= opts.rho);
            assert(opts.gamma > 0 && opts.gamma < 1);
            assert(opts.sigma > 0 && opts.sigma < 1);
            assert(opts.unit > 0);
            assert(opts.vertex.empty() || opts.vertex.size() == _n);

            //
            // Add N+1 vertices with N of them offset from the initial vertex by
            // one unit in a dimension corresponding to their index.
            //
            for (size_t i = 0; i <= _n; i++)
                _x.emplace_back(new vertex(opts, i));

            //
            // Initially evaluate the objective function for the simplex.
            //
            for (const auto & xi : _x)
                xi->objval = _evaluate(objfunc, xi->params);

            //
            // Initially sort the vertices.
            //
            std::sort(_x.begin(), _x.end(), _is_less);

            //
            // Declare vertices here to avoid memory allocations while
            // iterating.
            //
            _xbar.resize(_n);
            _xr.resize(_n);
            _xc.resize(_n);
            _xcc.resize(_n);
            _xe.resize(_n);
        }

        ///
        /// Calls the iterate method until an exit condition is reached.
        ///
        /// \return The exit condition.
        ///
        template <typename TObjfunc>
        exit_condition_type execute(
                const TObjfunc       objfunc,  ///< The objective function.
                const execute_args & exe_args) ///< The execution arguments.
        {
            typedef std::chrono::high_resolution_clock clock_type;
            typedef std::chrono::duration<double>      duration_type;

            const auto t0 = clock_type::now();
            const auto get_seconds = [t0]() -> double
            {
                return duration_type(clock_type::now() - t0).count();
            };

            auto iteration = size_t(0);
            auto flux0     = get_flux();

            for (;;)
            {
                if (exe_args.is_max_iterations_assigned())
                    if (iteration >= exe_args.max_iterations)
                        return exit_condition_type::iteration;

                if (exe_args.is_max_seconds_assigned())
                    if (get_seconds() >= exe_args.max_seconds)
                        return exit_condition_type::timeout;

                if (exe_args.is_min_delta_assigned())
                    if (get_delta() <= exe_args.min_delta)
                        return exit_condition_type::delta;

                if (exe_args.is_min_length_assigned())
                    if (get_length() <= exe_args.min_length)
                        return exit_condition_type::length;

                iterate(objfunc);
                iteration++;

                if (exe_args.is_logfunc_assigned())
                    exe_args.logfunc(log_args(
                            this,
                            get_seconds(),
                            iteration,
                            exe_args.user));

                auto flux = get_flux();

                if (exe_args.is_min_epsilon_assigned())
                    if (std::fabs(flux0 - flux) <= exe_args.min_epsilon)
                        return exit_condition::epsilon;
                flux0 = flux;
            }
        }

        ///
        /// \return The difference between the best and worst values returned
        /// from the objective function for all vertices of the simplex.
        ///
        value_type get_delta() const
        {
            return _x.back()->objval - _x.front()->objval;
        }

        ///
        /// \return The difference between the value compared to the epsilon
        /// in the execute method.  This value is determined by dividing the
        /// sum of the non-infinite values by the number of non-infinite values
        /// squared.
        ///
        value_type get_flux() const
        {
            static const auto inf = std::numeric_limits<value_type>::max();

            auto sum   = value_type(0);
            auto count = 0;

            for (const auto & x : _x)
            {
                if (x->objval < inf)
                {
                    sum += x->objval;
                    count++;
                }
            }

            return count == 0 ? value_type(0) : sum / count / count;
        }

        ///
        /// \return The square of the maximum length of the vector connecting
        /// the vertex with the minimum value returned from the objective
        /// function and any other vertex of the simplex.
        ///
        value_type get_length_squared() const
        {
            container_type vec;
            vec.resize(_n);

            value_type max = value_type(0);

            const auto & p0 = _x[0]->params;

            for (size_t i = 1; i <= _n; i++)
            {
                const auto & p = _x[i]->params;

                _subtract(vec, p, p0);
                _multiply(vec, vec, vec);

                max = std::max(max, std::accumulate(
                    vec.begin(),
                    vec.end(),
                    value_type(0),
                    std::plus<value_type>()));
            }

            return max;
        }

        ///
        /// \return The maximum length of the vector connecting the vertex with
        /// the minimum value returned from the objective function and any other
        /// vertex of the simplex.
        ///
        value_type get_length() const
        {
            return std::sqrt(get_length_squared());
        }

        ///
        /// \return The options used for the class.
        ///
        const options & get_options() const
        {
            return _opts;
        }

        ///
        /// \return The statistics about the execution.
        ///
        const stats & get_stats() const
        {
            return _stats;
        }

        ///
        /// \return The minimum objective value.
        ///
        const value_type & get_objval() const
        {
            return _x[0]->objval;
        }

        ///
        /// \return The minimum objective value.
        ///
        const value_type & get_objval(
                const size_t index) ///< The vertex index.
                const
        {
            assert(index <= _n);
            return _x[index]->objval;
        }

        ///
        /// \return The vertex with the minimum value returned from the
        /// objective function.
        ///
        const container_type & get_vertex() const
        {
            return _x[0]->params;
        }

        ///
        /// \return The vertex with the minimum value returned from the
        /// objective function.
        ///
        const container_type & get_vertex(
                const size_t index) ///< The vertex index.
                const
        {
            assert(index <= _n);
            return _x[index]->params;
        }

        ///
        /// Performs one iteration of the Nelder-Mead algorithm. In an
        /// iteration over two-dimensional space, a point p_min is reflected to
        /// point p_r, expanded to point p_e, or contracted to point p_c. If
        /// these test points do not improve the overall score of the simplex,
        /// then it shrinks around the point p_max with the highest score.
        ///
        /// \return The operation performed by the iteration.
        ///
        template <typename TObjfunc>
        operation_type iterate(
            const TObjfunc objfunc) ///< The objective function.
        {
            const auto & x0 = _x[0];

            _stats.iterations++;

            //
            // REFLECTION
            //
            _copy(_xbar, x0->params);
            for (size_t i = 1; i < _n; i++)
                _add(_xbar, _xbar, _x[i]->params);
            _scale(_xbar, value_type(1.0) / value_type(_n));

            _subtract(_xr, _xbar, _x[_n]->params);
            _scale(_xr, _opts.rho);
            _add(_xr, _xr, _xbar);

            const auto fr = _evaluate(objfunc, _xr);
            if (x0->objval <= fr && fr < _x[_n - 1]->objval)
                return _accept(_xr, fr, _stats.reflections,
                    operation::reflection);

            //
            // EXPANSION
            //
            if (fr < x0->objval)
            {
                _subtract(_xe, _xr, _xbar);
                _scale(_xe, _opts.chi);
                _add(_xe, _xe, _xbar);

                const auto fe = _evaluate(objfunc, _xe);
                return fe < fr
                    ? _accept(_xe, fe, _stats.expansions,
                        operation::expansion)
                    : _accept(_xr, fr, _stats.reflections,
                        operation::reflection);
            }

            //
            // CONTRACTION
            //
            if (fr >= _x[_n - 1]->objval)
            {
                if (fr < _x[_n]->objval)
                {
                    _subtract(_xc, _xr, _xbar);
                    _scale(_xc, _opts.gamma);
                    _add(_xc, _xc, _xbar);

                    const auto fc = _evaluate(objfunc, _xc);
                    if (fc <= fr)
                        return _accept(_xc, fc, _stats.contractions_out,
                            operation::contraction_out);
                }
                else
                {
                    _subtract(_xcc, _xbar, _x[_n]->params);
                    _scale(_xcc, _opts.gamma);
                    _subtract(_xcc, _xbar, _xcc);

                    const auto fcc = _evaluate(objfunc, _xcc);
                    if (fcc < _x[_n]->objval)
                        return _accept(_xcc, fcc, _stats.contractions_in,
                            operation::contraction_in);
                }
            }

            //
            // SHRINKAGE
            //
            for (size_t i = 1; i <= _n; i++)
            {
                auto & xi = _x[i];
                _subtract(xi->params, xi->params, x0->params);
                _scale(xi->params, _opts.sigma);
                _add(xi->params, xi->params, x0->params);
                xi->objval = _evaluate(objfunc, xi->params);
            }

            std::sort(_x.begin(), _x.end(), _is_less);
            _stats.shrinkages++;
            return operation::shrinkage;
        }

    private:
        // --------------------------------------------------------------------
        struct vertex
        {
            container_type params;
            value_type     objval;

            // ----------------------------------------------------------------
            vertex(
                    const options & opts,
                    const size_t    index)
                : params (opts.vertex)
                , objval (0)
            {
                if (index > 0)
                    params[index - 1] += opts.unit;
            }
        };

        typedef std::unique_ptr<vertex>      vertex_ptr_type;
        typedef std::vector<vertex_ptr_type> vertex_container_type;

        // --------------------------------------------------------------------
        operation_type _accept(
                const container_type & params,
                const value_type       objval,
                size_t &               stat,
                const operation_type   type)
        {
            //
            // Accepts a new vertex into the simplex by replacing the vertex
            // with the highest value returned from the objective function and
            // then resorting.
            //
            const auto n = params.size();
            auto & v = _x[n];
            _copy(v->params, params);
            v->objval = objval;
            std::sort(_x.begin(), _x.end(), _is_less);
            stat++;
            return type;
        }

        // --------------------------------------------------------------------
        static void _add(
                container_type &       dst,
                const container_type & lhs,
                const container_type & rhs)
        {
            assert(dst.size() == lhs.size());
            assert(lhs.size() == rhs.size());
            std::transform(lhs.begin(), lhs.end(), rhs.begin(), dst.begin(),
                std::plus<value_type>());
        }

        // --------------------------------------------------------------------
        static bool _is_less(
                const vertex_ptr_type & lhs,
                const vertex_ptr_type & rhs)
        {
            return lhs->objval < rhs->objval;
        }

        // --------------------------------------------------------------------
        static void _copy(
                container_type &       dst,
                const container_type & src)
        {
            assert(dst.size() == src.size());
            std::copy(src.begin(), src.end(), dst.begin());
        }

        // --------------------------------------------------------------------
        template <typename TObjfunc>
        value_type _evaluate(
                const TObjfunc         objfunc,
                const container_type & params)
        {
            _stats.evaluations++;
            return objfunc(params);
        }

        // --------------------------------------------------------------------
        static void _multiply(
                container_type &       dst,
                const container_type & lhs,
                const container_type & rhs)
        {
            assert(dst.size() == lhs.size());
            assert(dst.size() == rhs.size());
            std::transform(lhs.begin(), lhs.end(), rhs.begin(), dst.begin(),
                std::multiplies<value_type>());
        }

        // --------------------------------------------------------------------
        static void _scale(
                container_type & dst,
                const value_type k)
        {
            for (auto & n : dst)
                n *= k;
        }

        // --------------------------------------------------------------------
        static void _subtract(
                container_type &       dst,
                const container_type & lhs,
                const container_type & rhs)
        {
            assert(dst.size() == lhs.size());
            assert(dst.size() == rhs.size());
            std::transform(lhs.begin(), lhs.end(), rhs.begin(), dst.begin(),
                std::minus<value_type>());
        }

        size_t                _n;     // number of dimensions
        options               _opts;  // algorithm options
        stats                 _stats; // execution stats
        vertex_container_type _x;     // the vertices of the simplex
        container_type        _xbar;  // centroid excluding x[n]
        container_type        _xr;    // reflection vertex
        container_type        _xc;    // outside contraction vertex
        container_type        _xcc;   // inside contraction vertex
        container_type        _xe;    // expansion vertex
    };
}

///
/// Writes a simplex to the specified output stream.
/// \return The output stream.
///
template <typename TValue>
std::ostream & operator << (
        std::ostream &                      lhs, ///< The output stream.
        const jade::basic_simplex<TValue> & rhs) ///< The simplex.
{
    return lhs
        << rhs.get_stats().str()
        << "delta:            " << rhs.get_delta() << std::endl
        << "flux:             " << rhs.get_flux() << std::endl
        << "length:           " << rhs.get_length() << std::endl;
}

#endif // JADE_SIMPLEX_HPP__
