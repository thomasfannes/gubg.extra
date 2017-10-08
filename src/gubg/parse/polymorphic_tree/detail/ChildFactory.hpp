#ifndef HEADER_gubg_parse_polymorphic_tree_detail_ChildFactory_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_detail_ChildFactory_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include "gubg/parse/polymorphic_tree/ElementParser.hpp"
#include <memory>
#include <list>
#include <limits>

namespace gubg { namespace parse { namespace polymorphic_tree {

template <typename value_type> class TypedParser;

namespace detail {

template <typename value_type> class IChildFactory
{
public:
    IChildFactory()
        : limits_(0, std::numeric_limits<std::size_t>::max()),
          cnt_(0)
    {
    }

    void reset()
    {
        cnt_ = 0;
    }

    void on_parent_open()
    {
        cnt_ = 0;
        open_();
    }

    ReturnCode on_parent_close(value_type & vt)
    {
        if(cnt_ < limits_.first)
            return ReturnCode::Tag_NotInTree;
        else if (cnt_ > limits_.second)
            return ReturnCode::Tag_MultipleInTree;

        close_(vt);
        return ReturnCode::OK;
    }
    std::shared_ptr<ElementParser> child_parser()
    {
        ++cnt_;
        return child_parser_();
    };

private:
    virtual void reset_() = 0;
    virtual void open_() = 0;
    virtual void close_(value_type & vt) = 0;
    virtual std::shared_ptr<ElementParser> child_parser_() = 0;

    std::pair<std::size_t, std::size_t> limits_;
    std::size_t cnt_;
};

template <
        typename parent_value_type,
        typename child_value_type,
        typename value_inserter,
        typename creator_function> class MultiChildFactory : public IChildFactory<parent_value_type>
{
public:
    MultiChildFactory(value_inserter insert, creator_function create)
        : insert_(insert),
          create_(create)
    {
    }

private:
    virtual void reset_() { child_.reset(); results_.clear(); }

    virtual void open_() {}

    virtual void close_(parent_value_type & vt)
    {
        if (child_)
        {
            results_.push_back(child_->take_value());
            child_.reset();

            insert_(vt, results_.begin(), results_.end());
            results_.clear();
        }
    }

    virtual std::shared_ptr<ElementParser> child_parser_()
    {
        if (!child_)
            child_ = create_();
        else
            results_.push_back(child_->take_value());

        // reset the parser
        child_->reset();

        return child_;
    }

private:
    std::shared_ptr<TypedParser<child_value_type>> child_;

    std::list<child_value_type> results_;
    value_inserter insert_;
    creator_function create_;
};

template <
        typename parent_value_type,
        typename child_value_type,
        typename value_setter,
        typename creator_function
        > class SingleChildFactory : public IChildFactory<parent_value_type>
{
public:
    SingleChildFactory(value_setter set, creator_function create)
        : set_(set),
          create_(create)
    {
    }

private:
    virtual void reset_() { child_.reset();}
    virtual void open_() {}

    virtual void close_(parent_value_type & vt)
    {
        if (child_)
        {
            set_(vt, child_->take_value());
            child_.reset();
        }
    }

    virtual std::shared_ptr<ElementParser> child_parser_()
    {
        child_ = create_();
        child_->reset();

        return child_;
    }

private:
    std::shared_ptr<TypedParser<child_value_type>> child_;
    value_setter set_;
    creator_function create_;
};


} } } }

#endif
