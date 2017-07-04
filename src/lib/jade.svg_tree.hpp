/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_SVG_HPP__
#define JADE_SVG_HPP__

#include "jade.newick.hpp"
#include "jade.simplex.hpp"
#include "jade.vec2.hpp"

namespace jade
{
    ///
    /// A template for a class that renders newick trees as SVG.
    ///
    template <typename TValue>
    class basic_svg_tree
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The Newick node type.
        typedef basic_newick_node<value_type> node_type;

        /// The vector type.
        typedef basic_vec2<value_type> vec2_type;

        ///
        /// Initializes a new instance of the class based on the specified
        /// node.
        ///
        explicit basic_svg_tree(
                const node_type & node) ///< The newick node.
            : _root  (node.get_id())
            , _table ()
        {
            _init_table(node);
            _update_table(node);
        }

        ///
        /// Optimizes the positions of the vertices using the Nelder-Mead
        /// Simplex method.
        ///
        void optimize_positions()
        {
            typedef basic_simplex<value_type>             simplex_type;
            typedef typename simplex_type::container_type container_type;
            typedef typename simplex_type::execute_args   execute_args_type;
            typedef typename simplex_type::options        options_type;

            //
            // Updates the radians and positions of each node based on a
            // specified Nelder-Mead parameter set.
            //
            const auto import_params = [this](
                    const container_type & params) -> void
            {
                auto dst = _table.begin();
                for (auto src = params.begin(); src != params.end(); ++src)
                    (dst++)->second.radians = *src;
                _update_table(*_table.find(_root)->second.node);
            };

            //
            // Computes the objective function for the Nelder-Mead method. The
            // value returned is the sum of the inverses of the distances
            // between all pairs of distinct nodes.
            //
            const auto objfunc = [this, import_params](
                    const container_type & params) -> value_type
            {
                import_params(params);
                auto sum = value_type(0);
                for (const auto a0 : _table)
                for (const auto b0 : _table)
                if (a0.first != b0.first)
                {
                    const auto a = a0.second.position;
                    const auto b = b0.second.position;
                    const auto d = vec2_type::distance_squared(a, b);
                    if (d < 1e-6) // epsilon
                        return std::numeric_limits<value_type>::max();
                    sum += value_type(1) / d;
                }
                return sum;
            };

            //
            // Converts a specified number of degrees to radians.
            //
            const auto to_radians = [](const value_type deg) -> value_type
            {
                static const auto tau = value_type(2.0 * std::acos(-1.0));
                return deg * tau / value_type(360.0);
            };

            const auto n = _table.size();

            //
            // Construct the simplex using a unit size of one degree and the
            // current positions of the nodes.
            //
            options_type opts (n);
            opts.unit = to_radians(1);
            opts.vertex.clear();
            for (const auto & p : _table)
                opts.vertex.push_back(p.second.radians);
            simplex_type simplex (objfunc, opts);

            //
            // Execute the Nelder-Mead method until the simplex has collapsed
            // or a maximum timeout occurs; execute at least a minimum number
            // of iterations.
            //
            execute_args_type exe_args_1;
            exe_args_1.max_iterations = 100;
            exe_args_1.max_seconds    = 0.5;
            simplex.execute(objfunc, exe_args_1);

            execute_args_type exe_args_2;
            exe_args_2.min_length  = to_radians(0.01) * value_type(n);
            exe_args_2.max_seconds = 0.5;
            simplex.execute(objfunc, exe_args_2);

            //
            // Import the final parameters from the simplex.
            //
            import_params(simplex.get_vertex());
        }

        ///
        /// Returns the SVG scene as a string.
        ///
        /// \return The SVG scene as a string.
        ///
        std::string str() const
        {
            std::ostringstream out;
            write(out);
            return out.str();
        }

