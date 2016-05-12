
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
    class test_class_interface;
    using test_class_composite_interface = composite_object::abstract<test_class_interface>;
    class test_class_composite_base_impl;
    class test_class_composite;
    class test_class_leaf;

    namespace
    {
        class test
        {
        public:
            virtual const char * name() const = 0;
            virtual void run() = 0;
        };


        class object_lifetime_statistics
        {
        public:
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

            bool is_lifetime_valid() const
            {
                return constructor_calls == destructor_calls;
            }

        private:
            unsigned int constructor_calls;
            unsigned int destructor_calls;
        };
    }


    class test_class_interface
    {
    public:
        virtual int get_value() const = 0;
        virtual void set_value(int val) = 0;
    };


    class test_class_composite_base_impl : public test_class_composite_interface
    {
    public:
        test_class_composite_base_impl()
        {
            obj_lifetime_stat.inc_constr_calls();
        }

        test_class_composite_base_impl(int value) : value(value)
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

        ~test_class_composite_base_impl()
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

    object_lifetime_statistics test_class_composite_base_impl::obj_lifetime_stat;


    class test_class_composite : public composite_object::composite<test_class_composite_base_impl>
    {
        using self = test_class_composite;
        using parent = composite_object::composite<test_class_composite_base_impl>;
    public:
        test_class_composite() {}

        test_class_composite(int v)
        {
            set_value(v);
        }

        test_class_composite(const self &another)
            : parent(another)
        {
        }

        test_class_composite(self &&another)
            : parent(std::move(another))
        {
        }
    };

    class test_class_leaf : public composite_object::leaf<test_class_composite_base_impl>
    {
    public:
        test_class_leaf() {}

        test_class_leaf(int v)
        {
            set_value(v);
        }
    };


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


    struct basic_test_setup
    {
        basic_test_setup()
        {
            leaf_a = std::make_shared<test_class_leaf>(1);
            leaf_b = std::make_shared<test_class_leaf>(2);
            leaf_c = std::make_shared<test_class_leaf>(3);
            auto composite_c_tmp = std::make_shared<test_class_composite>(4);
            composite_c_tmp->cont().push_back(leaf_c);
            composite_c = composite_c_tmp;

            obj.cont() = { leaf_a, leaf_b, composite_c };
        }

        test_class_composite obj;
        decltype(obj)::value_type leaf_a, leaf_b, leaf_c, composite_c;
    };


    struct construction_and_lifetime : public test
    {
        const char * name() const override { return "Construction and lifetime"; }

        void run() override
        {
            const int val = 100500;
            auto obj = std::make_shared<test_class_composite>(val);
            assert(obj->get_value() == val);

            test_lifetime();
        }

    private:
        void test_lifetime()
        {
            test_class_composite_base_impl::reset_obj_lifetime_stat();

            auto t = std::make_shared<test_class_composite>();
            {
                auto t1 = std::make_shared<test_class_composite>(1);
                auto t2 = std::make_shared<test_class_composite>(2);
                auto t3 = std::make_shared<test_class_composite>(3);

                auto t_leaf = std::make_shared<test_class_leaf>(1000);
                t3->cont().push_back(t_leaf);

                t->cont() = { t1, t2, t3 };
                assert(t->size() == 3);
            }

            t.reset();

            assert(test_class_composite_base_impl::get_obj_lifetime_stat().is_lifetime_valid());
        }
    };

    struct copy_construction : public test, public basic_test_setup
    {
        const char * name() const override { return "Copy construction"; }

        void run() override
        {
            test_class_composite obj_copy(obj);
            auto it = obj_copy.begin();
            (*it)->set_value(1000);

            assert(leaf_a->get_value() == 1);
            assert((*it)->get_value() == 1000);
            assert ((*it)->get_value() != leaf_a->get_value());

            std::advance(it, 2);
            auto composite_c_copy = *it;
            auto leaf_c_copy = *(composite_c_copy->begin());
            leaf_c_copy->set_value(2000);
            
            assert(leaf_c->get_value() == 3);
            assert(leaf_c->get_value() != leaf_c_copy->get_value());
        }
    };

    struct move_construction : public test, public basic_test_setup
    {
        const char * name() const override { return "Move construction"; }

        void run() override
        {
            test_class_composite obj_moved(std::move(obj));

            assert(obj.size() == 0);

            auto moved_leaf_a = *obj_moved.begin();
            moved_leaf_a->set_value(1000);

            assert(leaf_a->get_value() == 1000);
            assert(moved_leaf_a->get_value() == 1000);
            assert(moved_leaf_a->get_value() == leaf_a->get_value());
        }
    };

    struct is_leaf_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`is_leaf()` function"; }

        void run() override
        {
            assert(leaf_a->is_leaf());
            assert(!composite_c->is_leaf());
        }
    };

    struct is_composite_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`is_composite()` function"; }

        void run() override
        {
            assert(obj.is_composite());
            assert(!leaf_a->is_composite());
            assert(composite_c->is_composite());
        }
    };

    struct clear_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`clear()` function"; }

        void run() override
        {
            obj.clear();
            assert(obj.size() == 0);
            assert(obj.cbegin() == obj.cend());
        }
    };

    struct size_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`size()` function"; }

        void run() override
        {
            assert(obj.size() == 3);
            obj.cont().push_back(std::make_shared<test_class_leaf>(1));
            assert(obj.size() == 4);
        }
    };

    struct nested_hierarchy_size_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`nested_hierarchy_size()` function"; }

        void run() override
        {
            assert(obj.nested_hierarchy_size() == 4);
            obj.cont().push_back(std::make_shared<test_class_leaf>(1));
            assert(obj.nested_hierarchy_size() == 5);
            obj.clear();
            assert(obj.nested_hierarchy_size() == 0);
        }
    };


    namespace iterators
    {
        struct contruction : public test
        {
            const char * name() const override { return "Construction of iterators"; }

            void run() override
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

                {
                    composite_type::bf_hierarchical_iterator composite_iterator = composite->bf_begin();
                    decltype(composite_iterator) composite_iterator_end = composite->bf_end();
                    composite_type::const_bf_hierarchical_iterator composite_const_iterator = composite->cbf_begin();
                    decltype(composite_const_iterator) composite_const_iterator_end = composite->cbf_end();
                    composite_type::reverse_bf_hierarchical_iterator composite_reverse_iterator = composite->rbf_begin();
                    decltype(composite_reverse_iterator) composite_reverse_iterator_end = composite->rbf_end();
                    composite_type::const_reverse_bf_hierarchical_iterator composite_const_reverse_iterator = composite->crbf_begin();
                    decltype(composite_const_reverse_iterator) composite_const_reverse_iterator_end = composite->crbf_end();

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

                {
                    leaf_type::bf_hierarchical_iterator leaf_iterator = leaf->bf_begin();
                    decltype(leaf_iterator) leaf_iterator_end = leaf->bf_end();
                    leaf_type::const_bf_hierarchical_iterator leaf_const_iterator = leaf->cbf_begin();
                    decltype(leaf_const_iterator) leaf_const_iterator_end = leaf->cbf_end();
                    leaf_type::reverse_bf_hierarchical_iterator leaf_reverse_iterator = leaf->rbf_begin();
                    decltype(leaf_reverse_iterator) leaf_reverse_iterator_end = leaf->rbf_end();
                    leaf_type::const_reverse_bf_hierarchical_iterator leaf_const_reverse_iterator = leaf->crbf_begin();
                    decltype(leaf_const_reverse_iterator) leaf_const_reverse_iterator_end = leaf->crbf_end();
                }
            }
        };


        struct hierarchy_basic_setup
        {
            hierarchy_basic_setup()
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
            }
        };

        struct iterator : public test, public basic_test_setup
        {
            const char * name() const override { return "Iterators - iterator"; }

            void run() override
            {
                {
                    test_class_composite_interface::iterator it = obj.begin();
                    unsigned int k = 0;
                    for (it; it != obj.end(); ++it)
                    {
                        ++k;
                    }

                    assert(k == obj.size());
                }

                const auto it = obj.begin();

                // copy assignment
                test_class_composite_interface::iterator it_copy;
                it_copy = it;
                assert((*it_copy)->get_value() == (*it)->get_value());
                assert((*it_copy)->get_value() == leaf_a->get_value());
                assert((*it++)->get_value() == leaf_b->get_value());
                assert((*it_copy)->get_value() == leaf_a->get_value());

                // move assigment
                test_class_composite_interface::iterator it_moved;
                it_moved = std::move(it_copy);
                assert(it_copy.empty());
                assert((*it_moved)->get_value() == leaf_a->get_value());

                // equality
                auto it_equal = it;
                assert(it_equal == it);
                ++it_equal;
                assert(it_equal != it);
                it_equal = obj.end();
                assert(it_equal == obj.end());

                // dereferencing
                test_class_composite_interface::value_type &leaf_a_ref = *it;
                assert(leaf_a_ref->get_value() == leaf_a->get_value());
                assert(it->operator->()->get_value() == leaf_a->get_value());

                // increment/decrement
                auto incremented_it = ++obj.begin();
                assert((*incremented_it)->get_value() == leaf_b->get_value());
                ++incremented_it;
                assert((*incremented_it)->get_value() == composite_c->get_value());
                auto decremented_it = incremented_it--;
                assert((*incremented_it)->get_value() == composite_c->get_value());
                assert(++incremented_it == obj.end());
                assert((*decremented_it)->get_value() == leaf_b->get_value());
                assert((*--decremented_it)->get_value() == leaf_a->get_value());
            }
        };
    }
    

    void run()
    {
        std::vector<std::unique_ptr<test>> tests;
        tests.emplace_back(std::make_unique<construction_and_lifetime>());
        tests.emplace_back(std::make_unique<copy_construction>());
        tests.emplace_back(std::make_unique<move_construction>());
        tests.emplace_back(std::make_unique<is_leaf_function>());
        tests.emplace_back(std::make_unique<is_composite_function>());
        tests.emplace_back(std::make_unique<clear_function>());
        tests.emplace_back(std::make_unique<size_function>());
        tests.emplace_back(std::make_unique<nested_hierarchy_size_function>());

        tests.emplace_back(std::make_unique<iterators::contruction>());
        tests.emplace_back(std::make_unique<iterators::iterator>());

        std::cout << "Test 'composite_object'..." << std::endl;

        for (auto &t : tests)
        {
            std::cout << "    " << t->name() << "... ";
            t->run();
            std::cout << "Ok." << std::endl;
        }

        std::cout << "Ok." << std::endl;
    }

} // unittest
} // composite_object