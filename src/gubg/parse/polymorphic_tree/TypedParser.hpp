#ifndef HEADER_gubg_parse_polymorphic_tree_TypedParser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_TypedParser_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ElementParser.hpp"
#include "gubg/parse/polymorphic_tree/detail/AttributeContainer.hpp"
#include "gubg/parse/polymorphic_tree/detail/ChildFactoryContainer.hpp"

namespace gubg { namespace parse { namespace polymorphic_tree {

template <typename value_type> class TypedParser
        : public ElementParser, public detail::AttributeContainer<value_type>, public detail::ChildFactoryContainer<value_type>
{
public:
    TypedParser()
        : allocator_([]() {return value_type(); })
    {}

    void reset() override
    {
        detail::AttributeContainer<value_type>::reset();
        detail::ChildFactoryContainer<value_type>::reset();
    }

    void on_open() override
    {
        value_ = allocator_();

        detail::AttributeContainer<value_type>::on_parent_open();
        detail::ChildFactoryContainer<value_type>::on_parent_open();
    }
    ReturnCode on_close() override
    {
        return detail::ChildFactoryContainer<value_type>::on_parent_close(value_);
    }

    ReturnCode on_child_open(ElementPtr & handler, const std::string & tag) override
    {
        return detail::ChildFactoryContainer<value_type>::on_child_open(handler, tag);
    }

    ReturnCode on_attribute(const std::string & key, const std::string & value) override
    {
        return detail::AttributeContainer<value_type>::on_attribute(key, value);
    }
    ReturnCode on_attributes_handled() override
    {
        return detail::AttributeContainer<value_type>::on_attributes_handled(value_);
    }

public:
    value_type take_value()
    {
        return value_;
    }

    void set_allocator(const std::function<value_type ()> & allocator)
    {
        allocator_ = allocator;
    }

private:
    value_type value_;
    std::function<value_type ()> allocator_;
};


} } }

#endif
