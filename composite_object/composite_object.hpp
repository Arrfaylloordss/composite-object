
//          Copyright Andrey Lifanov 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <memory>
#include <iterator>
#include <list>
#include <stack>
#include <vector>
#include <queue>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <utility>
#include <initializer_list>
#include <functional>

#ifdef _MSC_VER
    #pragma warning( disable : 4503)
#endif


namespace composite_object
{

template <class T>
struct default_pointer_model
{
    using type = std::unique_ptr<T, std::default_delete<T>>;
};

template <class T>
struct shared_pointer_model
{
    using type = std::shared_ptr<T>;
};

template <class T>
struct default_container_type
{
    using type = std::list<T>;
};


template
    <
    class Base,
    template <class T> class PointerModel = default_pointer_model,
    class BaseIteratorCategory = std::bidirectional_iterator_tag
    >
    class abstract;


template <class Base>
    class leaf;


template
    <
    class Base,
    template <class T> class Container = default_container_type
    >
    class composite;


template <class Base>
    class null_reference;


template <class Base>
    class reference;


template
    <
    class Category,
    class T,
    class Distance = ptrdiff_t,
    class Pointer = T*,
    class Reference = T&,
    bool reversed = false
    >
    class polymorphic_iterator_template;


enum class df_traverse_algorithm
{
    pre_order,
    post_order
};


template
    <
    class LinearIterator,
    const df_traverse_algorithm traverse_algorithm_param,
    bool reversed = false
    >
    class df_hierarchical_iterator_template;


template
    <
    class LinearIterator,
    bool reversed = false
    >
    class bf_hierarchical_iterator_template;

//


template
<
    class Base,
    template <class T> class PointerModel,
    class BaseIteratorCategory
>
class abstract : public Base
{
    using self = abstract;

    template <class Base>
        friend class leaf;

    template
        <
        class Base,
        template <class T> class Container
        >
        friend class composite;

    template <class Base>
        friend class reference;

public:
    using smart_ptr = typename PointerModel<self>::type;
    using value_type = smart_ptr;
    using reference = composite_object::reference<self>;
    using raw_pointer_to_base_interface = self*;


    struct iterator_traits
    {
        using iterator_category    = BaseIteratorCategory;
        using value_type           = value_type;
        using const_value_type     = const value_type;
        using reference            = value_type&;
        using const_reference      = const_value_type&;
        using raw_pointer          = self*;
        using pointer              = value_type*;
        using const_pointer        = const_value_type*;
        using difference_type      = ptrdiff_t;
    };

    using iterator = polymorphic_iterator_template
        <
        typename iterator_traits::iterator_category,
        typename iterator_traits::value_type,
        typename iterator_traits::difference_type,
        typename iterator_traits::pointer,
        typename iterator_traits::reference
        >;

    using const_iterator = polymorphic_iterator_template
        <
        typename iterator_traits::iterator_category,
        typename iterator_traits::const_value_type,
        typename iterator_traits::difference_type,
        typename iterator_traits::const_pointer,
        typename iterator_traits::const_reference
        >;

    using reverse_iterator = polymorphic_iterator_template
        <
        typename iterator_traits::iterator_category,
        typename iterator_traits::value_type,
        typename iterator_traits::difference_type,
        typename iterator_traits::pointer,
        typename iterator_traits::reference,
        true
        >;

    using const_reverse_iterator = polymorphic_iterator_template
        <
        typename iterator_traits::iterator_category,
        typename iterator_traits::const_value_type,
        typename iterator_traits::difference_type,
        typename iterator_traits::const_pointer,
        typename iterator_traits::const_reference,
        true
        >;

    using df_pre_order_hierarchical_iterator =
        df_hierarchical_iterator_template<iterator, df_traverse_algorithm::pre_order>;
    using const_df_pre_order_hierarchical_iterator =
        df_hierarchical_iterator_template<const_iterator, df_traverse_algorithm::pre_order>;
    using reverse_df_pre_order_hierarchical_iterator =
        df_hierarchical_iterator_template<reverse_iterator, df_traverse_algorithm::pre_order, true>;
    using const_reverse_df_pre_order_hierarchical_iterator =
        df_hierarchical_iterator_template<const_reverse_iterator, df_traverse_algorithm::pre_order, true>;

    using df_post_order_hierarchical_iterator =
        df_hierarchical_iterator_template<iterator, df_traverse_algorithm::post_order>;
    using const_df_post_order_hierarchical_iterator =
        df_hierarchical_iterator_template<const_iterator, df_traverse_algorithm::post_order>;
    using reverse_df_post_order_hierarchical_iterator =
        df_hierarchical_iterator_template<reverse_iterator, df_traverse_algorithm::post_order, true>;
    using const_reverse_df_post_order_hierarchical_iterator =
        df_hierarchical_iterator_template<const_reverse_iterator, df_traverse_algorithm::post_order, true>;

    using bf_hierarchical_iterator =
        bf_hierarchical_iterator_template<iterator>;
    using const_bf_hierarchical_iterator =
        bf_hierarchical_iterator_template<const_iterator>;
    using reverse_bf_hierarchical_iterator =
        bf_hierarchical_iterator_template<reverse_iterator, true>;
    using const_reverse_bf_hierarchical_iterator =
        bf_hierarchical_iterator_template<const_reverse_iterator, true>;

public:
    virtual void push_back(const value_type &) = 0;
    virtual void push_back(value_type &&) = 0;
    virtual bool is_leaf() const = 0;
    virtual bool is_composite() const = 0;
    virtual bool is_reference() const noexcept = 0;
    virtual bool is_null_reference() const noexcept = 0;
    virtual bool is_traversable() const noexcept = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual size_t nested_hierarchy_size() const = 0;
    virtual raw_pointer_to_base_interface clone() const = 0;

