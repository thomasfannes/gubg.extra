#ifndef HEADER_gubg_parse_polymorphic_tree_RootElement_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_RootElement_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ElementParser.hpp"
#include "gubg/parse/polymorphic_tree/detail/ChildFactoryContainer.hpp"

namespace gubg { namespace parse { namespace polymorphic_tree {

template <typename value_type>
class RootElement : public ElementParser, public detail::ChildFactoryContainer<value_type>
{
public:
    explicit RootElement(value_type & vt)
        : value_(vt)
    {
    }

    virtual void reset()
    {
        value_ = value_type();
        detail::ChildFactoryContainer<value_type>::reset();
    }

    virtual void on_open()
    {
        value_ = value_type();
        detail::ChildFactoryContainer<value_type>::on_parent_open();
    }

    virtual ReturnCode on_close()
    {
        return detail::ChildFactoryContainer<value_type>::on_parent_close(value_);
    }

    virtual ReturnCode on_child_open(ElementPtr & handler, const std::string & tag)
    {
        return detail::ChildFactoryContainer<value_type>::on_child_open(handler, tag);
    }
    virtual ReturnCode on_attribute(const std::string & key, const std::string & value)
    {
        return ReturnCode::Attribute_NotInTree;
    }
    virtual ReturnCode on_attributes_handled()
    {
        return ReturnCode::OK;
    }

private:
    value_type & value_;
};


} } }

#endif
