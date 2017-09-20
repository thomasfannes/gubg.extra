#ifndef HEADER_gubg_parse_polymorphic_tree_detail_FixedStructure_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_detail_FixedStructure_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/Parser.hpp"

namespace gubg { namespace parse { namespace polymorphic_tree {

template <typename NodeType> class FixedStructureElement;

namespace detail {

template <typename NodeType> struct StorageElement : public virtual Element
{
    virtual ReturnCode reset(NodeType & parent_node) = 0;
    virtual ReturnCode valid() const = 0;
};

template <typename ParentNodeType, typename ChildNodeType> struct NodeStorageElement
        : public virtual FixedStructureElement<ChildNodeType>, public virtual StorageElement<ParentNodeType>
{
    virtual ReturnCode reset(ParentNodeType & parent_node) override
    {
        parent_node_ = &parent_node;
        return ReturnCode::OK;
    }

    virtual ReturnCode on_open() override
    {
        return FixedStructureElement<ChildNodeType>::on_open();
    }

    virtual ReturnCode on_child_open(Element *& handler, const std::string & tag) override
    {
        return FixedStructureElement<ChildNodeType>::on_child_open(handler, tag);
    }

    virtual ReturnCode on_child_close(Element * handler)
    {
        return FixedStructureElement<ChildNodeType>::on_child_close(handler);
    }

    virtual ReturnCode on_attribute(const std::string & key, const std::string & value) override
    {
        return FixedStructureElement<ChildNodeType>::on_attribute(key, value);
    }

    virtual ReturnCode on_attributes_handled() override
    {
        return FixedStructureElement<ChildNodeType>::on_attributes_handled();
    }

protected:
    ParentNodeType * parent_node() const { return parent_node_; }

private:
    ParentNodeType * parent_node_;
};


template <typename ParentNodeType, typename ChildNodeType, typename Functor> struct SingleStorageElement
        : public virtual NodeStorageElement<ParentNodeType, ChildNodeType>
{
    explicit SingleStorageElement(const Functor & functor)
        : functor_(functor)
    {
    }

    virtual ReturnCode reset(ParentNodeType & parent_node) override
    {
        is_parsed_ = false;
        return NodeStorageElement<ParentNodeType, ChildNodeType>::reset(parent_node);
    }

    virtual ReturnCode on_open() override
    {
        MSS_BEGIN(ReturnCode);
        MSS_Q(!is_parsed_, return ReturnCode::Tag_MultipleInTree);

        PROPAGATE( (NodeStorageElement<ParentNodeType, ChildNodeType>::on_open()) );
        MSS_END();
    }

    virtual ReturnCode valid() const override
    {
        MSS_BEGIN(ReturnCode);
        MSS_Q(is_parsed_ || !required, return ReturnCode::Tag_NotInTree);

        MSS_END();
    }

    virtual ReturnCode on_close() override
    {
        MSS_BEGIN(ReturnCode);

        PROPAGATE(FixedStructureElement<ChildNodeType>::on_close());
        is_parsed_ = true;

        MSS(!!this->parent_node());

        functor_(*this->parent_node(), this->value());

        MSS_END();
    }

    bool required = true;

private:
    bool is_parsed_ = false;
    Functor functor_;
};


template <typename ParentNodeType, typename ChildNodeType, typename Functor> struct CompositeStorageElement
        : public NodeStorageElement<ParentNodeType, ChildNodeType>
{
    explicit CompositeStorageElement(const Functor & functor)
        : functor_(functor)
    { }

    virtual ReturnCode valid() const override { return ReturnCode::OK; }

    virtual ReturnCode on_close() override
    {
        MSS_BEGIN(ReturnCode);
        PROPAGATE(FixedStructureElement<ChildNodeType>::on_close());

        MSS(!!this->parent_node());
        functor_(*this->parent_node(), this->value());

        MSS_END();
    }

private:
    Functor functor_;
};

} } } }

#endif