    virtual iterator begin() = 0;
    virtual iterator end() = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual reverse_iterator rbegin() = 0;
    virtual reverse_iterator rend() = 0;
    virtual const_reverse_iterator crbegin() const = 0;
    virtual const_reverse_iterator crend() const = 0;

    virtual raw_pointer_to_base_interface get_parent()
    {
        return _parent;
    }

    virtual const raw_pointer_to_base_interface get_parent() const
    {
        return _parent;
    }

    // Pre-order iterators

    auto df_pre_order_begin()
    {
        return df_pre_order_hierarchical_iterator(begin(), end());
    }

    auto df_pre_order_end()
    {
        return df_pre_order_hierarchical_iterator(end(), end());
    }

    auto cdf_pre_order_begin() const
    {
        return const_df_pre_order_hierarchical_iterator(cbegin(), cend());
    }

    auto cdf_pre_order_end() const
    {
        return const_df_pre_order_hierarchical_iterator(cend(), cend());
    }

    auto rdf_pre_order_begin()
    {
        return reverse_df_pre_order_hierarchical_iterator(rbegin(), rend());
    }

    auto rdf_pre_order_end()
    {
        return reverse_df_pre_order_hierarchical_iterator(rend(), rend());
    }

    auto crdf_pre_order_begin() const
    {
        return const_reverse_df_pre_order_hierarchical_iterator(crbegin(), crend());
    }

    auto crdf_pre_order_end() const
    {
        return const_reverse_df_pre_order_hierarchical_iterator(crend(), crend());
    }

    // Post-order iterators

    auto df_post_order_begin()
    {
        return df_post_order_hierarchical_iterator(begin(), end());
    }

    auto df_post_order_end()
    {
        return df_post_order_hierarchical_iterator(end(), end());
    }

    auto cdf_post_order_begin() const
    {
        return const_df_post_order_hierarchical_iterator(cbegin(), cend());
    }

    auto cdf_post_order_end() const
    {
        return const_df_post_order_hierarchical_iterator(cend(), cend());
    }

    auto rdf_post_order_begin()
    {
        return reverse_df_post_order_hierarchical_iterator(rbegin(), rend());
    }

    auto rdf_post_order_end()
    {
        return reverse_df_post_order_hierarchical_iterator(rend(), rend());
    }

    auto crdf_post_order_begin() const
    {
        return const_reverse_df_post_order_hierarchical_iterator(crbegin(), crend());
    }

    auto crdf_post_order_end() const
    {
        return const_reverse_df_post_order_hierarchical_iterator(crend(), crend());
    }

    // Breadth-first iterators

    auto bf_begin()
    {
        return bf_hierarchical_iterator(begin(), end());
    }

    auto bf_end()
    {
        return bf_hierarchical_iterator();
    }

    auto cbf_begin() const
    {
        return const_bf_hierarchical_iterator(cbegin(), cend());
    }

    auto cbf_end() const
    {
        return const_bf_hierarchical_iterator();
    }

    auto rbf_begin()
    {
        return reverse_bf_hierarchical_iterator(rbegin(), rend());
    }

    auto rbf_end()
    {
        return reverse_bf_hierarchical_iterator();
    }

    auto crbf_begin() const
    {
        return const_reverse_bf_hierarchical_iterator(crbegin(), crend());
    }

    auto crbf_end() const
    {
        return const_reverse_bf_hierarchical_iterator();
    }

    bool awaits_destruction() const noexcept
    {
        return _awaits_destruction;
    }

    virtual ~abstract() noexcept
    {
    }

protected:
    virtual void set_parent(self * const ptr_to_parent)
    {
        _parent = ptr_to_parent;
    }

    virtual void mark_for_delete() noexcept
    {
        _awaits_destruction = true;
    }

    virtual void erase_awaiting_destruction()
    {
    }

protected:
    raw_pointer_to_base_interface _parent{ nullptr };

private:
    bool _awaits_destruction{ false };
};



template <class Base>
class leaf : public Base
{
    using self = leaf;
    using parent = Base;

public:
    using value_type = typename Base::iterator_traits::value_type;
    using raw_pointer_to_base_interface = typename Base::raw_pointer_to_base_interface;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = typename Base::reverse_iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;

public:
    template <class IteratorBaseType>
    class leaf_iterator_impl : public IteratorBaseType::implementation
    {
        using category = typename IteratorBaseType::iterator_category;
        using reference = typename IteratorBaseType::reference;
        using pointer = typename IteratorBaseType::pointer;
        using difference_type = typename IteratorBaseType::difference_type;

        using self = typename IteratorBaseType::implementation;
        using base_interface = typename IteratorBaseType::implementation::base_interface;

    public:
        leaf_iterator_impl() : _ptr(nullptr)
        {
        }

        reference ref() const override
        {
            return *_ptr;
        }

        pointer ptr() const override
        {
            return _ptr;
        }

        self &inc() override
        {
            return *this;
        }

        self &dec() override
        {
            return *this;
        }

        self *clone() override
        {
            return new leaf_iterator_impl();
        }

        bool equal(const base_interface *another) const override
        {
            // always nullptr
            return true;
        }

        bool less(const base_interface *another) const override
        {
            return false;
        }

        bool greater(const base_interface *another) const override
        {
            return false;
        }

        reference offset(const difference_type diff) const override
        {
            static pointer var = nullptr;
            return *var;
        }

    private:
        typename IteratorBaseType::pointer _ptr;
    };

public:
    leaf() : parent()
    {
    }

    void push_back(const value_type &another) override
    {

    }