        ///
        /// Writes the SVG scene to the specified output stream.
        ///
        void write(
                std::ostream & out) ///< The output stream.
                const
        {
            const auto & root  = *_table.find(_root)->second.node;
            metrics_data metrics;
            _create_metrics(metrics);

            _write_svg_header(out, metrics);
            _write_edges(out, metrics, root);
            _write_nodes(out, metrics, root);
            _write_svg_footer(out);
        }

        ///
        /// Writes the SVG scene to the specified output file.
        ///
        void write(
                char const * const path) ///< The path to the output file.
                const
        {
            assert(path != nullptr);
            std::ofstream out (path);
            if (!out.good())
                throw error() << "error opening '" << path << "' for writing";
            write(out);
        }

        ///
        /// Writes the SVG scene to the specified output file.
        ///
        void write(
                const std::string & path) ///< The path to the output file.
                const
        {
            write(path.c_str());
        }

    private:
        // --------------------------------------------------------------------
        struct node_ex
        {
            const node_type * node;
            value_type        length;
            value_type        radians;
            vec2_type         position;

            // ----------------------------------------------------------------
            inline node_ex()
                : node     (nullptr)
                , length   ()
                , radians  ()
                , position ()
            {
            }
        };

        // --------------------------------------------------------------------
        struct metrics_data
        {
            value_type font_size;
            value_type large_radius;
            value_type small_radius;
            value_type stroke_width;
            value_type text_offset;
            value_type scale;
            value_type view_box_height;
            value_type view_box_left;
            value_type view_box_top;
            value_type view_box_width;
        };

        // --------------------------------------------------------------------
        struct theme
        {
            static constexpr double       opacity             = 0.75;
            static constexpr char const * circle_fill_color   = "black";
            static constexpr char const * circle_stroke_color = "white";
            static constexpr char const * text_color          = "white";
            static constexpr char const * text_outline_color  = "black";
            static constexpr char const * line_color          = "black";
            static constexpr char const * font_family         = "Times,serif";
        };

        typedef std::map<int, node_ex> table_type;

        // --------------------------------------------------------------------
        void _create_metrics(metrics_data & metrics) const
        {
            typedef std::numeric_limits<value_type> numeric_limits_type;

            memset(&metrics, 0, sizeof(metrics));

            auto iter = _table.begin();
            if (iter == _table.end())
                return;

            metrics.scale = value_type(1);

            const auto is_measured = [](const node_ex & n) -> bool
            {
                return n.node->has_length() && n.node->has_name();
            };

            auto shortest_edge = numeric_limits_type::quiet_NaN();
            if (is_measured(iter->second))
                shortest_edge = iter->second.length;

            auto min = iter->second.position;
            auto max = iter->second.position;

            if (++iter == _table.end())
            {
                metrics.large_radius = value_type(100);
                metrics.scale        = value_type(1);
            }
            else
            {
                while (iter != _table.end())
                {
                    if (is_measured(iter->second))
                    {
                        const auto shortest_edge_i =
                            iter->second.length;

                        if (shortest_edge_i > value_type(0))
                            if (std::isnan(shortest_edge)
                                || shortest_edge_i < shortest_edge)
                                shortest_edge = shortest_edge_i;
                    }

                    min = vec2_type::min(min, iter->second.position);
                    max = vec2_type::max(max, iter->second.position);
                    ++iter;
                }

                if (std::isnan(shortest_edge))
                {
                    metrics.large_radius = value_type(100);
                    metrics.scale        = value_type(1);
                }
                else
                {
                    auto diameter = vec2_type::distance(min, max);

                    if (diameter < value_type(0.000001))
                    {
                        metrics.large_radius = value_type(100);
                        metrics.scale        = value_type(1);
                    }
                    else
                    {
                        while (diameter < value_type(100))
                        {
                            metrics.scale *= value_type(10);
                            diameter *= value_type(10);
                        }

                        while (diameter > value_type(1000))
                        {
                            metrics.scale /= value_type(10);
                            diameter /= value_type(10);
                        }

                        metrics.large_radius = metrics.scale *
                            shortest_edge / value_type(3);
                    }
                }
            }

            const auto diameter = metrics.large_radius + metrics.large_radius;
            min = metrics.scale * min - diameter;
            max = metrics.scale * max + diameter;

            metrics.font_size       = metrics.large_radius * value_type(0.90);
            metrics.small_radius    = metrics.large_radius * value_type(0.30);
            metrics.stroke_width    = metrics.large_radius * value_type(0.15);
            metrics.text_offset     = metrics.large_radius * value_type(0.30);
            metrics.view_box_left   = min.x;
            metrics.view_box_top    = min.y;
            metrics.view_box_width  = (max - min).x;
            metrics.view_box_height = (max - min).y;
            return;
        }

