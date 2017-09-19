#ifndef HEADER_gubg_parse_polymorphic_tree_detail_AttributeParser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_detail_AttributeParser_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/AttributeParser.hpp"

namespace gubg { namespace parse { namespace polymorphic_tree { namespace detail {

template <typename NodeType, typename Functor>
struct SingleAttributeParser : public AttributeParser<NodeType>
{
    explicit SingleAttributeParser(const Functor & f = Functor()) : f_(f) {}

    bool required = true;

    virtual ReturnCode reset() override { is_parsed_ = false; return ReturnCode::OK; }
    virtual ReturnCode valid() const override
    {
        MSS_BEGIN(ReturnCode);
        MSS_Q(!required || is_parsed_, return ReturnCode::Attribute_NotInTree);
        MSS_END();
    }
    virtual ReturnCode process(const std::string &value, NodeType & vt) override
    {
        MSS_BEGIN(ReturnCode);

        MSS_Q(!is_parsed_, return ReturnCode::Attribute_MultipleInTree);
        f_(vt, value);
        is_parsed_ = true;

        MSS_END();
    }
private:
    bool is_parsed_ = false;
    Functor f_;

};

template <typename NodeType, typename Functor>
struct CompositeAttributeParser : public AttributeParser<NodeType>
{
    explicit CompositeAttributeParser(const Functor & f = Functor()) : f_(f) {}

    virtual ReturnCode reset() override  { return ReturnCode::OK; }
    virtual ReturnCode valid() const override { return ReturnCode::OK; }
    virtual ReturnCode process(const std::string &value, NodeType & vt) override
    {
        MSS_BEGIN(ReturnCode);
        f_(vt, value);
        MSS_END();
    }

private:
    Functor f_;
};

} } } }

#endif
