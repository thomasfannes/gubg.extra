#ifndef HEADER_gubg_parse_polymorphic_tree_ReturnCode_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_ReturnCode_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg { namespace parse { namespace polymorphic_tree {

enum class ReturnCode
{
    OK,
    Error,

    Tag_Unknown,
    Tag_NotInTree,
    Tag_MultipleInTree,
    Attribute_Unknown,
    Attribute_NotInTree,
    Attribute_MultipleInTree,
};

std::ostream & operator<<(std::ostream & str, ReturnCode code)
{
    switch(code)
    {
#define CASE(TYPE) case ReturnCode::TYPE: str << #TYPE; break
        CASE(OK);
        CASE(Error);
        CASE(Tag_Unknown);
        CASE(Tag_NotInTree);
        CASE(Tag_MultipleInTree);
        CASE(Attribute_Unknown);
        CASE(Attribute_NotInTree);
        CASE(Attribute_MultipleInTree);
#undef CASE

    default:
        str << "unknown";
    }

    return str;
}

} } }

#define PROPAGATE(VAL) { if (VAL != ReturnCode::OK) return VAL; }

#endif
