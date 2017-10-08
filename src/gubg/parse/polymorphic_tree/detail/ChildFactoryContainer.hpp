#ifndef HEADER_gubg_parse_polymorphic_tree_detail_ChildFactoryContainer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_detail_ChildFactoryContainer_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/detail/ChildFactory.hpp"
#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include <memory>
#include <map>

namespace gubg { namespace parse { namespace polymorphic_tree { namespace detail {

template <typename value_type> class ChildFactoryContainer
{
public:
    template <typename child_value_type, typename setter_type, typename creator_function>
    auto single_child(const std::string & tag, setter_type setter, creator_function creator)
    {
        using ChildParser = SingleChildFactory<value_type, child_value_type, setter_type, creator_function>;
        auto p = std::make_shared<ChildParser>(setter, creator);

        child_factories_[tag] = p;

        return p;
    }

    template <typename child_value_type, typename creator_function>
    auto single_child(const std::string & tag, child_value_type value_type::*ptr, creator_function creator)
    {
        auto setter = [=](value_type & vt, const child_value_type & child_type) { vt.*ptr = child_type; };
        return single_child<child_value_type>(tag, setter, creator);
    }

    template <typename child_value_type, typename inserter_type, typename creator_function>
    auto multi_child(const std::string & tag, inserter_type inserter, creator_function creator)
    {
        using ChildParser = MultiChildFactory<value_type, child_value_type, inserter_type, creator_function>;
        auto p = std::make_shared<ChildParser>(inserter, creator);

        child_factories_[tag] = p;

        return p;
    }

    template <typename child_value_type, typename child_value_container_type, typename creator_function>
    auto multi_child(const std::string & tag, child_value_container_type value_type::*ptr, creator_function creator)
    {
        auto inserter = [=](value_type & vt, auto first, auto last)
        {
            auto & ctr = vt.*ptr;
            ctr.insert(ctr.end(), first, last);
        };
    }

    void reset()
    {
        for(auto & p : child_factories_)
            p.second->reset();
    }

    void on_parent_open()
    {
        for(auto & p : child_factories_)
            p.second->on_parent_open();
    }

    ReturnCode on_parent_close(value_type & vt)
    {
        for(auto & p : child_factories_)
        {
            auto v = p.second->on_parent_close(vt);
            if (v != ReturnCode::OK)
                return v;
        }

        return ReturnCode::OK;
    }

    ReturnCode on_child_open(std::shared_ptr<ElementParser> & handler, const std::string & tag)
    {
        handler = nullptr;

        auto it = child_factories_.find(tag);
        if (it == child_factories_.end())
            return ReturnCode::Tag_Unknown;

        handler = it->second->child_parser();
        return ReturnCode::OK;
    }

private:
    std::map<std::string, std::shared_ptr<IChildFactory<value_type> > > child_factories_;
};

} } } }

#endif
