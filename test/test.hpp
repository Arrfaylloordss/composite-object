
//          Copyright Andrey Lifanov 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "composite_object.hpp"
#include <cassert>
#include <iostream>


namespace composite_object
{

namespace unittest
{

    struct object_lifetime_statistics
    {
        unsigned int constructor_calls;
        unsigned int destructor_calls;

        object_lifetime_statistics() :
            constructor_calls(0), destructor_calls(0)
        {
        }

        void inc_constr_calls()
        {
            ++constructor_calls;
        }

        void inc_destr_calls()
        {
            ++destructor_calls;
        }

        bool check_constr_destr_invariant() const
        {
            return constructor_calls == destructor_calls;
        }
    };


    class test_class_interface;
    class test_class_composite_abstract;
    class test_class_composite;
    class test_class_leaf;
    using test_class_composite_interface = composite_object::interface<test_class_interface>;


    class test_class_interface
    {
    public:
        virtual int get_value() const = 0;
        virtual void set_value(int val) = 0;
    };


    class test_class_composite_abstract : public test_class_composite_interface
    {
    public:
        test_class_composite_abstract()
        {
            obj_lifetime_stat.inc_constr_calls();
        }

        test_class_composite_abstract(int value) : value(value)
        {
            obj_lifetime_stat.inc_constr_calls();
        }

        int get_value() const override
        {
            return value;
        }

        void set_value(int val) override
        {
            value = val;
        }

        ~test_class_composite_abstract()
        {
            obj_lifetime_stat.inc_destr_calls();
        }

        static object_lifetime_statistics get_obj_lifetime_stat()
        {
            return obj_lifetime_stat;
        }

        static void reset_obj_lifetime_stat()
        {
            obj_lifetime_stat = object_lifetime_statistics{};
        }

    private:
        int value{ 0 };
        static object_lifetime_statistics obj_lifetime_stat;
    };

    object_lifetime_statistics test_class_composite_abstract::obj_lifetime_stat;


    class test_class_composite : public composite_object::composite<test_class_composite_abstract>
    {
    public:
        test_class_composite() {}

        test_class_composite(int v)
        {
            set_value(v);
        }
    };

    class test_class_leaf : public composite_object::leaf<test_class_composite_abstract>
    {
    public:
        test_class_leaf() {}

        test_class_leaf(int v)
        {
            set_value(v);
        }
    };


    void basic_test()
    {
        auto t = std::make_shared<test_class_composite>();
        {
            auto t1 = std::make_shared<test_class_composite>(1);
            auto t2 = std::make_shared<test_class_composite>(2);
            auto t3 = std::make_shared<test_class_composite>(3);

            auto leaf = std::make_shared<test_class_leaf>(1000);
            t3->cont().push_back(leaf);

            t->cont() = { t1, t2, t3 };

            assert(t->size() == 3);

            auto it = composite_object::wrap_iterator(t->cbegin());

            assert(it->get_value() == 1);
            assert((it++)->get_value() == 2);
            assert((it++++)->get_value() == 3);

            auto rit = composite_object::wrap_iterator(t->crbegin());
            assert(rit->get_value() == 3);
            assert((rit++)->get_value() == 2);
            assert((rit++++)->get_value() == 1);

            auto t3_it = composite_object::wrap_iterator(t3->cbegin());
            assert(t3->size() == 1);
            assert(t3_it->get_value() == 1000);
        }

        t.reset();

        auto obj_lifetime_stat = test_class_composite_abstract::get_obj_lifetime_stat();
        assert(obj_lifetime_stat.check_constr_destr_invariant());
    }

