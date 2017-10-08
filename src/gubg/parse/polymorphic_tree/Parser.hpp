#ifndef HEADER_gubg_parse_polymorphic_tree_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_Parser_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include "gubg/parse/polymorphic_tree/ElementParser.hpp"
#include "gubg/parse/tree/Parser.hpp"
#include "gubg/mss.hpp"
#include <stack>
#include <list>
#include <sstream>
#include <iostream>

namespace gubg { namespace parse { namespace polymorphic_tree {

class Parser : private tree::Parser_crtp<Parser>
{
public:
    using ElementPtr = std::shared_ptr<ElementParser>;

    Parser() {}

    void set_root(ElementPtr element)
    {
        root_ = element;
    }

    bool process(const std::string & content)
    {
        MSS_BEGIN(bool);
        MSS(!!root_);
        MSS(elements_.empty());
        root_->on_open();
        push_(root_);

        bool success = Parser_crtp<Parser>::process(content);
        if(!success)
        {
            clean_up_();
            return false;
        }

        error_ = root_->on_close();
        MSS(error_);

        MSS_END();
    }

    const std::list<std::string> & current_path() const { return current_path_; }
    ReturnCode error_code() const { return error_; }

private:
    Parser(const Parser &) = delete;
    Parser & operator=(const Parser &) = delete;

    friend class tree::Parser_crtp<Parser>;

    bool tree_text(const std::string & text)
    {
        text_ << text;
        return true;
    }

    bool tree_node_open(const std::string & tag)
    {
        MSS_BEGIN(bool);

        current_path_.push_back(tag);

        ElementPtr child = nullptr;

        ElementPtr parent = top_();
        if (parent)
        {
            error_ = parent->on_child_open(child, tag);
            MSS(error_);
        }

        push_(child);

        if (child)
            child->on_open();

        MSS_END();
    }

    bool tree_node_close()
    {
        MSS_BEGIN(bool);

        // close the current open
        ElementPtr child = pop_();

        if(child)
        {
            error_ = child->on_close();
            MSS(error_);
        }

        current_path_.pop_back();

        MSS_END();
    }

    bool tree_attr(const std::string & key, const std::string & value)
    {
        MSS_BEGIN(bool);

        current_path_.push_back(key);

        ElementPtr cur = top_();
        if(cur)
        {
            error_ = cur->on_attribute(key, value);
            MSS(error_);
        }

        current_path_.pop_back();
        MSS_END();
    }

    bool tree_attr_done()
    {
        MSS_BEGIN(bool);

        ElementPtr cur = top_();
        if (cur)
        {
            error_ = cur->on_attributes_handled();
            MSS(error_);
        }

        MSS_END();
    }

    ElementPtr pop_()
    {
        assert(!elements_.empty());

        ElementPtr cur = elements_.top();
        elements_.pop();
        return cur;
    }

    ElementPtr top_()
    {
        assert(!elements_.empty());
        return elements_.top();
    }

    void push_(ElementPtr element)
    {
        elements_.push(element);
    }

    void clean_up_()
    {
        while(!elements_.empty())
        {
            ElementPtr handler = pop_();
            if (handler)
                handler->on_close();
        }
    }

    std::stack<ElementPtr> elements_;
    ElementPtr root_;

    std::list<std::string> current_path_;
    ReturnCode error_;
    std::ostringstream text_;
};


} } }

#endif
