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


namespace composite_object
{

template
<
    class Base,
    template <class T, class Alloc = std::allocator<T>> class Container = std::list
>
class interface;

template <class Base> class leaf;
template <class Base> class composite;

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

template <class LinearIterator, bool reversed = false>
class bf_hierarchical_iterator_template;


//

template <class Base, template <class T, class Alloc = std::allocator<T>> class Container>
class interface : public Base
{
public:
    using value_type             = std::shared_ptr<interface>;
    using const_value_type       = const std::shared_ptr<const interface>;
    using reference              = value_type&;
    using const_reference        = const_value_type&;
    using container_type         = Container<value_type>;

    using container_iterator         = typename container_type::iterator;
    using container_const_iterator   = typename container_type::const_iterator;
    using container_reverse_iterator = typename container_type::reverse_iterator;
    using container_const_reverse_iterator = typename container_type::const_reverse_iterator;

    using iterator = polymorphic_iterator_template
        <
        typename container_iterator::iterator_category,
        typename container_iterator::value_type,
        typename container_iterator::difference_type,
        typename container_iterator::pointer,
        typename container_iterator::reference
        >;

    using const_iterator = polymorphic_iterator_template
        <
        typename container_const_iterator::iterator_category,
        typename container_const_iterator::value_type,
        typename container_const_iterator::difference_type,
        typename container_const_iterator::pointer,
        typename container_const_iterator::reference
        >;

    using reverse_iterator = polymorphic_iterator_template
        <
        typename container_reverse_iterator::iterator_category,
        typename container_reverse_iterator::value_type,
        typename container_reverse_iterator::difference_type,
        typename container_reverse_iterator::pointer,
        typename container_reverse_iterator::reference,
        true
        >;

    using const_reverse_iterator = polymorphic_iterator_template
        <
        typename container_const_reverse_iterator::iterator_category,
        typename container_const_reverse_iterator::value_type,
        typename container_const_reverse_iterator::difference_type,
        typename container_const_reverse_iterator::pointer,
        typename container_const_reverse_iterator::reference,
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
    virtual bool is_leaf() const = 0;
    virtual bool is_composite() const = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual size_t nested_hierarchy_size() const = 0;

    virtual iterator begin() = 0;
    virtual iterator end() = 0;
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual reverse_iterator rbegin() = 0;
    virtual reverse_iterator rend() = 0;
    virtual const_reverse_iterator crbegin() const = 0;
    virtual const_reverse_iterator crend() const = 0;

    virtual df_pre_order_hierarchical_iterator df_pre_order_begin() = 0;
    virtual df_pre_order_hierarchical_iterator df_pre_order_end() = 0;
    virtual const_df_pre_order_hierarchical_iterator cdf_pre_order_begin() const = 0;
    virtual const_df_pre_order_hierarchical_iterator cdf_pre_order_end() const = 0;
    virtual reverse_df_pre_order_hierarchical_iterator rdf_pre_order_begin() = 0;
    virtual reverse_df_pre_order_hierarchical_iterator rdf_pre_order_end() = 0;
    virtual const_reverse_df_pre_order_hierarchical_iterator crdf_pre_order_begin() const = 0;
    virtual const_reverse_df_pre_order_hierarchical_iterator crdf_pre_order_end() const = 0;

    virtual df_post_order_hierarchical_iterator df_post_order_begin() = 0;
    virtual df_post_order_hierarchical_iterator df_post_order_end() = 0;
    virtual const_df_post_order_hierarchical_iterator cdf_post_order_begin() const = 0;
    virtual const_df_post_order_hierarchical_iterator cdf_post_order_end() const = 0;
    virtual reverse_df_post_order_hierarchical_iterator rdf_post_order_begin() = 0;
    virtual reverse_df_post_order_hierarchical_iterator rdf_post_order_end() = 0;
    virtual const_reverse_df_post_order_hierarchical_iterator crdf_post_order_begin() const = 0;
    virtual const_reverse_df_post_order_hierarchical_iterator crdf_post_order_end() const = 0;

    virtual bf_hierarchical_iterator bf_begin() = 0;
    virtual bf_hierarchical_iterator bf_end() = 0;
    virtual const_bf_hierarchical_iterator cbf_begin() const = 0;
    virtual const_bf_hierarchical_iterator cbf_end() const = 0;
    virtual reverse_bf_hierarchical_iterator rbf_begin() = 0;
    virtual reverse_bf_hierarchical_iterator rbf_end() = 0;
    virtual const_reverse_bf_hierarchical_iterator crbf_begin() const = 0;
    virtual const_reverse_bf_hierarchical_iterator crbf_end() const = 0;
};


namespace
{

template <class Base>
class interface_plug_in : public Base
{
public:
    interface_plug_in() : Base()
    {
    }

