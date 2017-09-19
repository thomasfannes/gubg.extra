#ifndef HEADER_gubg_parse_polymorphic_tree_Parser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_Parser_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include "gubg/parse/tree/Parser.hpp"
#include "gubg/mss.hpp"
#include <stack>
#include <list>
#include <sstream>
#include <iostream>

namespace gubg { namespace parse { namespace polymorphic_tree {

class Parser;

class Element
{
public:
    Element() {}

    virtual ~Element() = 0;

private:
    friend class Parser;

    virtual ReturnCode on_open() = 0;
    virtual ReturnCode on_close() = 0;

    virtual ReturnCode on_child_open(Element *& handler, const std::string & tag) = 0;
    virtual ReturnCode on_child_close(Element * handler) = 0;

    virtual ReturnCode on_attribute(const std::string & key, const std::string & value) = 0;
    virtual ReturnCode on_attributes_handled() { return ReturnCode::OK; }
};

inline Element::~Element()
{

}

struct Parser : private tree::Parser_crtp<Parser>
{
#define HANDLE(VAL) { if (VAL != ReturnCode::OK) { handle_error(VAL); return false; } }

    bool set_root(Element * element)
    {
        MSS_BEGIN(bool);
        MSS(is_parsing_ == false);
        MSS(elements_.empty());
        MSS(!root_);
        root_ = element;
        MSS_END();
    }

    bool process(const std::string & content)
    {
        MSS_BEGIN(bool);
        MSS(!!root_);

        HANDLE(root_->on_open());

        bool success = Parser_crtp<Parser>::process(content);
        if(!success)
        {
            clean_up_();
            return false;
        }

        HANDLE(root_->on_close());
        MSS_END();
    }

private:
    virtual void handle_error(ReturnCode code)
    {
        std::cout << "Error: " << code << " in tree at [";

        for(auto it = current_path_.begin(); it != current_path_.end(); ++it)
            std::cout << (it != current_path_.begin() ? "/" : "") << *it;

        std::cout << "]" << std::endl;
    }



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

        Element * handler = nullptr;
        MSS(!!top_());
        HANDLE(top_()->on_child_open(handler, tag));

        push_(handler);
        MSS(!!handler);

        HANDLE(handler->on_open());

        MSS_END();
    }

    bool tree_node_close()
    {
        MSS_BEGIN(bool);

        // close the current open
        HANDLE(top_()->on_close());

        Element * handler = pop_();
        HANDLE(top_()->on_child_close(handler));

        current_path_.pop_back();

        MSS_END();
    }

    bool tree_attr(const std::string & key, const std::string & value)
    {
        MSS_BEGIN(bool);

        current_path_.push_back(key);

        HANDLE(top_()->on_attribute(key, value));

        current_path_.pop_back();
        MSS_END();
    }

    bool tree_attr_done()
    {
        MSS_BEGIN(bool);

        HANDLE(top_()->on_attributes_handled());
        MSS_END();
    }

    Element * pop_()
    {
        assert(!elements_.empty());
        Element * cur = elements_.top();
        elements_.pop();
        return cur;
    }

    Element * top_()
    {
        if(elements_.empty())
            return root_;
        else
            return elements_.top();
    }

    void push_(Element * element)
    {
        elements_.push(element);
    }

    void clean_up_()
    {
        while(!elements_.empty())
        {
            Element * handler = pop_();
            top_()->on_child_close(handler);
        }
    }

    std::list<std::string> current_path_;
    std::stack<Element *> elements_;
    Element * root_ = nullptr;


    std::ostringstream text_;
    bool is_parsing_ = false;
};


} } }

#endif
