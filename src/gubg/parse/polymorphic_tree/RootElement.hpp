#ifndef HEADER_gubg_parse_polymorphic_tree_RootElement_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_RootElement_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/Parser.hpp"
#include "gubg/parse/polymorphic_tree/AttributeParser.hpp"
#include "gubg/parse/polymorphic_tree/detail/StorageElement.hpp"

namespace gubg { namespace parse { namespace polymorphic_tree {

template <typename NodeType>
class RootElement : public virtual Element
{
public:
    RootElement(NodeType & value, const std::string & key)
        : value_(value),
          key_(key),
          level_(0)
    {
    }

    ~RootElement()
    {
        for(auto & p : attribute_parsers_)
            delete p.second;

        for(auto & p : node_parsers_)
            delete p.second;
    }

    virtual ReturnCode on_open() override
    {
        MSS_BEGIN(ReturnCode);

        if (level_ == 1)
        {
            for(auto & p : attribute_parsers_)
                PROPAGATE(p.second->reset());

            for(auto & p : node_parsers_)
               PROPAGATE(p.second->reset(value_));
        }

        MSS_END();
    }

    virtual ReturnCode on_close() override
    {
        MSS_BEGIN(ReturnCode);

        if (level_ == 1)
        {
            for(auto & p : node_parsers_)
               PROPAGATE(p.second->valid());
        }

        MSS_END();
    }


    virtual ReturnCode on_child_open(Element *& handler, const std::string & tag) override
    {
        MSS_BEGIN(ReturnCode);

        if (false) {}
        else if (level_ == 0)
        {
            MSS(tag == key_);
            handler = this;
            level_ = 1;
        }
        else if (level_ == 1)
        {
            auto it = node_parsers_.find(tag);
            MSS_Q(it != node_parsers_.end(), return ReturnCode::Tag_Unknown);
            handler = it->second;
            level_ = 2;
        }
        else
        {
            MSS(false);
        }
        MSS_END();
    }

    virtual ReturnCode on_child_close(Element * /*handler*/) override
    {
        MSS_BEGIN(ReturnCode);
        MSS(level_ != 0);
        --level_;
        MSS_END();
    }

    virtual ReturnCode on_attribute(const std::string & key, const std::string & value) override
    {
        MSS_BEGIN(ReturnCode);
        MSS(level_ == 1);

        auto it = attribute_parsers_.find(key);

        MSS_Q(it != attribute_parsers_.end(), return ReturnCode::Attribute_Unknown);

        PROPAGATE(it->second->process(value, value_));
        MSS_END();
    }

    virtual ReturnCode on_attributes_handled() override
    {
        MSS_BEGIN(ReturnCode);

        if (level_ == 1)
        {
            for(const auto & p : attribute_parsers_)
                PROPAGATE(p.second->valid());
        }

        MSS_END();
    }

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
    NodeType & value_;
    std::string key_;
    unsigned int level_ = 0;
};


} } }

#endif