    void push_back(value_type &&another) override
    {

    }

    iterator begin() override
    {
        return end();
    }

    iterator end() override
    {
        return iterator(std::make_unique<leaf_iterator_impl<iterator>>());
    }

    const_iterator cbegin() const override
    {
        return cend();
    }

    const_iterator cend() const override
    {
        return const_iterator(std::make_unique<leaf_iterator_impl<const_iterator>>());
    }

    reverse_iterator rbegin() override
    {
        return rend();
    }

    reverse_iterator rend() override
    {
        return reverse_iterator(std::make_unique<leaf_iterator_impl<reverse_iterator>>());
    }

    const_reverse_iterator crbegin() const override
    {
        return crend();
    }

    const_reverse_iterator crend() const override
    {
        return const_reverse_iterator(std::make_unique<leaf_iterator_impl<const_reverse_iterator>>());
    }

    void clear() override
    {
    }

    size_t size() const override final
    {
        return 0;
    }

    bool empty() const override final
    {
        return true;
    }

    size_t nested_hierarchy_size() const override final
    {
        return 0;
    }

    bool is_leaf() const override final
    {
        return true;
    }

    bool is_composite() const override final
    {
        return false;
    }

    bool is_reference() const noexcept override final
    {
        return false;
    }

    bool is_null_reference() const noexcept override final
    {
        return false;
    }

    bool is_traversable() const noexcept override final
    {
        return true;
    }

    raw_pointer_to_base_interface clone() const override
    {
        return new self(*this);
    }
};



template <class Base, template <class T> class Container>
class composite : public Base
{  
    using self = composite;
    using parent = Base;

public:
    using value_type = typename Base::iterator_traits::value_type;
    using raw_pointer_to_base_interface = typename Base::raw_pointer_to_base_interface;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = typename Base::reverse_iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;

    using container_type = typename Container<value_type>::type;
    using initializer_list = std::initializer_list<value_type>;

public:
    template <class IteratorTag, class Iterator>
    struct iterator_impl_specific
    {
        using difference_type = typename Iterator::difference_type;

        static bool less(const Iterator &a, const Iterator &b)
        {
            return a < b;
        }

        static bool greater(const Iterator &a, const Iterator &b)
        {
            return a > b;
        }

        static typename Iterator::reference offset(const Iterator &it, const difference_type diff)
        {
            return it[diff];
        }

        static Iterator advance_forwards(Iterator &it, const difference_type diff)
        {
            return it + diff;
        }

        static Iterator advance_backwards(Iterator &it, const difference_type diff)
        {
            return it - diff;
        }

        static difference_type difference(const Iterator &a, const Iterator &b)
        {
            return a - b;
        }
    };

    template <class Iterator>
    struct iterator_impl_specific<std::bidirectional_iterator_tag, Iterator>
    {
        using difference_type = typename Iterator::difference_type;

        static bool less(const Iterator &a, const Iterator &b)
        {
            return false;
        }

        static bool greater(const Iterator &a, const Iterator &b)
        {
            return false;
        }

        static typename Iterator::reference offset(const Iterator &it, const difference_type diff)
        {
            static typename Iterator::pointer obj = nullptr;
            return *obj;
        }

        static Iterator advance_forwards(Iterator &it, const difference_type diff)
        {
            return it;
        }

        static Iterator advance_backwards(Iterator &it, const difference_type diff)
        {
            return it;
        }

        static difference_type difference(const Iterator &a, const Iterator &b)
        {
            return difference_type{};
        }
    };

    template <class IteratorBaseType, class UnderlyingContainerIterator>
    class iterator_impl_template : public IteratorBaseType::implementation
    {
    public:
        using category  = typename IteratorBaseType::iterator_category;
        using reference = typename IteratorBaseType::reference;
        using pointer   = typename IteratorBaseType::pointer;
        using difference_type = typename IteratorBaseType::difference_type;

        using self = typename IteratorBaseType::implementation;
        using base_interface = typename self::base_interface;

    public:
        iterator_impl_template(UnderlyingContainerIterator &it) : _it(it)
        {
        }

        reference ref() const override
        {
            return *_it;
        }

        pointer ptr() const override
        {
            return _it.operator->();
        }

        self &inc() override
        {
            ++_it; return *this;
        }

        self &dec() override
        {
            --_it; return *this;
        }

        self *clone() override
        {
            return new iterator_impl_template(_it);
        }

        bool equal(const base_interface *another) const override
        {
            return _it == upcast(another)->_it;
        }

        bool less(const base_interface *another) const override
        {
            return iterator_impl_specific<category, UnderlyingContainerIterator>::less(_it, upcast(another)->_it);
        }

        bool greater(const base_interface *another) const override
        {
            return iterator_impl_specific<category, UnderlyingContainerIterator>::greater(_it, upcast(another)->_it);
        }

        reference offset(const difference_type diff) const override
        {
            return iterator_impl_specific<category, UnderlyingContainerIterator>::offset(_it, diff);
        }

        void advance_forwards(const difference_type diff) override
        {
            _it = iterator_impl_specific<category, UnderlyingContainerIterator>::advance_forwards(_it, diff);
        }

        void advance_backwards(const difference_type diff) override
        {
            _it = iterator_impl_specific<category, UnderlyingContainerIterator>::advance_forwards(_it, diff);
        }

        difference_type difference(const base_interface *another) const override
        {
            return iterator_impl_specific<category, UnderlyingContainerIterator>::difference(_it, upcast(another)->_it);
        }

        UnderlyingContainerIterator get_cont_iterator() const
        {
            return _it;
        }

    private:
        iterator_impl_template *upcast(const base_interface *ptr) const
        {
            return (iterator_impl_template*)ptr;
        }

