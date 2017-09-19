#ifndef HEADER_gubg_parse_polymorphic_tree_FixedStructure_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_FixedStructure_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/Parser.hpp"
#include "gubg/parse/polymorphic_tree/AttributeParser.hpp"
#include "gubg/parse/polymorphic_tree/detail/StorageElement.hpp"

namespace gubg { namespace parse { namespace polymorphic_tree {

template <typename NodeType>
class FixedStructureElement : public Element
{
public:
    ~FixedStructureElement()
    {
        for(auto & p : attribute_parsers_)
            delete p.second;

        for(auto & p : node_parsers_)
            delete p.second;
    }

    virtual ReturnCode on_open() override
    {
        MSS_BEGIN(ReturnCode);

        for(auto & p : attribute_parsers_)
            PROPAGATE(p.second->reset());

        for(auto & p : node_parsers_)
            PROPAGATE(p.second->reset(value()));

        MSS_END();
    }

    virtual ReturnCode on_close() override
    {
        MSS_BEGIN(ReturnCode);
        for(auto & p : node_parsers_)
            PROPAGATE(p.second->valid());

        MSS_END();
    }


    virtual ReturnCode on_child_open(Element *& handler, const std::string & tag) override
    {
        MSS_BEGIN(ReturnCode);

        auto it = node_parsers_.find(tag);

        MSS_Q(it != node_parsers_.end(), return ReturnCode::Tag_Unknown);

        handler = it->second;
        MSS_END();
    }

    virtual ReturnCode on_child_close(Element * handler) override
    {
        MSS_BEGIN(ReturnCode);
        MSS_END();
    }

    virtual ReturnCode on_attribute(const std::string & key, const std::string & value) override
    {
        MSS_BEGIN(ReturnCode);
        auto it = attribute_parsers_.find(key);

        MSS_Q(it != attribute_parsers_.end(), return ReturnCode::Attribute_Unknown);

        PROPAGATE(it->second->process(value, value_));
        MSS_END();
    }

    virtual ReturnCode on_attributes_handled() override
    {
        MSS_BEGIN(ReturnCode);

        for(const auto & p : attribute_parsers_)
            PROPAGATE(p.second->valid());

        MSS_END();
    }

    NodeType & value() { return value_; }
    const NodeType & value() const { return value_; }

    template <typename ChildNodeType, typename Functor>
    FixedStructureElement<ChildNodeType> & node(const std::string & tag, const Functor & functor, bool is_required = true)
    {
        // cleanup if necessary
        {
            auto it = node_parsers_.find(tag);
            if(it != node_parsers_.end())
                delete it->second;
        }

        using ElementImpl = detail::SingleStorageElement<NodeType, ChildNodeType, Functor>;
        ElementImpl * element = new ElementImpl(functor);
        element->required = is_required;

        node_parsers_[tag] = dynamic_cast<detail::StorageElement<NodeType> *>(element);

        return *dynamic_cast<FixedStructureElement<ChildNodeType> *>(element);
    }

    template <typename ChildNodeType, typename Functor>
    FixedStructureElement<ChildNodeType> & composite_node(const std::string & tag, const Functor & functor)
    {
        // cleanup if necessary
        {
            auto it = node_parsers_.find(tag);
            if(it != node_parsers_.end())
                delete it->second;
        }

        using ElementImpl = detail::CompositeStorageElement<NodeType, ChildNodeType, Functor>;
        ElementImpl * element = new ElementImpl(functor);

        node_parsers_[tag] = dynamic_cast<detail::StorageElement<NodeType> *>(element);

        return *dynamic_cast<FixedStructureElement<ChildNodeType> *>(element);
    }

    template <typename Functor>
    void attribute(const std::string & key, const Functor & functor, bool is_required = true)
    {
        // cleanup
        auto it = attribute_parsers_.find(key);
        if(it != attribute_parsers_.end())
            delete it->second;

        using P = detail::SingleAttributeParser<NodeType, Functor>;
        P * p = new P(functor);
        p->required = is_required;
        attribute_parsers_[key] = p;
    }

    template <typename Functor>
    void composite_attribute(const std::string & key, const Functor & functor)
    {
        // cleanup
        auto it = attribute_parsers_.find(key);
        if(it != attribute_parsers_.end())
            delete it->second;

        using P = detail::CompositeAttributeParser<NodeType, Functor>;
        P * p = new P(functor);
        attribute_parsers_[key] = p;
    }


private:
    std::map<std::string, AttributeParser<NodeType> *> attribute_parsers_;
    std::map<std::string, detail::StorageElement<NodeType> *> node_parsers_;
    NodeType value_;
};


} } }

#endif
