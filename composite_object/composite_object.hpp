
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
#include <cstddef>


namespace composite_object
{

class composite_object;
class content;
using container_class = std::list<composite_object>;

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


class composite_object
{
    using self = composite_object;

    struct flags
    {
        bool is_leaf;
        bool is_reference;

        flags():
            is_leaf(false),
            is_reference(false)
        {
        }
    };

public:
    composite_object()
    {
    }

    template <class content_type>
    composite_object(content_type &&_content):
        _content_ptr(new content_type(std::move(_content)))
    {
    }

    composite_objectstd::unique_ptr<content>

    bool is_leaf() const noexcept
    {
        return _flags.is_leaf;
    }

    bool is_reference() const noexcept
    {
        return _flags.is_reference;
    }

private:
    std::unique_ptr<container_class> _children_ptr;
    std::unique_ptr<content> _content_ptr;
    flags _flags;
};


class content
{
    virtual void on_creation() {}
    virtual void on_placing(composite_object *container) {}
    virtual void on_child_addition(composite_object *container) {}
    virtual void on_child_deletion(composite_object *container) {}
};


} // composite_object