    private:
        UnderlyingContainerIterator _it;
    };

    using iterator_impl               = iterator_impl_template<iterator, typename container_type::iterator>;
    using reverse_iterator_impl       = iterator_impl_template<reverse_iterator, typename container_type::reverse_iterator>;

    using const_iterator_impl         = iterator_impl_template<const_iterator, typename container_type::const_iterator>;
    using const_reverse_iterator_impl = iterator_impl_template<const_reverse_iterator, typename container_type::const_reverse_iterator>;

public:
    composite() : parent()
    {}

    composite(initializer_list &&init_list)
        : parent(), children(std::move(init_list))
    {
    }

    composite(const self &another)
    {
        _parent = another._parent;
        for (auto &ptr : another.children)
        {
            children.emplace_back(ptr->clone());
        }
    }

    composite(self &&another) :
        children(std::move(another.children))
    {
        _parent = another._parent;
    }

    self &operator=(const self &another)
    {
        set_parent(another.get_parent());
        for (auto &ptr : another.children)
        {
            children.emplace_back(ptr->clone());
        }
        return *this;
    }

    self &operator=(self &&another)
    {
        set_parent(another.get_parent());
        another.set_parent(nullptr);
        children = std::move(another.children);
    }

    container_type &cont()
    {
        return children;
    }

    const container_type &cont() const
    {
        return children;
    }

    void push_back(const value_type &another) override
    {
        push_back_impl<std::is_copy_constructible<value_type>::value>(another);
    }

    void push_back(value_type &&another) override
    {
        another->set_parent(this);
        children.push_back(std::move(another));
    }


    enum references_remove_mode
    {
        do_not_track_references,
        remove_references,
        nullify_references
    };

    template <class Pred>
    void remove_if(Pred &func, const references_remove_mode mode = remove_references)
    {
        remove_if__mark_for_delete(func);

        if (mode != do_not_track_references)
        {
            remove_if__handle_references(mode == nullify_references);
        }

        erase_awaiting_destruction();
    }

    // linear iterators

    iterator begin() override
    {
        return iterator(std::make_unique<iterator_impl>(children.begin()));
    }

    iterator end() override
    {
        return iterator(std::make_unique<iterator_impl>(children.end()));
    }

    reverse_iterator rbegin() override
    {
        return reverse_iterator(std::make_unique<reverse_iterator_impl>(children.rbegin()));
    }

    reverse_iterator rend() override
    {
        return reverse_iterator(std::make_unique<reverse_iterator_impl>(children.rend()));
    }

    const_iterator cbegin() const override
    {
        return const_iterator(std::make_unique<const_iterator_impl>(children.cbegin()));
    }

    const_iterator cend() const override
    {
        return const_iterator(std::make_unique<const_iterator_impl>(children.cend()));
    }

    const_reverse_iterator crbegin() const override
    {
        return const_reverse_iterator(std::make_unique<const_reverse_iterator_impl>(children.crbegin()));
    }

    const_reverse_iterator crend() const override
    {
        return const_reverse_iterator(std::make_unique<const_reverse_iterator_impl>(children.crend()));
    }

    void clear() override
    {
        children.clear();
    }

    size_t size() const override
    {
        return children.size();
    }

    bool empty() const override final
    {
        return children.empty();
    }

    size_t nested_hierarchy_size() const override final
    {
        size_t count = size();
        for (auto it = cbegin(); it != cend(); ++it)
        {
            count += (*it)->nested_hierarchy_size();
        }

        return count;
    }

    bool is_leaf() const override final
    {
        return false;
    }

    bool is_composite() const override final
    {
        return true;
    }

    bool is_reference() const noexcept override final
    {
        return false;
    }

    bool is_null_reference() const noexcept override final
    {
        return false;
    }

    bool is_traversable() const noexcept override final
    {
        return true;
    }

    raw_pointer_to_base_interface clone() const override
    {
        return new self(*this);
    }

private:
    template <bool is_copy_constructible>
    void push_back_impl(const value_type &val)
    {
        children.push_back(val);
        children.back()->set_parent(this);
    }

    template<>
    void push_back_impl<false>(const value_type &val)
    {
    }

    template <class Pred>
    void remove_if__mark_for_delete(Pred &func)
    {
        for (auto &obj : children)
        {
            if (!obj->is_reference())
            {
                if (func(obj))
                {
                    obj->mark_for_delete();
                }
                else if (obj->is_composite())
                {
                    static_cast<self*>(obj.get())->remove_if__mark_for_delete(func);
                }
            }
        }
    }

    void remove_if__handle_references(const bool nullify)
    {
        for (auto &obj : children)
        {
            if (obj->is_reference())
            {
                auto ref = static_cast<typename parent::reference*>(obj.get());
                if (nullify)
                {
                    ref->reset();
                }
                else
                {
                    ref->mark_for_delete();
                }
            }
            else if (obj->is_composite())
            {
                static_cast<self*>(obj.get())->remove_if__handle_references(nullify);
            }
        }
    }

protected:
    void erase_awaiting_destruction() override
    {
        auto it = std::remove_if(children.begin(), children.end(),
            [](const smart_ptr &obj) { return obj->awaits_destruction(); }
        );

        children.erase(it, children.end());

        for (auto &obj : children)
        {
            obj->erase_awaiting_destruction();
        }
    }

    void mark_for_delete() noexcept override
    {
        parent::mark_for_delete();
        for (auto &obj : children)
        {
            obj->mark_for_delete();
        }
    }

protected:
    container_type children;
};



template <class Base>
class reference : public Base
{
    using self = reference;
    using parent = Base;

public:
    using smart_ptr = typename Base::smart_ptr;
    using value_type = typename Base::value_type;
    using raw_pointer_to_base_interface = typename Base::raw_pointer_to_base_interface;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = typename Base::reverse_iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;

