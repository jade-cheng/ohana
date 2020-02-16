/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "test.main.hpp"
#include "jade.newick.hpp"

namespace
{
    typedef double value_type;
    typedef jade::basic_newick_node<value_type> node_type;

    const auto epsilon = value_type(0.0001);

    // ------------------------------------------------------------------------
    void constructor()
    {
        {
            node_type node;
            TEST_EQUAL(
                std::string(";"),
                node.str());
        }

        {
            std::istringstream in (" ( a : 1 , ( b , c : 2.3 ) d ) e ; ");
            node_type node (in);
            TEST_EQUAL(
                std::string("(a:1,(b,c:2.3)d)e;"),
                node.str());
        }

        {
            const node_type node ("(a:1,(b,c:2.3)d)e;");
            TEST_EQUAL(
                std::string("(a:1,(b,c:2.3)d)e;"),
                node.str());
        }

        {
            const node_type node("()a;");
            TEST_EQUAL(
                std::string("()a;"),
                node.str());
        }
    }

    // ------------------------------------------------------------------------
    void encode()
    {
        node_type node (" ( a : 1 , ( b , c : 2.3 ) d ) e ; ");
        TEST_EQUAL(
            std::string("(a:1,(b,c:2.3)d)e;"),
            node.str());
    }

    // ------------------------------------------------------------------------
    void find_all()
    {
        node_type node ("(b,(d,e:2)c)a:2;");

        const auto set = node.find_all([](
            const node_type * n)
            -> bool
        {
            return n->has_length();
        });

        TEST_EQUAL(size_t(2), set.size());

        std::set<std::string> names;
        for (const auto ptr : set)
            names.insert(ptr->get_name());

        TEST_TRUE(names.end() == names.find("b"));
        TEST_TRUE(names.end() == names.find("c"));
        TEST_TRUE(names.end() == names.find("d"));
        TEST_TRUE(names.end() != names.find("a"));
        TEST_TRUE(names.end() != names.find("e"));
    }

    // ------------------------------------------------------------------------
    void find_descendents()
    {
        node_type node ("(b:1,(d:1,e:2)c:1)a:2;");

        const auto set = node.find_name("c")->find_descendents();
        TEST_EQUAL(size_t(2), set.size());

        std::set<std::string> names;
        for (const auto ptr : set)
            names.insert(ptr->get_name());

        TEST_TRUE(names.end() == names.find("a"));
        TEST_TRUE(names.end() == names.find("b"));
        TEST_TRUE(names.end() == names.find("c"));
        TEST_TRUE(names.end() != names.find("d"));
        TEST_TRUE(names.end() != names.find("e"));
    }

    // ------------------------------------------------------------------------
    void find_first()
    {
        node_type node ("(b:1,(d:1,e:2)c:1)a:2;");

        const auto first = node.find_first([](
            const node_type * n)
            -> bool
        {
            return n->get_length() < 1.5;
        });

        TEST_EQUAL(std::string("b"), first->get_name());
    }

    // ------------------------------------------------------------------------
    void find_id()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        typedef std::unique_ptr<node_type> ptr_type;
        ptr_type ptr (node.find_name("b")->reroot());

        const auto get_name = [&node, &ptr](
            char const * const name)
            -> const std::string &
        {
            const auto node_id = node.find_name(name)->get_id();
            return ptr->find_id(node_id)->get_name();
        };

