/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SHUNTING_YARD_HPP__
#define JADE_SHUNTING_YARD_HPP__

#include "jade.error.hpp"

namespace jade
{
    ///
    /// A template for a class that implements the shunting-yard algorithm. The
    /// implementation supports '+', '-', '*', and '/' operators; the '(' and
    /// ')' parentheses; floating-point numbers; and variables.  After parsing
    /// an expression, the instance can evaluate the expression if given a
    /// table of variable values.
    ///
    template <typename TFloat>
    class basic_shunting_yard
    {
    public:
        typedef TFloat float_type; ///< The floating-point type.

        ///
        /// A map of strings to floating-point values. The string represent the
        /// variable names parsed from the expression passed to the
        /// constructor, and the floating-point values are used to evaluate the
        /// expression.
        ///
        typedef std::map<std::string, float_type> args_type;

        ///
        /// Initializes a new instance of the class. The specified input stream
        /// provides tokens for the expression.
        /// \param in The input stream providing tokens.
        ///
        explicit basic_shunting_yard(std::istream & in)
            : _args  ()
            , _queue ()
        {
            std::vector<_token> tokens;
            _scan(in, tokens);
            _enqueue(tokens);

            for (const auto & t : _queue)
                if (t.type == _token::variable)
                    _args[t.text] = float_type(0);

            evaluate(_args);
        }

        ///
        /// Initializes a new instance of the class. The specified string
        /// provides tokens for the expression.
        /// \param expression The string providing tokens.
        ///
        explicit basic_shunting_yard(const std::string & expression)
            : _args  ()
            , _queue ()
        {
            std::istringstream in (expression);
            *this = basic_shunting_yard(in);
        }

        ///
        /// Returns a table of arguments that maps variables to values. The
        /// table reference returned has all values set to zero. The table can
        /// be used to evaluate the expression.
        /// \return A table mapping variables to values.
        ///
        const args_type & get_args() const
        {
            return _args;
        }

        ///
        /// Evaluates the expression using values from the specified table.
        /// \param args The table of variable values.
        /// \return The output from the expression.
        ///
        float_type evaluate(const args_type & args = args_type()) const
        {
            std::vector<float_type> stack;

            for (const auto & t : _queue)
            {
                if (t.type == _token::number)
                {
                    stack.push_back(t.get_value());
                }
                else if (t.type == _token::variable)
                {
                    const auto item = args.find(t.text);

                    if (item == args.end())
                        throw jade::error()
                            << "undefined variable '"
                            << t.text
                            << "'";

                    stack.push_back(item->second);
                }
                else
                {
                    if (stack.size() < 2)
                        throw jade::error("invalid expression");

                    const auto n2 = stack.back();
                    stack.pop_back();
                    const auto n1 = stack.back();
                    stack.pop_back();

                    if      (t.type == _token::plus ) stack.push_back(n1 + n2);
                    else if (t.type == _token::minus) stack.push_back(n1 - n2);
                    else if (t.type == _token::star ) stack.push_back(n1 * n2);
                    else if (t.type == _token::slash) stack.push_back(n1 / n2);
                    else throw jade::error("invalid operator");
                }
            }

            if (stack.size() != 1)
                throw jade::error("invalid expression");

            return stack[0];
        }

        ///
        /// Evaluates the expression using values from the specified table.
        /// \param args The table of variable values.
        /// \return The output from the expression.
        ///
        inline float_type operator () (
                const args_type & args = args_type())
                const
        {
            return evaluate(args);
        }

    private:
        // --------------------------------------------------------------------
        struct _token
        {
            // ----------------------------------------------------------------
            enum type_t
            {
                end,
                minus,
                number,
                paren_lhs,
                paren_rhs,
                plus,
                slash,
                star,
                variable
            };

            // ----------------------------------------------------------------
            explicit _token(const type_t type_)
                : type (type_)
                , text ()
            {
            }

            // ----------------------------------------------------------------
            int get_precedence() const
            {
                return type == plus || type == minus ? 1 :
                       type == star || type == slash ? 2 :
                       0;
            }

            // ----------------------------------------------------------------
            float_type get_value() const
            {
                float_type value;
                std::istringstream(text) >> value;
                return value;
            }

            // ----------------------------------------------------------------
            bool is_numeric() const
            {
                return type == variable || type == number;
            }

            // ----------------------------------------------------------------
            bool is_operator() const
            {
                return type == plus || type == minus
                    || type == star || type == slash;
            }

            type_t      type;
            std::string text;
        };

        // --------------------------------------------------------------------
        void _enqueue(const std::vector<_token> & tokens)
        {
            std::vector<_token> stack;

            for (size_t i = 0; i < tokens.size(); i++)
            {
                const auto & t1 = tokens[i];

                if (t1.is_numeric())
                {
                    _queue.push_back(t1);
                }
                else if (t1.is_operator())
                {
                    while (!stack.empty())
                    {
                        const auto & t2 = stack.back();

                        if (!t2.is_operator())
                            break;
                        if (t1.get_precedence() > t2.get_precedence())
                            break;

                        _queue.push_back(t2);
                        stack.pop_back();
                    }

                    stack.push_back(t1);
                }
                else if (t1.type == _token::paren_lhs)
                {
                    stack.push_back(t1);
                }
                else if (t1.type == _token::paren_rhs)
                {
                    while (!stack.empty())
                    {
                        const auto & t2 = stack.back();

                        if (t2.type == _token::paren_lhs)
                            break;

                        _queue.push_back(t2);
                        stack.pop_back();
                    }

                    if (stack.empty())
                        throw jade::error("mismatched parentheses");

                    stack.pop_back();
                }
            }

            while (!stack.empty())
            {
                const auto & t2 = stack.back();

                if (t2.type == _token::paren_lhs)
                    throw jade::error("mismatched parentheses");

                _queue.push_back(t2);
                stack.pop_back();
            }
        }

        // --------------------------------------------------------------------
        static void _scan(std::istream & in, std::vector<_token> & tokens)
        {
            const auto cat = [&tokens](const int ch)
            {
                tokens.back().text.push_back(static_cast<char>(ch));
            };

            for (auto ch = in.get(); ch >= 0; ch = in.get())
            {
                if (std::isspace(ch)) continue;
                else if (ch == '(') tokens.emplace_back(_token::paren_lhs);
                else if (ch == ')') tokens.emplace_back(_token::paren_rhs);
                else if (ch == '+') tokens.emplace_back(_token::plus);
                else if (ch == '-') tokens.emplace_back(_token::minus);
                else if (ch == '*') tokens.emplace_back(_token::star);
                else if (ch == '/') tokens.emplace_back(_token::slash);
                else if (std::isalpha(ch))
                {
                    tokens.emplace_back(_token::variable);
                    cat(ch);

                    while (in.peek() == '_' || 0 != std::isalnum(in.peek()))
                        cat(in.get());
                }
                else if (std::isdigit(ch))
                {
                    tokens.emplace_back(_token::number);
                    cat(ch);

                    while (std::isdigit(in.peek()))
                        cat(in.get());

                    if (in.peek() == '.')
                    {
                        cat(in.get());

                        while (std::isdigit(in.peek()))
                            cat(in.get());
                    }
                }
                else
                {
                    throw jade::error()
                        << "invalid symbol '"
                        << static_cast<char>(ch)
                        << "'";
                }
            }

            tokens.emplace_back(_token::end);
        }

        args_type           _args;  // A default set of arguments.
        std::vector<_token> _queue; // The tokens in reverse polish notation.
    };
}

#endif // JADE_SHUNTING_YARD_HPP__
