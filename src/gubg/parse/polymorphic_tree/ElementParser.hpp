#ifndef HEADER_gubg_parse_polymorphic_tree_ElementParser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_ElementParser_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include <memory>

namespace gubg { namespace parse { namespace polymorphic_tree {

class ElementParser
{
public:
    using ElementPtr = std::shared_ptr<ElementParser>;

    ElementParser() {}

    virtual ~ElementParser() = default;

    virtual void reset() = 0;
    virtual void on_open() = 0;
    virtual ReturnCode on_close() = 0;

    virtual ReturnCode on_child_open(ElementPtr & handler, const std::string & tag) = 0;
    virtual ReturnCode on_attribute(const std::string & key, const std::string & value) = 0;
    virtual ReturnCode on_attributes_handled() = 0;
};

} } }

#endif