    using container_type = typename Base::container_type;
    using value_type = typename Base::value_type;

    using iterator = typename Base::iterator;
    using const_iterator = typename Base::const_iterator;
    using reverse_iterator = typename Base::reverse_iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;

    using df_pre_order_hierarchical_iterator = typename Base::df_pre_order_hierarchical_iterator;
    using const_df_pre_order_hierarchical_iterator = typename Base::const_df_pre_order_hierarchical_iterator;
    using reverse_df_pre_order_hierarchical_iterator = typename Base::reverse_df_pre_order_hierarchical_iterator;
    using const_reverse_df_pre_order_hierarchical_iterator = typename Base::const_reverse_df_pre_order_hierarchical_iterator;

    using df_post_order_hierarchical_iterator = typename Base::df_post_order_hierarchical_iterator;
    using const_df_post_order_hierarchical_iterator = typename Base::const_df_post_order_hierarchical_iterator;
    using reverse_df_post_order_hierarchical_iterator = typename Base::reverse_df_post_order_hierarchical_iterator;
    using const_reverse_df_post_order_hierarchical_iterator = typename Base::const_reverse_df_post_order_hierarchical_iterator;

    using bf_hierarchical_iterator = typename Base::bf_hierarchical_iterator;
    using const_bf_hierarchical_iterator = typename Base::const_bf_hierarchical_iterator;
    using reverse_bf_hierarchical_iterator = typename Base::reverse_bf_hierarchical_iterator;
    using const_reverse_bf_hierarchical_iterator = typename Base::const_reverse_bf_hierarchical_iterator;
};

}


template <class Base>
class leaf : public interface_plug_in<Base>
{
public:
    using parent = interface_plug_in<Base>;

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

    // linear iterators

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

    // hierarchical depth first traversal iterators

    df_pre_order_hierarchical_iterator df_pre_order_begin() override
    {
        return df_pre_order_end();
    }

    df_pre_order_hierarchical_iterator df_pre_order_end() override
    {
        return df_pre_order_hierarchical_iterator(end(), end());
    }

    const_df_pre_order_hierarchical_iterator cdf_pre_order_begin() const override
    {
        return cdf_pre_order_end();
    }

    const_df_pre_order_hierarchical_iterator cdf_pre_order_end() const override
    {
        return const_df_pre_order_hierarchical_iterator(cend(), cend());
    }

    reverse_df_pre_order_hierarchical_iterator rdf_pre_order_begin() override
    {
        return rdf_pre_order_end();
    }

    reverse_df_pre_order_hierarchical_iterator rdf_pre_order_end() override
    {
        return reverse_df_pre_order_hierarchical_iterator(rend(), rend());
    }

    const_reverse_df_pre_order_hierarchical_iterator crdf_pre_order_begin() const override
    {
        return crdf_pre_order_end();
    }

    const_reverse_df_pre_order_hierarchical_iterator crdf_pre_order_end() const override
    {
        return const_reverse_df_pre_order_hierarchical_iterator(crend(), crend());
    }
   
    df_post_order_hierarchical_iterator df_post_order_begin() override
    {
        return df_post_order_end();
    }

    df_post_order_hierarchical_iterator df_post_order_end() override
    {
        return df_post_order_hierarchical_iterator(end(), end());
    }

    const_df_post_order_hierarchical_iterator cdf_post_order_begin() const override
    {
        return cdf_post_order_end();
    }

    const_df_post_order_hierarchical_iterator cdf_post_order_end() const override
    {
        return const_df_post_order_hierarchical_iterator(cend(), cend());
    }

    reverse_df_post_order_hierarchical_iterator rdf_post_order_begin() override
    {
        return rdf_post_order_end();
    }

    reverse_df_post_order_hierarchical_iterator rdf_post_order_end() override
    {
        return reverse_df_post_order_hierarchical_iterator(rend(), rend());
    }

    const_reverse_df_post_order_hierarchical_iterator crdf_post_order_begin() const override
    {
        return crdf_post_order_end();
    }

    const_reverse_df_post_order_hierarchical_iterator crdf_post_order_end() const override
    {
        return const_reverse_df_post_order_hierarchical_iterator(crend(), crend());
    }