    using null_reference_type = null_reference<Base>;

public:
    reference()
    {
    }

    void init()
    {
        reset();
    }

    virtual null_reference_type *obtain_null_reference() = 0;

    virtual void remove_null_reference()
    {
        // Mainly useful to delete null_reference objects which were constructed on heap.
    }

    explicit reference(const smart_ptr &source)
    {
        assign(source);
    }

    reference(const self &another)
    {
        if (!another.is_null())
        {
            ptr = another.ptr;
            _traversable = another._traversable;
        }
        else
        {
            reset();
        }
    }

    reference(self &&another)
    {
        if (!another.is_null())
        {
            ptr = another.ptr;
            _traversable = another._traversable;
            another.reset();
        }
        else
        {
            reset();
        }
    }

    self &operator=(const smart_ptr &source)
    {
        assign(source);
        return *this;
    }

    self &operator=(const self &another)
    {
        if (!another.is_null())
        {
            ptr = another.ptr;
            _traversable = another._traversable;
        }
        else
        {
            reset();
        }

        return *this;
    }

    self &operator=(self &&another)
    {
        if (!another.is_null())
        {
            ptr = another.ptr;
            _traversable = another._traversable;
            another.reset();
        }
        else
        {
            reset();
        }

        return *this;
    }

    void assign(const smart_ptr &source)
    {
        if (is_null() && !source->is_null_reference())
        {
            remove_null_reference();
        }

        if (!source->is_reference())
        {          
            ptr = source.get();
        }
        else
        {
            self *ref_obj = static_cast<self*>(source.get());
            if (!ref_obj->is_null())
            {
                ptr = ref_obj->ptr;
                _traversable = ref_obj->_traversable;
            }
            else
            {
                reset();
            }
        }
    }

    void push_back(const value_type &another) override
    {
        ptr->push_back(another);
    }

    void push_back(value_type &&another) override
    {
        ptr->push_back(std::move(another));
    }

    iterator begin() override
    {
        return ptr->begin();
    }

    iterator end() override
    {
        return ptr->end();
    }

    reverse_iterator rbegin() override
    {
        return ptr->rbegin();
    }

    reverse_iterator rend() override
    {
        return ptr->rend();
    }

    const_iterator cbegin() const override
    {
        return ptr->cbegin();
    }

    const_iterator cend() const override
    {
        return ptr->cend();
    }

    const_reverse_iterator crbegin() const override
    {
        return ptr->crbegin();
    }

    const_reverse_iterator crend() const override
    {
        return ptr->crend();
    }

    void clear() override
    {
        ptr->clear();
    }

    size_t size() const override final
    {
        return is_traversable() ? ptr->size() : 0;
    }

    bool empty() const override final
    {
        return ptr->empty();
    }

    size_t nested_hierarchy_size() const override final
    {
        return is_traversable() ? ptr->nested_hierarchy_size() : 0;
    }

    bool is_leaf() const override final
    {
        return ptr->is_leaf();
    }

    bool is_composite() const override final
    {
        return ptr->is_composite();
    }

    bool is_reference() const noexcept override final
    {
        return true;
    }

    bool is_null_reference() const noexcept override final
    {
        return ptr ? ptr->is_null_reference() : true;
    }

    bool is_traversable() const noexcept override final
    {
        return _traversable;
    }

    void reset()
    {
        if (ptr && ptr->is_null_reference())
        {
            return;
        }

        ptr = obtain_null_reference();
    }

    bool is_null() const
    {
        return is_null_reference();
    }

    void set_traversable(const bool value)
    {
        _traversable = value;
    }

    bool points_to(const smart_ptr &another_ptr) const noexcept
    {
        return ptr == another_ptr.get();
    }

    raw_pointer_to_base_interface get() const noexcept
    {
        return ptr;
    }

protected:
    raw_pointer_to_base_interface ptr{nullptr};

private:
    bool _traversable{false};
};


template <class Base>
class null_reference : public Base
{
    using self = null_reference;
    using parent = Base;

public:
    using smart_ptr = typename Base::smart_ptr;
    using value_type = typename Base::value_type;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = typename Base::reverse_iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;

public:
    null_reference()
    {
    }

    void push_back(const value_type &another) override
    {
        
    }

    void push_back(value_type &&another) override
    {
       
    }

    iterator begin() override
    {
        return iterator();
    }

    iterator end() override
    {
        return iterator();
    }

    reverse_iterator rbegin() override
    {
        return reverse_iterator();
    }

    reverse_iterator rend() override
    {
        return reverse_iterator();
    }

    const_iterator cbegin() const override
    {
        return const_iterator();
    }

    const_iterator cend() const override
    {
        return const_iterator();
    }

    const_reverse_iterator crbegin() const override
    {
        return const_reverse_iterator();
    }

    const_reverse_iterator crend() const override
    {
        return const_reverse_iterator();
    }

    void clear() override
    {
        
    }

    size_t size() const override final
    {
        return size_t();
    }

    bool empty() const override final
    {
        return true;
    }

    size_t nested_hierarchy_size() const override final
    {
        return size_t();
    }

    bool is_leaf() const override final
    {
        return false;
    }

    bool is_composite() const override final
    {
        return false;
    }

    bool is_reference() const noexcept override final
    {
        return true;
    }

    bool is_null_reference() const noexcept override final
    {
        return true;
    }