        // --------------------------------------------------------------------
        void _find_edge_coords(
            const metrics_data & metrics,
            const node_type &    n1,
            const node_type &    n2,
            vec2_type &          p1,
            vec2_type &          p2)
            const
        {
            const auto & n1ex = _table.find(n1.get_id())->second;
            const auto & n2ex = _table.find(n2.get_id())->second;

            p1 = metrics.scale * n1ex.position;
            p2 = metrics.scale * n2ex.position;

            const auto n = vec2_type::normalize(p2 - p1);

            p1 += n * (n1ex.node->get_name().empty()
                ? metrics.small_radius
                : metrics.large_radius);

            p2 -= n * (n2ex.node->get_name().empty()
                ? metrics.small_radius
                : metrics.large_radius);
        }

        // --------------------------------------------------------------------
        void _init_table(const node_type & node)
        {
            static const auto tau = value_type(2.0 * std::acos(-1.0));
            _init_table(node, 0, tau);
        }

        // --------------------------------------------------------------------
        void _init_table(
                const node_type & node,
                const value_type radians,
                const value_type range)
        {
            node_ex ex;
            ex.node    = &node;
            ex.radians = radians;
            ex.length  = std::max(value_type(0), node.get_length());

            const auto id = node.get_id();
            _table[id] = ex;

            const auto & children = node.get_children();
            const auto   n        = children.size();
            const auto   nval     = value_type(n);
            const auto   half     = value_type(0.5);
            for (size_t i = 0; i < n; i++)
            {
                const auto ival     = value_type(i);
                const auto percent  = (half + ival) / nval;
                const auto radians2 = (range * percent) - (half * range);
                const auto range2   = range / nval;
                _init_table(*children[i], radians2, range2);
            }
        }

        // --------------------------------------------------------------------
        void _update_table(const node_type & node)
        {
            _update_table(node, 0, vec2_type());
        }

        // --------------------------------------------------------------------
        void _update_table(
            const node_type & node,
            const value_type  radians0,
            const vec2_type & p0)
        {
            const auto id = node.get_id();
            auto & ex = _table[id];

            const auto rad = radians0 + ex.radians;

            ex.position = p0 + ex.length * vec2_type(
                std::cos(rad),
                std::sin(rad));

            for (const auto ptr : node.get_children())
                _update_table(*ptr, rad, ex.position);
        }

        // --------------------------------------------------------------------
        void _write_edges(
                std::ostream &       out,
                const metrics_data & metrics,
                const node_type &    node)
                const
        {
            const auto stroke_width = metrics.stroke_width;

            for (const auto child : node.get_children())
            {
                vec2_type p1, p2;
                _find_edge_coords(metrics, node, *child, p1, p2);
                _write_svg_line(out, p1.x, p1.y, p2.x, p2.y, stroke_width);
            }

            for (const auto child : node.get_children())
                _write_edges(out, metrics, *child);
        }

