
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
    using test_class_composite_interface = composite_object::abstract<
        test_class_interface,
        composite_object::default_pointer_model
    >;
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
        }

        test_class_composite_base_impl(int value) : value(value)
        {
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
        }

    private:
        int value{ 0 };
    };


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


    class test_class_null_reference :
        public composite_object::null_reference<test_class_composite_interface>
    {
    public:
        test_class_null_reference()
        {
        }

        int get_value() const override
        {
            return int();
        }

        void set_value(int val) override
        {
        }

        raw_pointer_to_base_interface clone() const override
        {
            return new test_class_null_reference(*this);
        }
    };


    class test_class_reference : public test_class_composite_interface::reference
    {
        using self = test_class_reference;
        using parent = typename test_class_composite_interface::reference;

    public:
        test_class_reference()
        {
            init();
        }

        test_class_reference(const smart_ptr &source) : parent(source)
        {
        }

        null_reference_type *obtain_null_reference() override
        {
            static test_class_null_reference obj;
            return &obj;
        }

        int get_value() const override
        {
            return ptr->get_value();
        }

        void set_value(int val) override
        {
            ptr->set_value(val);
        }

        raw_pointer_to_base_interface clone() const override
        {
            return new self(*this);
        }

    public:
        static bool are_same(const smart_ptr &a, const self &b)
        {
            return a->get_value() == b.get_value();
        }

    };


    struct basic_test_setup
    {
        basic_test_setup()
        {
            using smart_ptr = typename test_class_composite_interface::smart_ptr;

            obj.push_back(smart_ptr(new test_class_leaf(1)));
            obj.push_back(smart_ptr(new test_class_leaf(2)));
            obj.push_back(smart_ptr(new test_class_composite(4)));
            (*obj.rbegin())->push_back(smart_ptr(new test_class_leaf(3)));

            auto it = obj.begin();
            leaf_a = *it;
            leaf_b = *(++it);
            composite_c = *(++it);
            leaf_c = *((*it)->begin());
        }

        test_class_composite obj;
        test_class_reference leaf_a, leaf_b, leaf_c, composite_c;
    };


    struct construction : public test
    {
        const char * name() const override { return "Construction"; }

        void run() override
        {
            const int val = 100500;
            auto obj = std::make_unique<test_class_composite>(val);
            assert(obj->get_value() == val);
        }
    };

    struct reference_type: public test
    {
        const char * name() const override { return "Reference type"; }

        void run() override
        {
            using smart_ptr = test_class_composite_interface::smart_ptr;

            smart_ptr obj(new test_class_composite(1));
            test_class_reference obj_ref(obj);
            assert(obj_ref.is_reference());
            test_class_reference obj_ref_copy(obj_ref);

            assert(!obj_ref.is_null() && !obj_ref_copy.is_null());
            assert(obj_ref.get_value() == obj->get_value());
            assert(obj_ref.points_to(obj));
            assert(obj_ref_copy.points_to(obj));

            test_class_reference default_ref{};
            assert(default_ref.is_null());
            default_ref = obj;
            assert(!default_ref.is_null());
            assert(default_ref.points_to(obj));

            assert(obj_ref.is_traversable() == false);
            obj_ref.set_traversable(true);
            assert(obj_ref.is_traversable());

            default_ref.reset();
            assert(default_ref.is_null());

            smart_ptr leaf(new test_class_leaf(2));
            obj_ref.push_back(std::move(leaf));
            assert(obj_ref.size() == 1);
            assert(obj->size() == 1);
            assert(obj_ref.nested_hierarchy_size() == 1);
            obj_ref.set_traversable(false);
            assert(obj_ref.size() == 0);
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

            assert(leaf_a.get_value() == 1);
            assert((*it)->get_value() == 1000);
            assert ((*it)->get_value() != leaf_a.get_value());

            std::advance(it, 2);
            test_class_reference composite_c_ref(*it);
            test_class_reference leaf_c_ref(*composite_c_ref.begin());
            leaf_c_ref.set_value(2000);
            
            assert(leaf_c.get_value() == 3);
            assert(leaf_c.get_value() != leaf_c_ref.get_value());
        }
    };

    struct move_construction : public test, public basic_test_setup
    {
        const char * name() const override { return "Move construction"; }

        void run() override
        {
            test_class_composite obj_moved(std::move(obj));

            assert(obj.size() == 0);

            test_class_reference moved_leaf_a_ref = *obj_moved.begin();
            moved_leaf_a_ref.set_value(1000);

            assert(leaf_a.get_value() == 1000);
            assert(moved_leaf_a_ref.get_value() == 1000);
            assert(moved_leaf_a_ref.get_value() == leaf_a.get_value());
        }
    };

    struct push_back_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`push_back()` function"; }

        void run() override
        {
            using smart_ptr = typename test_class_composite_interface::smart_ptr;
            auto new_element = smart_ptr(new test_class_composite(25));
            const size_t size = obj.size();
            const size_t hsize = obj.nested_hierarchy_size();
            obj.push_back(std::move(new_element));
            assert(obj.size() == (size + 1));
            assert(obj.nested_hierarchy_size() == (hsize + 1));

            // moving
            auto new_element2 = smart_ptr(new test_class_composite(32));
            obj.push_back(std::move(new_element2));
            assert(!new_element2);
            assert(obj.size() == (size + 2));
        }
    };

    struct remove_if_function : public test
    {
        const char * name() const override { return "`remove_if()` function"; }

        test_class_composite construct_obj()
        {
            test_class_composite obj;

            using smart_ptr = decltype(obj)::smart_ptr;

            auto composite = [](int val) { return smart_ptr(new test_class_composite(val)); };
            auto leaf = [](int val) { return smart_ptr(new test_class_leaf(val)); };
            auto reference = [](const smart_ptr &ptr) { return smart_ptr(new test_class_reference(ptr)); };

            obj.push_back(composite(1));
            obj.push_back(composite(2));
            obj.push_back(composite(3));
            obj.push_back(leaf(2));

            smart_ptr &composite_2 = *++obj.begin();
            composite_2->push_back(reference(*obj.rbegin()));
            composite_2->push_back(leaf(10));
            obj.push_back(reference(composite_2));

            return obj;
        }

        void run() override
        {
            auto pred = [](const auto &obj) { return obj->get_value() == 2; };
           
            {
                test_class_composite obj = construct_obj();
                const size_t size = obj.nested_hierarchy_size();

                obj.remove_if(pred);

                assert(obj.nested_hierarchy_size() == size - 5);

                auto &cont = obj.cont();
                assert(cont.end() == std::find_if(cont.begin(), cont.end(), pred));
                assert(obj.df_pre_order_end() == std::find_if(obj.df_pre_order_begin(), obj.df_pre_order_end(), pred));
            }

            {
                test_class_composite obj = construct_obj();
                const size_t size = obj.nested_hierarchy_size();

                obj.remove_if(pred, test_class_composite::nullify_references);

                assert(obj.nested_hierarchy_size() == size - 4);
                assert((*obj.rbegin())->is_null_reference());

                auto &cont = obj.cont();
                assert(obj.df_pre_order_end() == std::find_if(obj.df_pre_order_begin(), obj.df_pre_order_end(), pred));
            }
        }
    };

    struct is_leaf_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`is_leaf()` function"; }

        void run() override
        {
            assert(leaf_a.is_leaf());
            assert(!composite_c.is_leaf());
        }
    };

    struct is_composite_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`is_composite()` function"; }

        void run() override
        {
            assert(obj.is_composite());
            assert(!leaf_a.is_composite());
            assert(composite_c.is_composite());
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
            using smart_ptr = typename test_class_composite_interface::smart_ptr;
            assert(obj.size() == 3);
            obj.cont().push_back(smart_ptr(new test_class_leaf(1)));
            assert(obj.size() == 4);
        }
    };

    struct nested_hierarchy_size_function : public test, public basic_test_setup
    {
        const char * name() const override { return "`nested_hierarchy_size()` function"; }

        void run() override
        {
            using smart_ptr = typename test_class_composite_interface::smart_ptr;
            assert(obj.nested_hierarchy_size() == 4);
            obj.cont().push_back(smart_ptr(new test_class_leaf(1)));
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
            using smart_ptr = typename test_class_composite_interface::smart_ptr;
            auto composite = smart_ptr(new composite_type(0));
            auto leaf = smart_ptr(new leaf_type(1));
            composite->push_back(smart_ptr(new test_class_reference(leaf)));

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

    struct pointer_to_parent : public test, public basic_test_setup
    {
        const char * name() const override { return "Pointer to parent"; }

        void run() override
        {
            assert(leaf_a.get_parent() == nullptr);
            const auto &front = *obj.cbegin();
            const auto &back = *obj.crbegin();
            assert(front->get_parent() != nullptr);
            assert(front->get_parent() == back->get_parent());
            assert(leaf_a.get()->get_parent() == &obj);
            assert(leaf_c.get()->get_parent() == composite_c.get());
        }
    };

    struct relocate_to_function: public test, public basic_test_setup
    {
        const char * name() const override { return "`relocate_to` function"; }

        void run() override
        {
            const size_t size_before_relocation = obj.size();
            auto &leaf_a = *obj.begin();
            auto &composite_c = *obj.rbegin();
            leaf_a->relocate_to(composite_c);
            assert(obj.size() == size_before_relocation - 1);
            assert((*composite_c->rbegin())->get_parent() == composite_c.get());

            using smart_ptr = typename test_class_composite_interface::smart_ptr;
            obj.push_back(smart_ptr(new test_class_leaf(100500)));
            auto &leaf_b = *obj.begin();
            auto &leaf_new = *obj.rbegin();
            leaf_b->relocate_to(leaf_new); //fail, can't move to leaf.
            assert(leaf_b->get_parent() == leaf_new->get_parent());
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

                /*
                 *                          (f)
                 *              (b)                     (g)
                 *        (a)         (d)                      (i)
                 *               (c)       (e)            (h)
                */

                using smart_ptr = typename test_class_composite_interface::smart_ptr;

                f = smart_ptr(new test_class_composite(0));
                a = smart_ptr(new test_class_composite(1));
                b = smart_ptr(new test_class_composite(2));
                c = smart_ptr(new test_class_composite(3));
                d = smart_ptr(new test_class_composite(4));
                e = smart_ptr(new test_class_composite(5));
                g = smart_ptr(new test_class_composite(6));
                i = smart_ptr(new test_class_composite(7));
                h = smart_ptr(new test_class_leaf(8));

                auto traversable_reference = [](const smart_ptr &ptr)
                {
                    auto new_obj = new test_class_reference(ptr);
                    new_obj->set_traversable(true);
                    return smart_ptr(new_obj);
                };

                f->push_back(traversable_reference(b));
                f->push_back(traversable_reference(g));

                b->push_back(traversable_reference(a));
                b->push_back(traversable_reference(d));

                d->push_back(traversable_reference(c));
                d->push_back(traversable_reference(e));

                g->push_back(traversable_reference(i));
                i->push_back(traversable_reference(h));
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
                    assert((*it_copy)->get_value() == leaf_a.get_value());
                    assert((*it++)->get_value() == leaf_b.get_value());
                    assert((*it_copy)->get_value() == leaf_a.get_value());
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
                    assert(leaf_a_ref->get_value() == leaf_a.get_value());
                    assert(it->operator->()->get_value() == leaf_a.get_value());
                }
            };

            struct increment_decrement : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - iterator increment/decrement"; }

                void run() override
                {
                    auto incremented_it = ++obj.begin();
                    assert((*incremented_it)->get_value() == leaf_b.get_value());
                    ++incremented_it;
                    assert((*incremented_it)->get_value() == composite_c.get_value());
                    auto decremented_it = incremented_it--;
                    assert((*incremented_it)->get_value() == composite_c.get_value());
                    assert(++incremented_it == obj.end());
                    assert((*decremented_it)->get_value() == leaf_b.get_value());
                    assert((*--decremented_it)->get_value() == leaf_a.get_value());
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
                    assert((*it_copy)->get_value() == composite_c.get_value());
                    assert((*it++)->get_value() == leaf_b.get_value());
                    assert((*it_copy)->get_value() == composite_c.get_value());
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
                    assert(composite_c.get_value() == composite_c_ref->get_value());
                    assert(it->operator->()->get_value() == composite_c.get_value());
                }
            };

            struct increment_decrement : public test, public basic_test_setup
            {
                const char * name() const override { return "Iterators - reverse iterator increment/decrement"; }

                void run() override
                {
                    auto incremented_it = ++obj.rbegin();
                    assert((*incremented_it)->get_value() == leaf_b.get_value());
                    ++incremented_it;
                    assert((*incremented_it)->get_value() == leaf_a.get_value());
                    auto decremented_it = incremented_it--;
                    assert((*incremented_it)->get_value() == leaf_a.get_value());
                    assert(++incremented_it == obj.rend());
                    assert((*decremented_it)->get_value() == leaf_b.get_value());
                    assert((*--decremented_it)->get_value() == composite_c.get_value());
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
                    const std::vector<test_class_reference> expected_result{ b, a, d, c, e, g, i, h };
                    assert(expected_result.front().points_to(b));
                    assert(expected_result.back().points_to(h));

                    assert(std::equal(f->cdf_pre_order_begin(),
                                      f->cdf_pre_order_end(),
                                      expected_result.cbegin(),
                                      test_class_reference::are_same));
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
                    const std::vector<test_class_reference> expected_result{ g, i, h, b, d, e, c, a };
                    assert(std::equal(f->crdf_pre_order_begin(),
                                      f->crdf_pre_order_end(),
                                      expected_result.cbegin(),
                                      test_class_reference::are_same));
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
                    const std::vector<test_class_reference> expected_result{ a, c, e, d, b, h, i, g };
                    assert(std::equal(f->cdf_post_order_begin(),
                                      f->cdf_post_order_end(),
                                      expected_result.cbegin(),
                                      test_class_reference::are_same));
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
                    const std::vector<test_class_reference> expected_result{ h, i, g, e, c, d, a, b };
                    assert(std::equal(f->crdf_post_order_begin(),
                                      f->crdf_post_order_end(),
                                      expected_result.cbegin(),
                                      test_class_reference::are_same));
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
                    const std::vector<test_class_reference> expected_result{ b, g, a, d, i, c, e, h };
                    assert(std::equal(f->cbf_begin(),
                                      f->cbf_end(),
                                      expected_result.cbegin(),
                                      test_class_reference::are_same));
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
                    const std::vector<test_class_reference> expected_result{ g, b, i, d, a, h, e, c };
                    assert(std::equal(f->crbf_begin(),
                                      f->crbf_end(),
                                      expected_result.cbegin(),
                                      test_class_reference::are_same));
                }
            };
        }
    }


    void run()
    {
        std::vector<std::unique_ptr<test>> tests;
        tests.emplace_back(new construction());
        tests.emplace_back(new reference_type());
        tests.emplace_back(new copy_construction());
        tests.emplace_back(new move_construction());
        tests.emplace_back(new push_back_function());
        tests.emplace_back(new remove_if_function());
        tests.emplace_back(new is_leaf_function());
        tests.emplace_back(new is_composite_function());
        tests.emplace_back(new clear_function());
        tests.emplace_back(new size_function());
        tests.emplace_back(new nested_hierarchy_size_function());
        tests.emplace_back(new iterators_returning_functions());
        tests.emplace_back(new pointer_to_parent());
        tests.emplace_back(new relocate_to_function());

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