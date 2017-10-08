#ifndef HEADER_gubg_parse_polymorphic_tree_detail_AttributeParser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_parse_polymorphic_tree_detail_AttributeParser_hpp_ALREADY_INCLUDED

#include "gubg/parse/polymorphic_tree/ReturnCode.hpp"
#include <list>

namespace gubg { namespace parse { namespace polymorphic_tree { namespace detail {

template <typename value_type>
class AttributeParser
{
public:
    AttributeParser(const std::pair<std::size_t, std::size_t> & limits)
        : limits_(limits),
          cnt_(0)
    {
    }

    void reset()
    {
        cnt_ = 0;
        reset_();
    }

    void on_parent_open()
    {
        cnt_ = 0;
        reset_();
    }

    ReturnCode on_parent_close(value_type & vt)
    {
        if(cnt_ < limits_.first)
            return ReturnCode::Attribute_NotInTree;
        else if (cnt_ > limits_.second)
            return ReturnCode::Attribute_MultipleInTree;

        close_(vt);
        return ReturnCode::OK;
    }

    void process_element(const std::string & value)
    {
        ++cnt_;
        return process_(value);
    }

    const std::pair<std::size_t, std::size_t> & limits() const  { return limits_; }
    std::pair<std::size_t, std::size_t> & limits()              { return limits_; }

    void set_required()                                         { limits_.first = 1; }

private:
    virtual void reset_() = 0;
    virtual void close_(value_type & vt) = 0;
    virtual void process_(const std::string & value) = 0;

    std::pair<std::size_t, std::size_t> limits_;
    std::size_t cnt_;
};

template <typename value_type, typename attribute_type, typename extractor_type, typename convertor_type>
class SingleAttributeParser : public AttributeParser<value_type>
{
public:
    SingleAttributeParser(extractor_type extract, convertor_type convert)
        : AttributeParser<value_type>(std::make_pair(0,1)),
          extract_(extract),
          convert_(convert)
    {
    }

private:
    virtual void reset_()
    {
        attr_ = attribute_type();
    }
    virtual void close_(value_type & vt)
    {
        extract_(vt) = attr_;
        attr_ = attribute_type();
    }

    virtual void process_(const std::string & value)
    {
        attr_ = convert_(value);
    }

    extractor_type extract_;
    convertor_type convert_;
    attribute_type attr_;
};

template <typename value_type, typename attribute_type, typename attribute_container_type, typename convertor_type>
class MultiAttributeParser : public AttributeParser<value_type>
{
public:
    MultiAttributeParser(attribute_type value_type::*ptr, convertor_type convert)
        : ptr_(ptr),
          convert_(convert)
    {
    }

private:
    virtual void reset_()
    {
        attrs_.clear();
    }
    virtual void close_(value_type & vt)
    {
        auto & ctr = vt.*ptr_;
        ctr.insert(ctr.end(), attrs_.begin(), attrs_.end());
        attrs_.clear();
    }

    virtual void process_(const std::string & value)
    {
        attrs_.push_back(convert_(value));
    }

    attribute_container_type value_type::*ptr_;
    convertor_type convert_;
    std::list<attribute_type> attrs_;
};


} } } }

#endif
