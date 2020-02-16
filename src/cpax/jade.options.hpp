/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_OPTIONS_HPP__
#define JADE_OPTIONS_HPP__

#include "jade.args.hpp"

namespace jade
{
    ///
    /// A template for a class encapsulating the program options.
    ///
    template <typename TValue>
    class basic_options
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The random number generator seed type.
        typedef std::random_device::result_type seed_type;

        /// The value assigned for no --epsilon option.
        static constexpr auto no_epsilon =
            std::numeric_limits<value_type>::quiet_NaN();

        /// The value assigned for no --ksize option.
        static constexpr auto no_ksize =
            std::numeric_limits<size_t>::max();

        /// The value assigned for no --max-iterations option.
        static constexpr auto no_max_iterations =
            std::numeric_limits<size_t>::max();

        /// The value assigned for no --max-time option.
        static constexpr auto no_max_time =
            std::numeric_limits<double>::quiet_NaN();

        ///
        /// Initializes a new instance of the class.
        ///
        explicit basic_options(
                args & a) ///< The command-line arguments.
            : _epsilon        (a.read("--epsilon", "-e", no_epsilon))
            , _fin            (a.read<std::string>("--fin", "-fi"))
            , _fin_force      (a.read<std::string>("--fin-force", "-fif"))
            , _force          (a.read<std::string>("--force", "-fg"))
            , _fout           (a.read<std::string>("--fout",  "-fo"))
            , _ksize          (a.read("--ksize", "-k", no_ksize))
            , _max_iterations (a.read("--max-iterations", "-mi", no_max_iterations))
            , _max_time       (a.read("--max-time", "-mt", no_max_time))
            , _qin            (a.read<std::string>("--qin", "-qi"))
            , _qout           (a.read<std::string>("--qout", "-qo"))
            , _seed           (a.read("--seed", "-s", std::random_device()()))
            , _frb            (a.read_flag("--frequency-bounds", "-frb"))
            , _fixed_f        (a.read_flag("--fixed-f", "-ff"))
            , _fixed_q        (a.read_flag("--fixed-q", "-fq"))
            , _quiet          (a.read_flag("--quiet", "-q"))
        {
            if (is_epsilon_specified() && _epsilon < value_type(0))
                throw error()
                    << "invalid value for --epsilon option: "
                    << _epsilon;

            if (is_ksize_specified() && _ksize < 2)
                throw error()
                    << "invalid value for --ksize option: "
                    << _ksize;

            if (is_max_time_specified() && _max_time < 0.0)
                throw error()
                    << "invalid value for --max-time option: "
                    << _max_time;

            if (!is_ksize_specified() &&
                !is_qin_specified() &&
                !is_fin_specified() &&
                !is_force_specified())
            {
                throw error()
                    << "at least one of --ksize, --qin, --fin, or --force must "
                    << "be specified in order to determine the number of "
                    << "components";
            }

            if (is_fin_specified() && is_fin_force_specified())
                throw error() << "invalid specification of --fin option "
                              << "with --fin-force option";

            if (!is_fin_specified() && _fixed_f)
                throw error() << "invalid specification of --fixed-f option "
                              << "without --fin option";

            if (!is_qin_specified() && _fixed_q)
                throw error() << "invalid specification of --fixed-q option "
                              << "without --qin option";

            if (is_force_specified() && _fixed_q)
                throw error() << "invalid specification of --fixed-q option "
                              << "and --force option";

            if (is_frb())
            {
                if (is_fin_force_specified())
                    throw error()
                        << "invalid specification of --fin-force "
                        << "and --frequency-bounds options";

                if (is_fixed_f() && is_fin_specified())
                    throw error()
                        << "invalid specification of --fixed-f, --fin, "
                        << "and --frequency-bounds options";
            }
        }

        ///
        /// \return The epsilon value.
        ///
        inline value_type get_epsilon() const
        {
            assert(is_epsilon_specified());
            return _epsilon;
        }