        TEST_EQUAL(std::string("a"), get_name("a"));
        TEST_EQUAL(std::string("b"), get_name("b"));
        TEST_EQUAL(std::string("c"), get_name("c"));
        TEST_EQUAL(std::string("d"), get_name("d"));
        TEST_EQUAL(std::string("e"), get_name("e"));
    }

    // ------------------------------------------------------------------------
    void find_leafs()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");
        const auto leafs = node.find_leafs();

        std::set<std::string> names;
        for (const auto ptr : leafs)
            names.insert(ptr->get_name());

        TEST_EQUAL(size_t(3), names.size());
        TEST_TRUE(names.end() == names.find("a"));
        TEST_TRUE(names.end() == names.find("c"));
        TEST_TRUE(names.end() != names.find("b"));
        TEST_TRUE(names.end() != names.find("d"));
        TEST_TRUE(names.end() != names.find("e"));
    }

    // ------------------------------------------------------------------------
    void find_name()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        const auto find = [&node](
            char const * const name)
            -> const std::string &
        {
            return node.find_name(name)->get_name();
        };

        TEST_EQUAL(std::string("a"), find("a"));
        TEST_EQUAL(std::string("b"), find("b"));
        TEST_EQUAL(std::string("c"), find("c"));
        TEST_EQUAL(std::string("d"), find("d"));
        TEST_EQUAL(std::string("e"), find("e"));
    }

    // ------------------------------------------------------------------------
    void find_root()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        const auto root_name = [&node](
            char const * const name)
            -> const std::string &
        {
            return node.find_name(name)->find_root()->get_name();
        };

        TEST_EQUAL(std::string("a"), root_name("a"));
        TEST_EQUAL(std::string("a"), root_name("b"));
        TEST_EQUAL(std::string("a"), root_name("c"));
        TEST_EQUAL(std::string("a"), root_name("d"));
        TEST_EQUAL(std::string("a"), root_name("e"));
    }

    // ------------------------------------------------------------------------
    void for_each()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        std::vector<std::string> names;
        node.for_each([&names](
            const node_type * n)
            -> void
        {
            names.push_back(n->get_name());
        });

        TEST_EQUAL(size_t(5), names.size());
        TEST_EQUAL(std::string("b"), names[0]);
        TEST_EQUAL(std::string("d"), names[1]);
        TEST_EQUAL(std::string("e"), names[2]);
        TEST_EQUAL(std::string("c"), names[3]);
        TEST_EQUAL(std::string("a"), names[4]);
    }

    // ------------------------------------------------------------------------
    void from_file()
    {
        char const * const path = "test.nwk";

        std::ofstream fout (path);
        fout << " ( a : 1 , ( b , c : 2.3 ) d ) e ; ";
        fout.close();

        typedef std::unique_ptr<node_type> ptr_type;
        ptr_type ptr (node_type::from_file(std::string(path)));

        remove(path);

        TEST_EQUAL(
            std::string("(a:1,(b,c:2.3)d)e;"),
            ptr->str());
    }

    // ------------------------------------------------------------------------
    void id()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        const auto eq = [](
            const node_type & lhs,
            char const * const lhs_name,
            const node_type & rhs,
            char const * const rhs_name) -> bool
        {
            return
                lhs.find_name(lhs_name)->get_id() ==
                rhs.find_name(rhs_name)->get_id();
        };

        TEST_FALSE(eq(node, "a", node, "b"));
        TEST_FALSE(eq(node, "a", node, "c"));
        TEST_FALSE(eq(node, "a", node, "d"));
        TEST_FALSE(eq(node, "a", node, "e"));
        TEST_FALSE(eq(node, "b", node, "c"));
        TEST_FALSE(eq(node, "b", node, "d"));
        TEST_FALSE(eq(node, "b", node, "e"));
        TEST_FALSE(eq(node, "c", node, "d"));
        TEST_FALSE(eq(node, "c", node, "e"));
        TEST_FALSE(eq(node, "d", node, "e"));

        typedef std::unique_ptr<node_type> ptr_type;
        ptr_type ptr (node.find_name("b")->reroot());

        TEST_TRUE(eq(node, "a", *ptr, "a"));
        TEST_TRUE(eq(node, "b", *ptr, "b"));
        TEST_TRUE(eq(node, "c", *ptr, "c"));
        TEST_TRUE(eq(node, "d", *ptr, "d"));
        TEST_TRUE(eq(node, "e", *ptr, "e"));
    }

    // ------------------------------------------------------------------------
    void leaf()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        TEST_FALSE(node.find_name("a")->is_leaf());
        TEST_TRUE(node.find_name("b")->is_leaf());
        TEST_FALSE(node.find_name("c")->is_leaf());
        TEST_TRUE(node.find_name("d")->is_leaf());
        TEST_TRUE(node.find_name("e")->is_leaf());
    }

    // ------------------------------------------------------------------------
    void length()
    {
        node_type node ("a:5;");
        TEST_TRUE(node.has_length());
        TEST_ALMOST(value_type(5.0), node.get_length(), epsilon);

        node.erase_length();
        TEST_FALSE(node.has_length());
        TEST_ALMOST(value_type(0.0), node.get_length(), epsilon);

        node.set_length(42.0);
        TEST_TRUE(node.has_length());
        TEST_ALMOST(value_type(42.0), node.get_length(), epsilon);
    }

    // ------------------------------------------------------------------------
    void name()
    {
        node_type node ("a:5;");
        TEST_TRUE(node.has_name());
        TEST_EQUAL(std::string("a"), node.get_name());

        node.set_name("");
        TEST_FALSE(node.has_name());
        TEST_EQUAL(std::string(""), node.get_name());

        node.set_name("b");
        TEST_TRUE(node.has_name());
        TEST_EQUAL(std::string("b"), node.get_name());
    }

    // ------------------------------------------------------------------------
    void parent()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        const auto parent_name = [&node](
            char const * const name)
            -> const std::string &
        {
            return node.find_name(name)->get_parent()->get_name();
        };

        TEST_EQUAL(std::string("a"), parent_name("b"));
        TEST_EQUAL(std::string("a"), parent_name("c"));
        TEST_EQUAL(std::string("c"), parent_name("d"));
        TEST_EQUAL(std::string("c"), parent_name("e"));

        TEST_EQUAL((node_type *)nullptr, node.get_parent());
    }

    // ------------------------------------------------------------------------
    void reroot()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        typedef std::unique_ptr<node_type> ptr_type;
        ptr_type a (node.find_name("a")->reroot());
        TEST_EQUAL(std::string("(b:1,(d:3,e:4)c:2)a;"), a->str());

        ptr_type b (node.find_name("b")->reroot());
        TEST_EQUAL(std::string("(((d:3,e:4)c:2)a:1)b;"), b->str());

        ptr_type c (node.find_name("c")->reroot());
        TEST_EQUAL(std::string("(d:3,e:4,(b:1)a:2)c;"), c->str());

        ptr_type d (node.find_name("d")->reroot());
        TEST_EQUAL(std::string("((e:4,(b:1)a:2)c:3)d;"), d->str());

        ptr_type e (node.find_name("e")->reroot());
        TEST_EQUAL(std::string("((d:3,(b:1)a:2)c:4)e;"), e->str());
    }

    // ------------------------------------------------------------------------
    void root()
    {
        node_type node ("(b:1,(d:3,e:4)c:2)a;");

        TEST_TRUE(node.find_name("a")->is_root());
        TEST_FALSE(node.find_name("b")->is_root());
        TEST_FALSE(node.find_name("c")->is_root());
        TEST_FALSE(node.find_name("d")->is_root());
        TEST_FALSE(node.find_name("e")->is_root());
    }
}

namespace test
{
    test_group newick {
        TEST_CASE(constructor),
        TEST_CASE(encode),
        TEST_CASE(find_all),
        TEST_CASE(find_descendents),
        TEST_CASE(find_first),
        TEST_CASE(find_id),
        TEST_CASE(find_leafs),
        TEST_CASE(find_name),
        TEST_CASE(find_root),
        TEST_CASE(for_each),
        TEST_CASE(from_file),
        TEST_CASE(id),
        TEST_CASE(leaf),
        TEST_CASE(length),
        TEST_CASE(name),
        TEST_CASE(reroot),
        TEST_CASE(root),
        TEST_CASE(parent)
    };
}