        // --------------------------------------------------------------------
        void _write_nodes(
                std::ostream &       out,
                const metrics_data & metrics,
                const node_type &    node)
                const
        {
            const auto stroke_width = value_type(0.5) * metrics.stroke_width;
            const auto parent_id = node.get_id();
            const auto & parent_ex = _table.find(parent_id)->second;
            const auto position = metrics.scale * parent_ex.position;

            const auto radius = node.get_name().empty() ?
                metrics.small_radius :
                metrics.large_radius;

            out << "  <g>\n";

            _write_svg_circle(out, position.x, position.y,
                radius, stroke_width);

            if (!node.get_name().empty())
            {
                const auto x         = position.x;
                const auto y         = position.y + metrics.text_offset;
                const auto font_size = metrics.font_size;
                const auto text      = node.get_name().c_str();
                _write_svg_text(out, x, y, font_size, text, true);
                _write_svg_text(out, x, y, font_size, text, false);
            }

            out << "  </g>\n";

            for (const auto child : node.get_children())
                _write_nodes(out, metrics, *child);
        }

        // --------------------------------------------------------------------
        static void _write_svg_circle(
            std::ostream &   out,
            const value_type cx,
            const value_type cy,
            const value_type r,
            const value_type stroke_width)
        {
            out << "    <circle "
                << "cx=\"" << cx << "px\" "
                << "cy=\"" << cy << "px\" "
                << "r=\"" << r << "px\" "
                << "style=\""
                << "fill:" << theme::circle_fill_color << ";"
                << "fill-opacity:" << theme::opacity << ";"
                << "stroke:" << theme::circle_stroke_color << ";"
                << "stroke-width:" << stroke_width << "px\">"
                << "</circle>\n";
        }

        // --------------------------------------------------------------------
        static void _write_svg_footer(
                std::ostream & out)
        {
            out << "</svg>\n";
        }

        // --------------------------------------------------------------------
        static void _write_svg_header(
                std::ostream &       out,
                const metrics_data & metrics)
        {
            out << "<svg "
                << "xmlns=\"http://www.w3.org/2000/svg\" "
                << "version=\"1.1\" "
                << "viewBox=\""
                << metrics.view_box_left << " "
                << metrics.view_box_top << " "
                << metrics.view_box_width << " "
                << metrics.view_box_height << "\">\n";
        }

        // --------------------------------------------------------------------
        static void _write_svg_line(
            std::ostream &   out,
            const value_type x1,
            const value_type y1,
            const value_type x2,
            const value_type y2,
            const value_type stroke_width)
        {
            out << "  <line "
                << "x1=\"" << x1 << "px\" "
                << "y1=\"" << y1 << "px\" "
                << "x2=\"" << x2 << "px\" "
                << "y2=\"" << y2 << "px\" "
                << "style=\""
                << "stroke-opacity:" << theme::opacity << ";"
                << "stroke-linecap:butt;"
                << "stroke:" << theme::line_color << ";"
                << "stroke-width:" << stroke_width << "px\">"
                << "</line>\n";
        }

        // --------------------------------------------------------------------
        static void _write_svg_text(
            std::ostream &     out,
            const value_type   x,
            const value_type   y,
            const value_type   font_size,
            char const * const text,
            bool               is_outlined)
        {
            out << "    <text "
                << "x=\"" << x << "px\" "
                << "y=\"" << y << "px\" "
                << "style=\"";

            if (is_outlined)
                out << "fill:" << theme::text_outline_color << ";"
                    << "fill-opacity:" << theme::opacity << ";"
                    << "stroke-opacity:" << theme::opacity << ";"
                    << "stroke:" << theme::text_outline_color << ";"
                    << "stroke-width:" << font_size / value_type(7) << "px;";
            else
                out << "fill:" << theme::text_color << ";";

            out << "text-anchor:middle;"
                << "font-size:" << font_size << "px;"
                << "font-family:" << theme::font_family << "\">";

            for (char const * ptr = text; *ptr != '\0'; ptr++)
                if (std::isprint(*ptr))
                    out << *ptr;

            out << "</text>\n";
        }

        int        _root;
        table_type _table;
    };
}

#endif // JADE_SVG_HPP__