    bf_hierarchical_iterator bf_begin() override
    {
        return bf_hierarchical_iterator(begin(), end());
    }

    bf_hierarchical_iterator bf_end() override
    {
        return bf_hierarchical_iterator();
    }

    const_bf_hierarchical_iterator cbf_begin() const override
    {
        return const_bf_hierarchical_iterator(cbegin(), cend());
    }

    const_bf_hierarchical_iterator cbf_end() const override
    {
        return const_bf_hierarchical_iterator();
    }

    reverse_bf_hierarchical_iterator rbf_begin() override
    {
        return reverse_bf_hierarchical_iterator(rbegin(), rend());
    }

    reverse_bf_hierarchical_iterator rbf_end() override
    {
        return reverse_bf_hierarchical_iterator();
    }

    const_reverse_bf_hierarchical_iterator crbf_begin() const override
    {
        return const_reverse_bf_hierarchical_iterator(crbegin(), crend());
    }
    
    const_reverse_bf_hierarchical_iterator crbf_end() const override
    {
        return const_reverse_bf_hierarchical_iterator();
    }

    void clear() override
    {
    }

    size_t size() const override final
    {
        return 0;
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
};


template <class Base>
class composite : public interface_plug_in<Base>
{  
public:
    using parent = interface_plug_in<Base>;

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

    composite(std::initializer_list<value_type> &&init_list)
        : parent(), children(init_list)
    {
    }

    container_type &cont()
    {
        return children;
    }