    void hierarchical_iterators_test()
    {
        auto f = std::make_shared<test_class_composite>(0);

        auto a = std::make_shared<test_class_composite>(1);
        auto b = std::make_shared<test_class_composite>(2);
        auto c = std::make_shared<test_class_composite>(3);
        auto d = std::make_shared<test_class_composite>(4);
        auto e = std::make_shared<test_class_composite>(5);
        auto g = std::make_shared<test_class_composite>(6);
        auto i = std::make_shared<test_class_composite>(7);
        auto h = std::make_shared<test_class_leaf>(8);

        f->cont() = { b, g };
        b->cont() = { a, d };
        d->cont() = { c, e };
        g->cont().push_back(i);
        i->cont().push_back(h);

        assert(f->nested_hierarchy_size() == 8);

        auto equal_predicate = [](const auto &a, const auto &b) {return a->get_value() == b->get_value(); };

        auto check_func = [](auto &first, auto &last, std::initializer_list<test_class_composite_interface::iterator_traits::value_type> &&_expected_result) -> bool
        {
            using result_vector_type = std::vector<test_class_composite_interface::iterator_traits::value_type>;
            result_vector_type result;
            result_vector_type expected_result(std::move(_expected_result));
            std::copy(first, last, std::back_inserter(result));
            auto equal_predicate = [](const auto &a, const auto &b) {return a->get_value() == b->get_value(); };
            return std::equal(result.cbegin(), result.cend(), expected_result.cbegin(), equal_predicate);
        };

        auto printValues = [](auto &first, auto &second)
        {
            for (auto it = first; it != second; ++it)
            {
                std::cout << (*it)->get_value() << " ";
            }
            std::cout << std::endl;
        };

        const bool pre_order_ok = check_func(f->cdf_pre_order_begin(), f->cdf_pre_order_end(), { b, a, d, c, e, g, i, h });
        assert(pre_order_ok);

        const bool reverse_pre_order_ok = check_func(f->crdf_pre_order_begin(), f->crdf_pre_order_end(), { g, i, h, b, d, e, c, a });
        assert(reverse_pre_order_ok);

        const bool post_order_ok = check_func(f->cdf_post_order_begin(), f->cdf_post_order_end(), { a, c, e, d, b, h, i, g });
        assert(post_order_ok);

        const bool reverse_post_order_ok = check_func(f->crdf_post_order_begin(), f->crdf_post_order_end(), { h, i, g, e, c, d, a, b });
        assert(reverse_post_order_ok);

        const bool bf_ok = check_func(f->cbf_begin(), f->cbf_end(), { b, g, a, d, i, c, e, h });
        assert(bf_ok);
    }