    bool is_traversable() const noexcept override final
    {
        return false;
    }
};



template <class Category,
    class T,
    class Distance,
    class Pointer,
    class Reference,
    bool reversed
>
class polymorphic_iterator_template : public std::iterator<Category, T, Distance, Pointer, Reference>
{
    using self = polymorphic_iterator_template;
public:
    const static bool is_reversed = reversed;

    class common_implementation
    {
    public:
        using self = common_implementation;
        using base_interface = common_implementation;

    public:
        virtual reference ref() const = 0;
        virtual pointer ptr() const = 0;
        virtual self &inc() = 0;
        virtual self &dec() = 0;
        virtual self *clone() = 0;
        virtual bool equal(const self *another) const = 0;
        virtual bool less(const self *another) const = 0;
        virtual bool greater(const self *another) const = 0;
        virtual void advance_forwards(const difference_type diff) = 0;
        virtual void advance_backwards(const difference_type diff) = 0;
        virtual difference_type difference(const self *another) const = 0;
        virtual reference offset(const difference_type diff) const = 0;
        virtual ~common_implementation() {}
    };

    using implementation = common_implementation;
    using pointer_to_implementation = std::unique_ptr<implementation>;

public:
    polymorphic_iterator_template()
    {
    }

    explicit polymorphic_iterator_template(pointer_to_implementation &&iter_impl) :
        impl(std::move(iter_impl))
    {
    }

    polymorphic_iterator_template(const self &another) :
        impl(another.impl->clone())
    {
    }

    polymorphic_iterator_template(self &&another) :
        impl(std::move(another.impl))
    {
    }

    self &operator=(const self &another)
    {
        if (*this != another)
        {
            impl.reset(another.impl->clone());
        }
        return *this;
    }

    self &operator=(self &&another)
    {
        impl = std::move(another.impl);
        return *this;
    }

    bool operator==(const self &another) const
    {
        if (impl)
        {
            return impl->equal(another.impl.get());
        }
        else
        {
            return another.impl.get() == nullptr;
        }
    }

    bool operator!=(const polymorphic_iterator_template &another) const
    {
        return !(*this == another);
    }

    reference operator*() const
    {
        return impl->ref();
    }

    pointer operator->() const
    {
        return impl->ptr();
    }

    self& operator++()
    {
        impl->inc();
        return *this;
    }

    self& operator--()
    {
        impl->dec();
        return *this;
    }

    self operator++(int) const
    {
        self new_it(*this);
        ++new_it;
        return new_it;
    }

    self operator--(int) const
    {
        self new_it(*this);
        --new_it;
        return new_it;
    }

    self operator+(const difference_type offset)
    {
        self new_it(*this);
        new_it.impl->advance_forwards(offset);
        return new_it;
    }

    self operator-(const difference_type offset)
    {
        self new_it(*this);
        new_it.impl->advance_backwards(offset);
        return new_it;
    }

    difference_type operator-(const self &another) const
    {
        return impl->difference(another.impl.get());
    }

    bool operator>(const self &another) const
    {
        return impl->greater(another.impl.get());
    }

    bool operator<(const self &another) const
    {
        return impl->less(another.impl.get());
    }

    bool operator>=(const self &another) const
    {
        return !(*this < another);
    }

    bool operator<=(const self &another) const
    {
        return !(*this > another);
    }

    reference operator[](const difference_type offset) const
    {
        return impl->offset(offset);
    }

    bool empty() const
    {
        return impl == nullptr;
    }

    pointer_to_implementation &get_impl()
    {
        return impl;
    }

    const pointer_to_implementation &get_impl() const
    {
        return impl;
    }

protected:
    pointer_to_implementation impl;
};



template
<
    class T,
    class Distance,
    class Pointer,
    class Reference,
    bool reversed
>
class polymorphic_iterator_template<std::bidirectional_iterator_tag, T, Distance, Pointer, Reference, reversed>
    : public polymorphic_iterator_template<std::random_access_iterator_tag, T, Distance, Pointer, Reference, reversed>
{
    using parent = polymorphic_iterator_template<std::random_access_iterator_tag, T, Distance, Pointer, Reference, reversed>;
    using self = polymorphic_iterator_template;

public:
    using iterator_category = std::bidirectional_iterator_tag;

    class bidirectional_implementation : public common_implementation
    {
    public:
        bool less(const common_implementation *another) const override
        {
            return false;
        }

        bool greater(const common_implementation *another) const override
        {
            return false;
        }

        reference offset(const difference_type diff) const override
        {
            static pointer var = nullptr;
            return *var;
        }

        virtual void advance_forwards(const difference_type diff) override
        {
        }

        virtual void advance_backwards(const difference_type diff) override
        {
        }

        virtual difference_type difference(const common_implementation *another) const override
        {
            return difference_type{};
        }
    };

    using implementation = bidirectional_implementation;
    using pointer_to_implementation = std::unique_ptr<implementation>;

public:
    polymorphic_iterator_template() : parent()
    {
    }

    explicit polymorphic_iterator_template(pointer_to_implementation &&iter_impl) :
        parent(std::forward<pointer_to_implementation>(iter_impl))
    {
    }

    polymorphic_iterator_template(const self &another)
    {
        if (another.impl)
            this->impl.reset(another.impl->clone());
        else
            this->impl.reset();
    }

    polymorphic_iterator_template(self &&another)
        : parent(std::move(another.impl))
    {
    }

    self &operator=(const self &another)
    {
        if (*this != another)
        {
            impl.reset(another.impl->clone());
        }
        return *this;
    }

    self &operator=(self &&another)
    {
        impl = std::move(another.impl);
        return *this;
    }

    self operator+(const difference_type offset) = delete;
    self operator-(const difference_type offset) = delete;
    difference_type operator-(const self &another) const = delete;
    bool operator>(const self &another) const = delete;
    bool operator<(const self &another) const = delete;
    bool operator>=(const self &another) const = delete;
    bool operator<=(const self &another) const = delete;
    reference operator[](const difference_type offset) const = delete;
};



template <class Container>
struct begin
{
    void operator()(Container &cont, typename Container::iterator &out)
    {
        out = cont.begin();
    }