    const container_type &cont() const
    {
        return children;
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

    // hierarchical iterators

    df_pre_order_hierarchical_iterator df_pre_order_begin() override
    {
        return df_pre_order_hierarchical_iterator(begin(), end());
    }

    df_pre_order_hierarchical_iterator df_pre_order_end() override
    {
        return df_pre_order_hierarchical_iterator(end(), end());
    }

    const_df_pre_order_hierarchical_iterator cdf_pre_order_begin() const override
    {
        return const_df_pre_order_hierarchical_iterator(cbegin(), cend());
    }

    const_df_pre_order_hierarchical_iterator cdf_pre_order_end() const override
    {
        return const_df_pre_order_hierarchical_iterator(cend(), cend());
    }

    reverse_df_pre_order_hierarchical_iterator rdf_pre_order_begin() override
    {
        return reverse_df_pre_order_hierarchical_iterator(rbegin(), rend());
    }

    reverse_df_pre_order_hierarchical_iterator rdf_pre_order_end() override
    {
        return reverse_df_pre_order_hierarchical_iterator(rend(), rend());
    }

    const_reverse_df_pre_order_hierarchical_iterator crdf_pre_order_begin() const override
    {
        return const_reverse_df_pre_order_hierarchical_iterator(crbegin(), crend());
    }

    const_reverse_df_pre_order_hierarchical_iterator crdf_pre_order_end() const override
    {
        return const_reverse_df_pre_order_hierarchical_iterator(crend(), crend());
    }

    df_post_order_hierarchical_iterator df_post_order_begin() override
    {
        return df_post_order_hierarchical_iterator(begin(), end());
    }

    df_post_order_hierarchical_iterator df_post_order_end() override
    {
        return df_post_order_hierarchical_iterator(end(), end());
    }

    const_df_post_order_hierarchical_iterator cdf_post_order_begin() const override
    {
        return const_df_post_order_hierarchical_iterator(cbegin(), cend());
    }

    const_df_post_order_hierarchical_iterator cdf_post_order_end() const override
    {
        return const_df_post_order_hierarchical_iterator(cend(), cend());
    }

    reverse_df_post_order_hierarchical_iterator rdf_post_order_begin() override
    {
        return reverse_df_post_order_hierarchical_iterator(rbegin(), rend());
    }

    reverse_df_post_order_hierarchical_iterator rdf_post_order_end() override
    {
        return reverse_df_post_order_hierarchical_iterator(rend(), rend());
    }

    const_reverse_df_post_order_hierarchical_iterator crdf_post_order_begin() const override
    {
        return const_reverse_df_post_order_hierarchical_iterator(crbegin(), crend());
    }

    const_reverse_df_post_order_hierarchical_iterator crdf_post_order_end() const override
    {
        return const_reverse_df_post_order_hierarchical_iterator(crend(), crend());
    }

    bf_hierarchical_iterator bf_begin() override
    {
        return bf_hierarchical_iterator(begin(), end());
    }

    bf_hierarchical_iterator bf_end() override
    {
        return bf_hierarchical_iterator();
    }

    const_bf_hierarchical_iterator cbf_begin() const override
    {
        return const_bf_hierarchical_iterator(cbegin(), cend());
    }

    const_bf_hierarchical_iterator cbf_end() const override
    {
        return const_bf_hierarchical_iterator();
    }

    reverse_bf_hierarchical_iterator rbf_begin() override
    {
        return reverse_bf_hierarchical_iterator(rbegin(), rend());
    }

    reverse_bf_hierarchical_iterator rbf_end() override
    {
        return reverse_bf_hierarchical_iterator();
    }

    const_reverse_bf_hierarchical_iterator crbf_begin() const override
    {
        return const_reverse_bf_hierarchical_iterator(crbegin(), crend());
    }

    const_reverse_bf_hierarchical_iterator crbf_end() const override
    {
        return const_reverse_bf_hierarchical_iterator();
    }

    void clear() override
    {
        children.clear();
    }

    size_t size() const override
    {
        return children.size();
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

protected:
    container_type children;
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

    polymorphic_iterator_template(const polymorphic_iterator_template &another) :
        impl(another.impl->clone())
    {
    }

    polymorphic_iterator_template(polymorphic_iterator_template &&another) :
        impl(std::move(another.impl))
    {
    }

    polymorphic_iterator_template &operator=(const polymorphic_iterator_template &another)
    {
        if (*this != another)
        {
            impl.reset(another.impl->clone());
        }
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

    self operator++(int)
    {
        self new_it(*this);
        ++new_it;
        return new_it;
    }

    self operator--(int)
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

public:
    using iterator_category = std::bidirectional_iterator_tag;

    class bidirectional_implementation : public common_implementation
    {
    public:
        using base_interface = common_implementation;

    public:
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

        virtual void advance_forwards(const difference_type diff) override
        {
        }

        virtual void advance_backwards(const difference_type diff) override
        {
        }

        virtual difference_type difference(const base_interface *another) const override
        {
            return difference_type{};
        }
    };

    using implementation = bidirectional_implementation;

public:
    polymorphic_iterator_template() : parent()
    {
    }

    explicit polymorphic_iterator_template(std::unique_ptr<implementation> &&iter_impl) :
        parent(std::forward<std::unique_ptr<implementation>>(iter_impl))
    {
    }

    polymorphic_iterator_template(const polymorphic_iterator_template &another)
        //impl(another.impl->clone())
    {
        if (another.impl)
            this->impl.reset(another.impl->clone());
        else
            this->impl.reset();
    }

    polymorphic_iterator_template(polymorphic_iterator_template &&another)
        : parent(std::move(another.impl))
    {
    }

    polymorphic_iterator_template &operator=(const polymorphic_iterator_template &another)
    {
        if (*this != another)
        {
            impl.reset(another.impl->clone());
        }
        return *this;
    }

    polymorphic_iterator_template operator+(const difference_type offset) = delete;
    polymorphic_iterator_template operator-(const difference_type offset) = delete;
    difference_type operator-(const polymorphic_iterator_template &another) const = delete;
    bool operator>(const polymorphic_iterator_template &another) const = delete;
    bool operator<(const polymorphic_iterator_template &another) const = delete;
    bool operator>=(const polymorphic_iterator_template &another) const = delete;
    bool operator<=(const polymorphic_iterator_template &another) const = delete;
    reference operator[](const difference_type offset) const = delete;
};


namespace
{
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
}


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

    df_hierarchical_iterator_template(const df_hierarchical_iterator_template &another) :
        iters(another.iters)
    {
    }

    bool operator==(const df_hierarchical_iterator_template &another) const
    {
        return iters.size() == another.iters.size() &&
            std::equal(iters.crbegin(), iters.crend(), another.iters.crbegin(),
                [](const auto &a, const auto &b) {return a.current == b.current; }
        );
    }

    bool operator!=(const df_hierarchical_iterator_template &another) const
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

    bool can_go_down()
    {
        return (*top_it())->size() > 0;
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
    bf_hierarchical_iterator_template() : level(0)
    {
    }

    explicit bf_hierarchical_iterator_template(const LinearIterator &root_begin, const LinearIterator &root_end)
        : level(0)
    {
        push(node_iters{ root_begin, root_end});
    }

    bf_hierarchical_iterator_template(const bf_hierarchical_iterator_template &another) :
        iters(another.iters), level(another.level)
    {
    }

    bool operator==(const bf_hierarchical_iterator_template &another) const
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

    bool operator!=(const bf_hierarchical_iterator_template &another) const
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
    size_t level;
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