        ///
        /// \return The fin value.
        ///
        inline const std::string & get_fin() const
        {
            assert(is_fin_specified());
            return _fin;
        }

        ///
        /// \return The fin-force value.
        ////
        inline const std::string & get_fin_force() const
        {
            assert(is_fin_force_specified());
            return _fin_force;
        }

        ///
        /// \return The force value.
        ///
        inline const std::string & get_force() const
        {
            assert(is_force_specified());
            return _force;
        }

        ///
        /// \return The fout value.
        ///
        inline const std::string & get_fout() const
        {
            assert(is_fout_specified());
            return _fout;
        }

        ///
        /// \return The ksize value.
        ///
        inline size_t get_ksize() const
        {
            assert(is_ksize_specified());
            return _ksize;
        }

        ///
        /// \return The maximum iterations value.
        ///
        inline size_t get_max_iterations() const
        {
            assert(is_max_iterations_specified());
            return _max_iterations;
        }

        ///
        /// \return The maximum time value.
        ///
        inline double get_max_time() const
        {
            assert(is_max_time_specified());
            return _max_time;
        }

        ///
        /// \return The qin value.
        ///
        inline const std::string & get_qin() const
        {
            assert(is_qin_specified());
            return _qin;
        }

        ///
        /// \return The qout value.
        ///
        inline const std::string & get_qout() const
        {
            assert(is_qout_specified());
            return _qout;
        }

        ///
        /// \return The seed value.
        ///
        inline seed_type get_seed() const
        {
            return _seed;
        }

        ///
        /// \return True if the epsilon option is specified.
        ///
        inline bool is_epsilon_specified() const
        {
            return !std::isnan(_epsilon);
        }

        ///
        /// \return True if the frequency-bounds option is specified.
        ///
        inline bool is_frb() const
        {
            return _frb;
        }

        ///
        /// \return True if the fin option is specified.
        ///
        inline bool is_fin_specified() const
        {
            return !_fin.empty();
        }

        ///
        /// \return True if the fin-force option is specified.
        ///
        inline bool is_fin_force_specified() const
        {
            return !_fin_force.empty();
        }

        ///
        /// \return The fixed F value.
        ///
        inline bool is_fixed_f() const
        {
            return _fixed_f;
        }

        ///
        /// \return The fixed Q value.
        ///
        inline bool is_fixed_q() const
        {
            return _fixed_q;
        }

        ///
        /// \return True if the force option is specified.
        ///
        inline bool is_force_specified() const
        {
            return !_force.empty();
        }

        ///
        /// \return True if the fout option is specified.
        ///
        inline bool is_fout_specified() const
        {
            return !_fout.empty();
        }

        ///
        /// \return True if the ksize option is specified.
        ///
        inline bool is_ksize_specified() const
        {
            return _ksize != no_ksize;
        }

        ///
        /// \return True if the maximum iterations option is specified.
        ///
        inline bool is_max_iterations_specified() const
        {
            return _max_iterations != no_max_iterations;
        }

        ///
        /// \return True if the maximum time option is specified.
        ///
        inline bool is_max_time_specified() const
        {
            return !std::isnan(_max_time);
        }

        ///
        /// \return True if the qin option is specified.
        ///
        inline bool is_qin_specified() const
        {
            return !_qin.empty();
        }

        ///
        /// \return True if the qout option is specified.
        ///
        inline bool is_qout_specified() const
        {
            return !_qout.empty();
        }

        ///
        /// \return The quiet value.
        ///
        inline bool is_quiet() const
        {
            return _quiet;
        }

    private:
        // options with arguments
        const value_type  _epsilon;
        const std::string _fin;
        const std::string _fin_force;
        const std::string _force;
        const std::string _fout;
        const size_t      _ksize;
        const size_t      _max_iterations;
        const double      _max_time;
        const std::string _qin;
        const std::string _qout;
        const seed_type   _seed;

        // options without arguments
        const bool _frb;
        const bool _fixed_f;
        const bool _fixed_q;
        const bool _quiet;
    };
}

#endif // JADE_OPTIONS_HPP__