    void operator()(const Container &cont, typename Container::const_iterator &out)
    {
        out = cont.cbegin();
    }

    void operator()(Container &cont, typename Container::reverse_iterator &out)
    {
        out = cont.rbegin();
    }

    void operator()(const Container &cont, typename Container::const_reverse_iterator &out)
    {
        out = cont.crbegin();
    }
};


template <class Container>
struct end
{
    void operator()(Container &cont, typename Container::iterator &out)
    {
        out = cont.end();
    }

    void operator()(const Container &cont, typename Container::const_iterator &out)
    {
        out = cont.cend();
    }

    void operator()(Container &cont, typename Container::reverse_iterator &out)
    {
        out = cont.rend();
    }

    void operator()(const Container &cont, typename Container::const_reverse_iterator &out)
    {
        out = cont.crend();
    }
};



template <class Source, class Dest>
struct inherit_const
{
    using type = typename std::conditional<std::is_const<Source>::value, typename std::add_const<Dest>::type, Dest>::type;
};


// Hierarchical iterators

template <class LinearIterator, const df_traverse_algorithm traverse_algorithm_param, bool reversed>
class df_hierarchical_iterator_template :
    public std::iterator
    <
        typename std::forward_iterator_tag,
        typename LinearIterator::value_type,
        typename LinearIterator::difference_type,
        typename LinearIterator::pointer,
        typename LinearIterator::reference
    >
{
    using self = df_hierarchical_iterator_template;

    struct node_iters
    {
        LinearIterator begin;
        LinearIterator end;
        LinearIterator current;
    };

    using stack_container_type = std::vector<node_iters>;

    template <const df_traverse_algorithm a>
    struct traverse_algorithm_impl
    {
        static void init(df_hierarchical_iterator_template &it) {}
        static void next(df_hierarchical_iterator_template &it) {}
    };

    friend struct traverse_algorithm_impl<df_traverse_algorithm::pre_order>;
    friend struct traverse_algorithm_impl<df_traverse_algorithm::post_order>;

    using traverse_algorithm = traverse_algorithm_impl<traverse_algorithm_param>;

public:
    df_hierarchical_iterator_template()
    {
    }

    explicit df_hierarchical_iterator_template(const LinearIterator &root_begin,
        const LinearIterator &root_end)
    {
        push(node_iters{ root_begin, root_end, root_begin });
        init();
    }

    df_hierarchical_iterator_template(const self &another) :
        iters(another.iters)
    {
    }

    df_hierarchical_iterator_template(self &&another) :
        iters(std::move(another.iters))
    {
    }

    self &operator=(const self &another)
    {
        iters = another.iters;
        return *this;
    }

    self &operator=(self &&another)
    {
        iters = std::move(another.iters);
        return *this;
    }

    bool operator==(const self &another) const
    {
        return iters.size() == another.iters.size() &&
            std::equal(iters.crbegin(), iters.crend(), another.iters.crbegin(),
                [](const auto &a, const auto &b) {return a.current == b.current; }
        );
    }

    bool empty() const
    {
        return iters.empty();
    }

    bool operator!=(const self &another) const
    {
        return !(*this == another);
    }

    reference operator*() const
    {
        return *top_it();
    }

    pointer operator->() const
    {
        return top_it().operator->();
    }

    self& operator++()
    {
        traverse_algorithm::next(*this);
        return *this;
    }

    self operator++(int)
    {
        self new_it(*this);
        ++new_it;
        return new_it;
    }

    void return_up()
    {
        pop();
    }

    void go_down()
    {
        using container_type = typename inherit_const<value_type, typename value_type::element_type>::type;
        container_type &cont = **top_it();
        LinearIterator _begin, _end;
        begin<container_type>()(cont, _begin);
        end<container_type>()(cont, _end);
        push(node_iters{ _begin, _end, _begin });
    }

    LinearIterator get_linear_iterator() const
    {
        return top_it();
    }

    LinearIterator get_linear_iterator_to_parent() const
    {
        const size_t size = iters.size();
        return size > 1 ? iters[size - 2].current : LinearIterator();
    }

private:
    LinearIterator &top_it()
    {
        return top().current;
    }

    const LinearIterator &top_it() const
    {
        return top().current;
    }

    LinearIterator &top_it_begin()
    {
        return top().begin;
    }

    const LinearIterator &top_it_begin() const
    {
        return top().begin;
    }

    LinearIterator &top_it_end()
    {
        return top().end;
    }

    const LinearIterator &top_it_end() const
    {
        return top().end;
    }

    void init()
    {
        traverse_algorithm::init(*this);
    }

    void push(typename stack_container_type::reference pair)
    {
        iters.push_back(pair);
    }

    void pop()
    {
        iters.pop_back();
    }

    typename stack_container_type::reference top()
    {
        return iters.back();
    }

    typename stack_container_type::const_reference top() const
    {
        return iters.back();
    }

    bool can_go_down() const
    {
        const auto &node = *top_it();
        return node->is_traversable() && node->size() > 0;
    }

private:
    template <>
    struct traverse_algorithm_impl<df_traverse_algorithm::pre_order>
    {
        static void init(df_hierarchical_iterator_template &it)
        {
        }

        static void next(df_hierarchical_iterator_template &it)
        {
            if (it.can_go_down())
            {
                it.go_down();
                return;
            }

            while ((++it.top_it()) == it.top_it_end())
            {
                if (it.iters.size() == 1) break;
                it.return_up();
            }
        }
    };

    template <>
    struct traverse_algorithm_impl<df_traverse_algorithm::post_order>
    {
        static void init(df_hierarchical_iterator_template &it)
        {
            if (it.top_it() != it.top_it_end())
            {
                while (it.can_go_down())
                {
                    it.go_down();
                }
            }
        }

        static void next(df_hierarchical_iterator_template &it)
        {
            if ((++it.top_it()) == it.top_it_end())
            {
                if (it.iters.size() != 1)
                {
                    it.return_up();
                }
                return;
            }

            while (it.can_go_down())
            {
                it.go_down();
            }
        }
    };

private:
    stack_container_type iters;
};



template <class LinearIterator, bool reversed>
class bf_hierarchical_iterator_template :
    public std::iterator
    <
        typename std::forward_iterator_tag,
        typename LinearIterator::value_type,
        typename LinearIterator::difference_type,
        typename LinearIterator::pointer,
        typename LinearIterator::reference
    >
{
    using self = bf_hierarchical_iterator_template;

    struct node_iters
    {
        LinearIterator current;
        LinearIterator end;
    };

    using queue_container_type = std::queue<node_iters>;

public:
    bf_hierarchical_iterator_template()
    {
    }

    explicit bf_hierarchical_iterator_template(const LinearIterator &root_begin, const LinearIterator &root_end)
    {
        push(node_iters{ root_begin, root_end});
    }

    bf_hierarchical_iterator_template(const self &another) :
        iters(another.iters)
    {
    }

    bf_hierarchical_iterator_template(self &&another) :
        iters(std::move(another.iters))
    {
    }

    self &operator=(const self &another)
    {
        iters = another.iters;
        return *this;
    }

    self &operator=(self &&another)
    {
        iters = std::move(another.iters);
        return *this;
    }

    bool operator==(const self &another) const
    {
        bool result = false;
        if (iters.size() != 0 && another.iters.size() != 0)
        {
            result = current_it() == another.current_it();
        }
        else if (iters.size() == 0 && another.iters.size() == 0)
        {
            result = true;
        }

        return result;
    }

    bool operator!=(const self &another) const
    {
        return !(*this == another);
    }

    reference operator*() const
    {
        return *current_it();
    }

    pointer operator->() const
    {
        return current_it().operator->();
    }

    self& operator++()
    {
        if (current_it() != current_it_end())
        {
            try_to_push_children_to_queue();
            ++current_it();
        }

        if (current_it() == current_it_end())
        {
            pop();
        }

        return *this;
    }

    self operator++(int)
    {
        self new_it(*this);
        ++new_it;
        return new_it;
    }

    bool empty() const
    {
        return iters.empty();
    }

    LinearIterator &get_linear_iterator()
    {
        return current_it();
    }

    const LinearIterator &get_linear_iterator() const
    {
        return current_it();
    }

private:
    LinearIterator &current_it()
    {
        return front().current;
    }

    const LinearIterator &current_it() const
    {
        return front().current;
    }

    LinearIterator &current_it_end()
    {
        return front().end;
    }

    const LinearIterator &current_it_end() const
    {
        return front().end;
    }

    void try_to_push_children_to_queue()
    {
        if (current_has_children())
        {
            using container_type = typename inherit_const<value_type, typename value_type::element_type>::type;
            container_type &cont = **current_it();
            LinearIterator _begin, _end;
            begin<container_type>()(cont, _begin);
            end<container_type>()(cont, _end);
            iters.emplace(node_iters{ _begin, _end });
        }
    }

    void push(typename queue_container_type::reference pair)
    {
        iters.push(pair);
    }

    void pop()
    {
        iters.pop();
    }

    typename queue_container_type::reference front()
    {
        return iters.front();
    }

    typename queue_container_type::const_reference front() const
    {
        return iters.front();
    }

    bool current_has_children()
    {
        return (*current_it())->size() > 0;
    }

private:
    queue_container_type iters;
};



template <class CompositeObjectIterator>
class iter_wrapper : public std::iterator
    <
    typename CompositeObjectIterator::iterator_category,
    typename CompositeObjectIterator::value_type::element_type
    >
{
public:
    iter_wrapper() {}

    explicit iter_wrapper(const CompositeObjectIterator &iterator) :
        it(iterator)
    {
    }

    iter_wrapper(const iter_wrapper &another) :
        it(another.it)
    {
    }

    iter_wrapper &operator=(const iter_wrapper &another)
    {
        if (*this != another)
        {
            it = another.it;
        }
        return *this;
    }

    bool operator==(const iter_wrapper &another) const
    {
        return it == another.it;
    }

    bool operator!=(const iter_wrapper &another) const
    {
        return !(*this == another);
    }

    reference operator*() const
    {
        return **it;
    }

    pointer operator->() const
    {
        return it->operator->();
    }

    iter_wrapper& operator++()
    {
        ++it;
        return *this;
    }

    iter_wrapper& operator--()
    {
        --it;
        return *this;
    }

    iter_wrapper operator++(int)
    {
        iter_wrapper new_it(*this);
        ++new_it;
        return new_it;
    }

    iter_wrapper operator--(int)
    {
        iter_wrapper new_it(*this);
        --new_it;
        return new_it;
    }

private:
    CompositeObjectIterator it;
};


template <class Iterator>
iter_wrapper<Iterator> wrap_iterator(const Iterator &iter)
{
    return iter_wrapper<Iterator>(iter);
}


} // composite_object namespace end
