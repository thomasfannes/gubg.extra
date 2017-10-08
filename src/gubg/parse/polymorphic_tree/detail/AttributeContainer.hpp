#ifndef HEADER_gubg_parse_polymorphic_tree_detail_AttributeContainer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_detail_AttributeContainer_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/detail/AttributeParser.hpp"
#include "gubg/parse/polymorphic_tree/ElementParser.hpp"
#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include <memory>
#include <map>

namespace gubg { namespace parse { namespace polymorphic_tree { namespace detail {

template <typename value_type>
class AttributeContainer
{
public:
    using AttributeParserPtr = std::shared_ptr<AttributeParser<value_type>>;

    template <typename attribute_type, typename extractor_type, typename convertor_type>
    auto single_attr(const std::string & key, extractor_type extractor, convertor_type convertor)
    {
        using AttrParser = SingleAttributeParser<value_type, attribute_type, extractor_type, convertor_type>;
        auto p = std::make_shared<AttrParser>(extractor, convertor);

        attributes_handler_[key] = p;

        return p;
    }

    template <typename attribute_type, typename value_type_, typename convertor_type>
    auto single_attr(const std::string & key, attribute_type value_type_::*ptr, convertor_type convertor)
    {
        auto extractor = [=](value_type & vt) -> attribute_type & { return vt.*ptr; };
        return single_attr<attribute_type>(key, extractor, convertor);
    }


    template <typename attribute_type, typename extractor_type>
    auto single_attr(const std::string & key, extractor_type extractor)
    {
        auto convertor = [](const std::string & v) { return v; };
        return single_attr<attribute_type>(key, extractor, convertor);
    }

    template <typename attribute_type, typename value_type_>
    auto single_attr(const std::string & key, attribute_type value_type_::*ptr)
    {
        auto convertor = [](const std::string & v) { return v; };
        return single_attr(key, ptr, convertor);
    }





    template <typename attribute_container_type, typename convertor_type>
    auto multi_attr(const std::string & key, attribute_container_type value_type::*ptr, convertor_type convertor)
    {
        using attribute_type = decltype(convertor(std::string()));

        using AttrParser = MultiAttributeParser<value_type, attribute_type, attribute_container_type, convertor_type>;
        AttrParser p = std::make_shared<AttrParser>(ptr, convertor);

        attributes_handler_[key] = p;

        return p;
    }

    template <typename attribute_container_type, typename convertor_type>
    auto multi_attr(const std::string & key, attribute_container_type value_type::*ptr)
    {
        return multi_attr(key, ptr, [](const std::string & v) {return v; });
    }

    void reset()
    {
        for(auto & p : attributes_handler_)
            p.second->reset();
    }

    void on_parent_open()
    {
        for(auto & p : attributes_handler_)
            p.second->on_parent_open();
    }

    ReturnCode on_attributes_handled(value_type & vt)
    {
        for(auto & p : attributes_handler_)
        {
            auto v = p.second->on_parent_close(vt);
            if (v != ReturnCode::OK)
                return v;
        }

        return ReturnCode::OK;
    }

    ReturnCode on_attribute(const std::string & key, const std::string & value)
    {
        auto it = attributes_handler_.find(key);
        if (it == attributes_handler_.end())
            return ReturnCode::Attribute_Unknown;

        it->second->process_element(value);
        return ReturnCode::OK;
    }

private:
    std::map<std::string, AttributeParserPtr> attributes_handler_;
};

} } } }

#endif
