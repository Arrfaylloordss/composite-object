
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

    struct push_back_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`push_back()` function"; }

        void run() override
        {
            auto new_element = std::make_shared<test_class_composite>(25);
            const size_t size = obj.size();
            const size_t hsize = obj.nested_hierarchy_size();
            obj.push_back(new_element);
            assert(obj.size() == (size + 1));
            assert(obj.nested_hierarchy_size() == (hsize + 1));

            // moving
            auto new_element2 = std::make_shared<test_class_composite>(32);
            obj.push_back(std::move(new_element2));
            assert(!new_element2);
            assert(obj.size() == (size + 2));
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

    struct iterators_returning_functions : public test
    {
        const char * name() const override { return "Iterators returning functions"; }

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


    namespace iterators
    {
        struct hierarchy_basic_setup
        {
            test_class_composite_interface::value_type a, b, c, d, e, f, g, h, i;

            hierarchy_basic_setup()
            {
                // Structure was taken from https://en.wikipedia.org/wiki/Tree_traversal

                f = std::make_shared<test_class_composite>(0);
                a = std::make_shared<test_class_composite>(1);
                b = std::make_shared<test_class_composite>(2);
                c = std::make_shared<test_class_composite>(3);
                d = std::make_shared<test_class_composite>(4);
                e = std::make_shared<test_class_composite>(5);
                g = std::make_shared<test_class_composite>(6);
                i = std::make_shared<test_class_composite>(7);
                h = std::make_shared<test_class_leaf>(8);

                f->push_back(b);
                f->push_back(g);

                b->push_back(a);
                b->push_back(d);

                d->push_back(c);
                d->push_back(e);

                g->push_back(i);
                i->push_back(h);
            }
        };


        namespace iterator
        {
            struct construction : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator construction"; }

                void run() override
                {
                    test_class_composite_interface::iterator it = obj.begin();
                    test_class_composite_interface::iterator it_end = obj.end();
                    unsigned int k = 0;
                    for (it; it != it_end; ++it) ++k;
                    assert(k == obj.size());
                }
            };

            struct copy_construction : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator copy construction"; }

                void run() override
                {
                    test_class_composite_interface::iterator it = obj.begin();
                    auto it_copy{ it };
                    assert(!it.empty());
                    assert(!it_copy.empty());
                    assert(it == it_copy);
                    assert((*it)->get_value() == (*it_copy)->get_value());

                    ++it_copy;
                    assert(it != it_copy);
                    assert((*it)->get_value() != (*it_copy)->get_value());
                }
            };

            struct move_construction : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator move construction"; }

                void run() override
                {
                    test_class_composite_interface::iterator it = obj.begin();
                    auto it_moved{ std::move(it) };
                    assert(it.empty());
                    assert(!it_moved.empty());
                    assert(it != it_moved);
                }
            };

            struct copy_assignment : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator copy assignment"; }

                void run() override
                {
                    const auto it = obj.begin();
                    test_class_composite_interface::iterator it_copy;
                    it_copy = it;
                    assert((*it_copy)->get_value() == (*it)->get_value());
                    assert((*it_copy)->get_value() == leaf_a->get_value());
                    assert((*it++)->get_value() == leaf_b->get_value());
                    assert((*it_copy)->get_value() == leaf_a->get_value());
                }
            };

            struct move_assignment : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator move assignment"; }

                void run() override
                {
                    const auto it = obj.begin();
                    test_class_composite_interface::iterator it_copy;
                    it_copy = it;
                    test_class_composite_interface::iterator it_moved;
                    it_moved = std::move(it_copy);
                    assert(it_copy.empty());
                    assert((*it_moved)->get_value() == (*it)->get_value());
                }
            };

            struct equality_check : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator equality check"; }

                void run() override
                {
                    const auto it = obj.begin();
                    auto it_equal = it;
                    assert(it_equal == it);
                    ++it_equal;
                    assert(it_equal != it);
                    it_equal = obj.end();
                    assert(it_equal == obj.end());
                }
            };

            struct dereferencing : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator dereferencing"; }

                void run() override
                {
                    const auto it = obj.begin();
                    test_class_composite_interface::value_type &leaf_a_ref = *it;
                    assert(leaf_a_ref->get_value() == leaf_a->get_value());
                    assert(it->operator->()->get_value() == leaf_a->get_value());
                }
            };

            struct increment_decrement : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator increment/decrement"; }

                void run() override
                {
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

            struct empty_container : public test
            {
                const char * name() const override { return "Iterators - iterator for empty container"; }

                void run() override
                {
                    test_class_composite composite;
                    test_class_leaf leaf;
                    assert(composite.begin() == composite.end());
                    assert(leaf.begin() == leaf.end());
                }
            };
        }


        namespace reverse_iterator
        {
            struct construction : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator construction"; }

                void run() override
                {
                    test_class_composite_interface::reverse_iterator it = obj.rbegin();
                    test_class_composite_interface::reverse_iterator it_end = obj.rend();
                    unsigned int k = 0;
                    for (it; it != it_end; ++it) ++k;
                    assert(k == obj.size());
                }
            };

            struct copy_construction : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator copy construction"; }

                void run() override
                {
                    test_class_composite_interface::reverse_iterator it = obj.rbegin();
                    auto it_copy{ it };
                    assert(!it.empty());
                    assert(!it_copy.empty());
                    assert(it == it_copy);
                    assert((*it)->get_value() == (*it_copy)->get_value());

                    ++it_copy;
                    assert(it != it_copy);
                    assert((*it)->get_value() != (*it_copy)->get_value());
                }
            };

            struct move_construction : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator move construction"; }

                void run() override
                {
                    auto it = obj.rbegin();
                    auto it_moved{ std::move(it) };
                    assert(it.empty());
                    assert(!it_moved.empty());
                    assert(it != it_moved);
                }
            };

            struct copy_assignment : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator copy assignment"; }

                void run() override
                {
                    const auto it = obj.rbegin();
                    test_class_composite_interface::reverse_iterator it_copy;
                    it_copy = it;
                    assert((*it_copy)->get_value() == (*it)->get_value());
                    assert((*it_copy)->get_value() == composite_c->get_value());
                    assert((*it++)->get_value() == leaf_b->get_value());
                    assert((*it_copy)->get_value() == composite_c->get_value());
                }
            };

            struct move_assignment : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator move assignment"; }

                void run() override
                {
                    const auto it = obj.rbegin();
                    test_class_composite_interface::reverse_iterator it_copy;
                    it_copy = it;
                    test_class_composite_interface::reverse_iterator it_moved;
                    it_moved = std::move(it_copy);
                    assert(it_copy.empty());
                    assert((*it_moved)->get_value() == (*it)->get_value());
                }
            };

            struct equality_check : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator equality check"; }

                void run() override
                {
                    const auto it = obj.rbegin();
                    auto it_equal = it;
                    assert(it_equal == it);
                    ++it_equal;
                    assert(it_equal != it);
                    it_equal = obj.rend();
                    assert(it_equal == obj.rend());
                }
            };

            struct dereferencing : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator dereferencing"; }

                void run() override
                {
                    const auto it = obj.rbegin();
                    test_class_composite_interface::value_type &composite_c_ref = *it;
                    assert(composite_c->get_value() == composite_c_ref->get_value());
                    assert(it->operator->()->get_value() == composite_c->get_value());
                }
            };

            struct increment_decrement : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator increment/decrement"; }

                void run() override
                {
                    auto incremented_it = ++obj.rbegin();
                    assert((*incremented_it)->get_value() == leaf_b->get_value());
                    ++incremented_it;
                    assert((*incremented_it)->get_value() == leaf_a->get_value());
                    auto decremented_it = incremented_it--;
                    assert((*incremented_it)->get_value() == leaf_a->get_value());
                    assert(++incremented_it == obj.rend());
                    assert((*decremented_it)->get_value() == leaf_b->get_value());
                    assert((*--decremented_it)->get_value() == composite_c->get_value());
                }
            };

            struct empty_container : public test
            {
                const char * name() const override { return "Iterators - reverse iterator for empty container"; }

                void run() override
                {
                    test_class_composite composite;
                    test_class_leaf leaf;
                    assert(composite.rbegin() == composite.rend());
                    assert(leaf.rbegin() == leaf.rend());
                }
            };
        }


        namespace depth_first_hierarchical_iterator
        {
            struct construction : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator construction"; }

                void run() override
                {
                    test_class_composite_interface::df_pre_order_hierarchical_iterator it =
                        f->df_pre_order_begin();
                    test_class_composite_interface::df_pre_order_hierarchical_iterator it_end =
                        f->df_pre_order_end();

                    unsigned int k = 0;
                    for (it; it != it_end; ++it) ++k;
                    assert(k == f->nested_hierarchy_size());
                }
            };

            struct copy_construction : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator copy construction"; }

                void run() override
                {
                    test_class_composite_interface::df_pre_order_hierarchical_iterator it = f->df_pre_order_begin();
                    auto it_copy{ it };
                    assert(!it.empty());
                    assert(!it_copy.empty());
                    assert(it == it_copy);
                    assert((*it)->get_value() == (*it_copy)->get_value());

                    ++it_copy;
                    assert(it != it_copy);
                    assert((*it)->get_value() != (*it_copy)->get_value());
                }
            };
          
            struct move_construction : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator move construction"; }

                void run() override
                {
                    auto it = f->df_pre_order_begin();
                    auto it_moved{ std::move(it) };
                    assert(it.empty());
                    assert(!it_moved.empty());
                    assert(it != it_moved);
                }
            };
        
            struct copy_assignment : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator copy assignment"; }

                void run() override
                {
                    const auto it = f->df_pre_order_begin();
                    test_class_composite_interface::df_pre_order_hierarchical_iterator it_copy;
                    it_copy = it;
                    assert(!it.empty());
                    assert(!it_copy.empty());
                    assert(it == it_copy);
                    assert((*it)->get_value() == (*it_copy)->get_value());

                    ++it_copy;
                    assert(it != it_copy);
                    assert((*it)->get_value() != (*it_copy)->get_value());
                }
            };

            struct move_assignment : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator move assignment"; }

                void run() override
                {
                    const auto it = f->df_pre_order_begin();
                    test_class_composite_interface::df_pre_order_hierarchical_iterator it_copy;
                    it_copy = it;
                    test_class_composite_interface::df_pre_order_hierarchical_iterator it_moved;
                    it_moved = std::move(it_copy);
                    assert(it_copy.empty());
                    assert((*it_moved)->get_value() == (*it)->get_value());
                }
            };
            
            struct equality_check : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator equality check"; }

                void run() override
                {
                    const auto it = f->df_pre_order_begin();
                    auto it_equal = it;
                    assert(it_equal == it);
                    ++it_equal;
                    assert(it_equal != it);
                    it_equal = f->df_pre_order_end();
                    assert(it_equal == f->df_pre_order_end());
                }
            };

            struct dereferencing : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator dereferencing"; }

                void run() override
                {
                    const auto it = f->df_pre_order_begin();
                    test_class_composite_interface::value_type &cmomposite_b_ref = *it;
                    assert(cmomposite_b_ref->get_value() == b->get_value());
                    assert(it->operator->()->get_value() == b->get_value());
                }
            };

            struct increment : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - depth-first iterator increment"; }

                void run() override
                {
                    auto incremented_it = ++f->df_pre_order_begin();
                    assert((*incremented_it)->get_value() == a->get_value());
                    ++incremented_it;
                    assert((*incremented_it)->get_value() == d->get_value());
                }
            };

            struct pre_order_traverse_algorithm : public test, public hierarchy_basic_setup
            {
                const char * name() const override
                {
                    return "Iterators - depth-first iterator pre-order traverse algorithm";
                }

                void run() override
                {
                    const std::vector<test_class_composite_interface::value_type> expected_result{ b, a, d, c, e, g, i, h };
                    assert(std::equal(f->cdf_pre_order_begin(), f->cdf_pre_order_end(), expected_result.cbegin()));
                }
            };

            struct reverse_pre_order_traverse_algorithm : public test, public hierarchy_basic_setup
            {
                const char * name() const override
                {
                    return "Iterators - reverse depth-first iterator pre-order traverse algorithm";
                }

                void run() override
                {
                    const std::vector<test_class_composite_interface::value_type> expected_result{ g, i, h, b, d, e, c, a };
                    assert(std::equal(f->crdf_pre_order_begin(), f->crdf_pre_order_end(), expected_result.cbegin()));
                }
            };

            struct post_order_traverse_algorithm : public test, public hierarchy_basic_setup
            {
                const char * name() const override
                {
                    return "Iterators - depth-first iterator post-order traverse algorithm";
                }

                void run() override
                {
                    const std::vector<test_class_composite_interface::value_type> expected_result{ a, c, e, d, b, h, i, g };
                    assert(std::equal(f->cdf_post_order_begin(), f->cdf_post_order_end(), expected_result.cbegin()));
                }
            };

            struct reverse_post_order_traverse_algorithm : public test, public hierarchy_basic_setup
            {
                const char * name() const override
                {
                    return "Iterators - reverse depth-first iterator post-order traverse algorithm";
                }

                void run() override
                {
                    const std::vector<test_class_composite_interface::value_type> expected_result{ h, i, g, e, c, d, a, b };
                    assert(std::equal(f->crdf_post_order_begin(), f->crdf_post_order_end(), expected_result.cbegin()));
                }
            };
        }


        namespace breadth_first_hierarchical_iterator
        {
            struct construction : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator construction"; }

                void run() override
                {
                    test_class_composite_interface::bf_hierarchical_iterator it =
                        f->bf_begin();
                    test_class_composite_interface::bf_hierarchical_iterator it_end =
                        f->bf_end();

                    unsigned int k = 0;
                    for (it; it != it_end; ++it) ++k;
                    assert(k == f->nested_hierarchy_size());
                }
            };

            struct copy_construction : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator copy construction"; }

                void run() override
                {
                    test_class_composite_interface::bf_hierarchical_iterator it = f->bf_begin();
                    auto it_copy{ it };
                    assert(!it.empty());
                    assert(!it_copy.empty());
                    assert(it == it_copy);
                    assert((*it)->get_value() == (*it_copy)->get_value());

                    ++it_copy;
                    assert(it != it_copy);
                    assert((*it)->get_value() != (*it_copy)->get_value());
                }
            };

            struct move_construction : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator move construction"; }

                void run() override
                {
                    auto it = f->bf_begin();
                    auto it_moved{ std::move(it) };
                    assert(it.empty());
                    assert(!it_moved.empty());
                    assert(it != it_moved);
                }
            };

            struct copy_assignment : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator copy assignment"; }

                void run() override
                {
                    const auto it = f->bf_begin();
                    test_class_composite_interface::bf_hierarchical_iterator it_copy;
                    it_copy = it;
                    assert(!it.empty());
                    assert(!it_copy.empty());
                    assert(it == it_copy);
                    assert((*it)->get_value() == (*it_copy)->get_value());

                    ++it_copy;
                    assert(it != it_copy);
                    assert((*it)->get_value() != (*it_copy)->get_value());
                }
            };

            struct move_assignment : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator move assignment"; }

                void run() override
                {
                    const auto it = f->bf_begin();
                    test_class_composite_interface::bf_hierarchical_iterator it_copy;
                    it_copy = it;
                    test_class_composite_interface::bf_hierarchical_iterator it_moved;
                    it_moved = std::move(it_copy);
                    assert(it_copy.empty());
                    assert((*it_moved)->get_value() == (*it)->get_value());
                }
            };

            struct equality_check : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator equality check"; }

                void run() override
                {
                    const auto it = f->bf_begin();
                    auto it_equal = it;
                    assert(it_equal == it);
                    ++it_equal;
                    assert(it_equal != it);
                    it_equal = f->bf_end();
                    assert(it_equal == f->bf_end());
                }
            };

            struct dereferencing : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator dereferencing"; }

                void run() override
                {
                    const auto it = f->bf_begin();
                    test_class_composite_interface::value_type &cmomposite_b_ref = *it;
                    assert(cmomposite_b_ref->get_value() == b->get_value());
                    assert(it->operator->()->get_value() == b->get_value());
                }
            };

            struct increment : public test, public hierarchy_basic_setup
            {
                const char * name() const override { return "Iterators - breadth-first iterator increment"; }

                void run() override
                {
                    auto incremented_it = ++f->bf_begin();
                    assert((*incremented_it)->get_value() == g->get_value());
                    ++incremented_it;
                    assert((*incremented_it)->get_value() == a->get_value());
                }
            };

            struct traverse_algorithm : public test, public hierarchy_basic_setup
            {
                const char * name() const override
                {
                    return "Iterators - breadth-first iterator traverse algorithm";
                }

                void run() override
                {
                    const std::vector<test_class_composite_interface::value_type> expected_result{ b, g, a, d, i, c, e, h };
                    assert(std::equal(f->cbf_begin(), f->cbf_end(), expected_result.cbegin()));
                }
            };

            struct reverse_traverse_algorithm : public test, public hierarchy_basic_setup
            {
                const char * name() const override
                {
                    return "Iterators - reverse breadth-first iterator traverse algorithm";
                }

                void run() override
                {
                    const std::vector<test_class_composite_interface::value_type> expected_result{ g, b, i, d, a, h, e, c };
                    assert(std::equal(f->crbf_begin(), f->crbf_end(), expected_result.cbegin()));
                }
            };
        }
    }


    void run()
    {
        std::vector<std::unique_ptr<test>> tests;
        tests.emplace_back(new construction_and_lifetime());
        tests.emplace_back(new copy_construction());
        tests.emplace_back(new move_construction());
        tests.emplace_back(new push_back_function());
        tests.emplace_back(new is_leaf_function());
        tests.emplace_back(new is_composite_function());
        tests.emplace_back(new clear_function());
        tests.emplace_back(new size_function());
        tests.emplace_back(new nested_hierarchy_size_function());
        tests.emplace_back(new iterators_returning_functions());

        // Iterators checks

        tests.emplace_back(new iterators::iterator::construction());
        tests.emplace_back(new iterators::iterator::copy_construction());
        tests.emplace_back(new iterators::iterator::move_construction());
        tests.emplace_back(new iterators::iterator::copy_assignment());
        tests.emplace_back(new iterators::iterator::move_assignment());
        tests.emplace_back(new iterators::iterator::equality_check());
        tests.emplace_back(new iterators::iterator::dereferencing());
        tests.emplace_back(new iterators::iterator::increment_decrement());
        tests.emplace_back(new iterators::iterator::empty_container());

        tests.emplace_back(new iterators::reverse_iterator::construction());
        tests.emplace_back(new iterators::reverse_iterator::copy_construction());
        tests.emplace_back(new iterators::reverse_iterator::move_construction());
        tests.emplace_back(new iterators::reverse_iterator::copy_assignment());
        tests.emplace_back(new iterators::reverse_iterator::move_assignment());
        tests.emplace_back(new iterators::reverse_iterator::equality_check());
        tests.emplace_back(new iterators::reverse_iterator::dereferencing());
        tests.emplace_back(new iterators::reverse_iterator::increment_decrement());
        tests.emplace_back(new iterators::reverse_iterator::empty_container());

        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::construction());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::copy_construction());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::move_construction());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::copy_assignment());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::move_assignment());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::equality_check());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::dereferencing());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::increment());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::pre_order_traverse_algorithm());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::reverse_pre_order_traverse_algorithm());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::post_order_traverse_algorithm());
        tests.emplace_back(new iterators::depth_first_hierarchical_iterator::reverse_post_order_traverse_algorithm());

        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::construction());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::copy_construction());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::move_construction());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::copy_assignment());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::move_assignment());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::equality_check());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::dereferencing());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::increment());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::traverse_algorithm());
        tests.emplace_back(new iterators::breadth_first_hierarchical_iterator::reverse_traverse_algorithm());
        

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