    void iterators_construction_test()
    {
        using composite_type = test_class_composite;
        using leaf_type = test_class_leaf;
        auto composite = std::make_shared<composite_type>(0);
        auto leaf = std::make_shared<leaf_type>(1);
        composite->cont().push_back(leaf);

        // composite

        {
            composite_type::iterator composite_iterator = composite->begin();
            decltype(composite_iterator) composite_iterator_end = composite->end();
            composite_type::const_iterator composite_const_iterator = composite->cbegin();
            decltype(composite_const_iterator) composite_const_iterator_end = composite->cend();
            composite_type::reverse_iterator composite_reverse_iterator = composite->rbegin();
            decltype(composite_reverse_iterator) composite_reverse_iterator_end = composite->rend();
            composite_type::const_reverse_iterator composite_const_reverse_iterator = composite->crbegin();
            decltype(composite_const_reverse_iterator) composite_const_reverse_iterator_end = composite->crend();

            assert(composite_iterator++ == composite_iterator_end);
            assert(composite_reverse_iterator++ == composite_reverse_iterator_end);
        }

        {
            composite_type::df_pre_order_hierarchical_iterator composite_iterator = composite->df_pre_order_begin();
            decltype(composite_iterator) composite_iterator_end = composite->df_pre_order_end();
            composite_type::const_df_pre_order_hierarchical_iterator composite_const_iterator = composite->cdf_pre_order_begin();
            decltype(composite_const_iterator) composite_const_iterator_end = composite->cdf_pre_order_end();
            composite_type::reverse_df_pre_order_hierarchical_iterator composite_reverse_iterator = composite->rdf_pre_order_begin();
            decltype(composite_reverse_iterator) composite_reverse_iterator_end = composite->rdf_pre_order_end();
            composite_type::const_reverse_df_pre_order_hierarchical_iterator composite_const_reverse_iterator = composite->crdf_pre_order_begin();
            decltype(composite_const_reverse_iterator) composite_const_reverse_iterator_end = composite->crdf_pre_order_end();

            assert(composite_iterator++ == composite_iterator_end);
            assert(composite_reverse_iterator++ == composite_reverse_iterator_end);
        }

        {
            composite_type::df_post_order_hierarchical_iterator composite_iterator = composite->df_post_order_begin();
            decltype(composite_iterator) composite_iterator_end = composite->df_post_order_end();
            composite_type::const_df_post_order_hierarchical_iterator composite_const_iterator = composite->cdf_post_order_begin();
            decltype(composite_const_iterator) composite_const_iterator_end = composite->cdf_post_order_end();
            composite_type::reverse_df_post_order_hierarchical_iterator composite_reverse_iterator = composite->rdf_post_order_begin();
            decltype(composite_reverse_iterator) composite_reverse_iterator_end = composite->rdf_post_order_end();
            composite_type::const_reverse_df_post_order_hierarchical_iterator composite_const_reverse_iterator = composite->crdf_post_order_begin();
            decltype(composite_const_reverse_iterator) composite_const_reverse_iterator_end = composite->crdf_post_order_end();

            assert(composite_iterator++ == composite_iterator_end);
            assert(composite_reverse_iterator++ == composite_reverse_iterator_end);
        }

        // leaf

        {
            leaf_type::iterator leaf_iterator = leaf->begin();
            decltype(leaf_iterator) leaf_iterator_end = leaf->end();
            leaf_type::const_iterator leaf_const_iterator = leaf->cbegin();
            decltype(leaf_const_iterator) leaf_const_iterator_end = leaf->cend();
            leaf_type::reverse_iterator leaf_reverse_iterator = leaf->rbegin();
            decltype(leaf_reverse_iterator) leaf_reverse_iterator_end = leaf->rend();
            leaf_type::const_reverse_iterator leaf_const_reverse_iterator = leaf->crbegin();
            decltype(leaf_const_reverse_iterator) leaf_const_reverse_iterator_end = leaf->crend();
        }

        {
            leaf_type::df_pre_order_hierarchical_iterator leaf_iterator = leaf->df_pre_order_begin();
            decltype(leaf_iterator) leaf_iterator_end = leaf->df_pre_order_end();
            leaf_type::const_df_pre_order_hierarchical_iterator leaf_const_iterator = leaf->cdf_pre_order_begin();
            decltype(leaf_const_iterator) leaf_const_iterator_end = leaf->cdf_pre_order_end();
            leaf_type::reverse_df_pre_order_hierarchical_iterator leaf_reverse_iterator = leaf->rdf_pre_order_begin();
            decltype(leaf_reverse_iterator) leaf_reverse_iterator_end = leaf->rdf_pre_order_end();
            leaf_type::const_reverse_df_pre_order_hierarchical_iterator leaf_const_reverse_iterator = leaf->crdf_pre_order_begin();
            decltype(leaf_const_reverse_iterator) leaf_const_reverse_iterator_end = leaf->crdf_pre_order_end();
        }

        {
            leaf_type::df_post_order_hierarchical_iterator leaf_iterator = leaf->df_post_order_begin();
            decltype(leaf_iterator) leaf_iterator_end = leaf->df_post_order_end();
            leaf_type::const_df_post_order_hierarchical_iterator leaf_const_iterator = leaf->cdf_post_order_begin();
            decltype(leaf_const_iterator) leaf_const_iterator_end = leaf->cdf_post_order_end();
            leaf_type::reverse_df_post_order_hierarchical_iterator leaf_reverse_iterator = leaf->rdf_post_order_begin();
            decltype(leaf_reverse_iterator) leaf_reverse_iterator_end = leaf->rdf_post_order_end();
            leaf_type::const_reverse_df_post_order_hierarchical_iterator leaf_const_reverse_iterator = leaf->crdf_post_order_begin();
            decltype(leaf_const_reverse_iterator) leaf_const_reverse_iterator_end = leaf->crdf_post_order_end();
        }
    }


    void run()
    {
        std::cout << "Test 'composite_object'..." << std::endl;
        basic_test();
        iterators_construction_test();
        hierarchical_iterators_test();
        std::cout << "Ok." << std::endl;
    }

} // unittest
} // composite_object