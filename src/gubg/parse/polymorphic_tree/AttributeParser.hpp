#ifndef HEADER_gubg_parse_polymorphic_tree_AttributeParser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_AttributeParser_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include "gubg/mss.hpp"
#include <functional>
#include <map>
#include <string>

namespace gubg { namespace parse { namespace polymorphic_tree {

template <typename NodeType>
struct AttributeParser
{
    virtual ~AttributeParser() {}

    virtual ReturnCode process(const std::string & value, NodeType & vt) = 0;
    virtual ReturnCode valid() const = 0;
    virtual ReturnCode reset() = 0;
};

} } }

#include "gubg/parse/polymorphic_tree/detail/AttributeParser.hpp"

#endif
