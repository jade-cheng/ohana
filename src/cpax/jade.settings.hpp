/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SETTINGS_HPP__
#define JADE_SETTINGS_HPP__

#include "jade.forced_grouping.hpp"
#include "jade.genotype_matrix_factory.hpp"
#include "jade.options.hpp"

namespace jade
{
    ///
    /// A template for a class encapsulating the settings provided to the
    /// optimizer.
    ///
    template <typename TValue>
    class basic_settings
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The matrix type.
        typedef basic_matrix<value_type> matrix_type;

        /// The forced grouping type.
        typedef basic_forced_grouping<value_type> forced_grouping_type;

        /// The genotype matrix.
        typedef basic_genotype_matrix<value_type> genotype_matrix_type;

        /// The options type.
        typedef basic_options<value_type> options_type;

        /// The randomizer type.
        typedef basic_randomizer<value_type> randomizer_type;

        /// The verification type.
        typedef basic_verification<value_type> verification_type;

        /// The forced grouping pointer type.
        typedef std::unique_ptr<forced_grouping_type> fg_ptr;

        /// The Fin-force pointer type.
        typedef std::unique_ptr<matrix_type> fif_ptr;

        /// The genotype matrix pointer type.
        typedef std::unique_ptr<genotype_matrix_type> g_ptr;

        /// The genotype matrix factory type.
        typedef basic_genotype_matrix_factory<value_type>
            g_matrix_factory_type;

        ///
        /// Initializes a new instance of the class based on the specified
        /// arguments.
        ///
        explicit basic_settings(
                args & a) ///< The command-line arguments.
            : _opts (a)
            , _g    (g_matrix_factory_type::create(a.pop<std::string>()))
            , _q    ()
            , _f    ()
            , _fif  ()
            , _fg   ()
            , _rnd  ()
        {
            verification_type::validate_g(*_g);

            a.validate_empty();

            if (_opts.is_qin_specified())
            {
                _q.read(_opts.get_qin());
                verification_type::validate_q(_q);
            }

            if (_opts.is_fin_specified())
            {
                _f.read(_opts.get_fin());
                verification_type::validate_f(_f);
            }

            if (_opts.is_fin_force_specified())
            {
                _fif.reset(new matrix_type(_opts.get_fin_force()));
                verification_type::validate_f(*_fif);
            }

            if (_opts.is_force_specified())
                _fg.reset(new forced_grouping_type(_opts.get_force()));

            const auto n = _g->get_height();
            const auto k = _opts.is_qin_specified() ? _q.get_width() :
                _opts.is_fin_specified() ? _f.get_height() :
                _opts.is_force_specified() ? _fg->get_k() :
                _opts.get_ksize();

            if (_fif)
                verification_type::validate_fif_size(
                    *_fif, k, _g->get_width());

            _rnd.get_engine().seed(_opts.get_seed());

            if (!_opts.is_qin_specified())
                _q = _opts.is_force_specified()
                        ? _fg->randomize_q(_rnd)
                        : _rnd.randomize_q(n ,k);

            if (_opts.is_fin_force_specified())
                _f = _rnd.randomize_f(k, _g->create_mu(), *_fif);
            else if (!_opts.is_fin_specified())
                _f = _rnd.randomize_f(k, _g->create_mu());

            verification_type::validate_gqf_sizes(*_g, _q, _f);

            if (_opts.is_force_specified())
                _fg->validate_q(_q);
        }

        ///
        /// \return The F matrix.
        ///
        inline const matrix_type & get_f() const
        {
            return _f;
        }

        ///
        /// \return The F matrix.
        ///
        inline matrix_type & get_f()
        {
            return _f;
        }

        ///
        /// \return The Fin-force matrix.
        ///
        inline const matrix_type * get_fif() const
        {
            return _fif.get();
        }

        ///
        /// \return A pointer to the forced grouping file or nullptr if the
        /// information is not available.
        ///
        inline const forced_grouping_type * get_fg() const
        {
            return _fg.get();
        }

        ///
        /// \return The genotype matrix.
        ///
        inline const genotype_matrix_type & get_g() const
        {
            return *_g;
        }

        ///
        /// \return The options.
        ///
        inline const options_type & get_options() const
        {
            return _opts;
        }

        ///
        /// \return The Q matrix.
        ///
        inline const matrix_type & get_q() const
        {
            return _q;
        }

        ///
        /// \return The Q matrix.
        ///
        inline matrix_type & get_q()
        {
            return _q;
        }

        ///
        /// \return The randomizer.
        ///
        inline const randomizer_type & get_randomizer() const
        {
            return _rnd;
        }

    private:
        options_type    _opts;
        g_ptr           _g;
        matrix_type     _q;
        matrix_type     _f;
        fif_ptr         _fif;
        fg_ptr          _fg;
        randomizer_type _rnd;
    };
}

#endif // JADE_SETTINGS_HPP__
