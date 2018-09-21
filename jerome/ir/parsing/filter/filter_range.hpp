//
//  filter_range.hpp
//
//  Created by Anton Leuski on 9/20/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Jerome is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Jerome is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with Jerome.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __jerome_ir_parsing_filter_filter_range_hpp
#define __jerome_ir_parsing_filter_filter_range_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <boost/iterator/iterator_facade.hpp>
#pragma clang diagnostic pop

namespace jerome {
  template <class Derived, class Iterator, typename Value>
  struct filter_iterator : public boost::iterator_facade<
    filter_iterator<Derived, Iterator, Value>, Value,
    boost::single_pass_traversal_tag
  >
  {
    typedef Iterator source_iterator_type;

    filter_iterator()
    : mIsAtEnd(true)
    {}

    filter_iterator(source_iterator_type inIter, bool atEnd = false)
    : mSource(inIter)
    , mIsAtEnd(atEnd)
    { _advance(); }

  private:
    friend class boost::iterator_core_access;

    typedef filter_iterator<Derived, Iterator, Value> this_t;

    source_iterator_type mSource;
    optional<Value> mValue;
    bool mIsAtEnd;

    void _advance() {
      if (mIsAtEnd) return;
      mValue = static_cast<Derived*>(this)->next(mSource);
      if (!mValue) { mIsAtEnd = true; }
    }

    void increment() { _advance(); }

    bool equal(this_t const& other) const
    {
      return this->mIsAtEnd == other.mIsAtEnd && mIsAtEnd;
    }

    Value& dereference() const { return mValue.get(); }
  };

  // bool op(value&, iterator&)

  template <class Function, class Iterator>
  struct functional_filter_iterator : public boost::iterator_facade<
    functional_filter_iterator<Function, Iterator>,
    typename Function::result_type,
    boost::single_pass_traversal_tag
  >
  {
    functional_filter_iterator()
    : mIsAtEnd(true)
    {}

    functional_filter_iterator(Function f, Iterator inIter, bool atEnd = false)
    : mFunction(f)
    , mSource(inIter)
    , mIsAtEnd(atEnd)
    { _advance(); }

    typedef Iterator source_iterator_type;
    typedef typename Function::result_type value_type;
  private:
    friend class boost::iterator_core_access;
    typedef functional_filter_iterator<Function, Iterator> this_t;

    Function mFunction;
    source_iterator_type mSource;
    mutable value_type mValue;
    bool mIsAtEnd;

    void _advance() {
      if (mIsAtEnd) return;
      mIsAtEnd = !mFunction(mValue, mSource);
    }

    void increment() { _advance(); }

    bool equal(this_t const& other) const
    {
      return this->mIsAtEnd == other.mIsAtEnd && mIsAtEnd;
    }

    value_type& dereference() const { return mValue; }
  };

  template <class F, class R>
  class filter_range :
    public boost::iterator_range<
      functional_filter_iterator<
        F, typename boost::range_iterator<R>::type>
    >
  {
  private:
    typedef F function_t;
    typedef typename boost::range_iterator<R>::type base_iter_t;
    typedef functional_filter_iterator<function_t, base_iter_t> iter_t;
    typedef boost::iterator_range<iter_t> parent_t;
  public:
    filter_range(F f, R& r)
    : parent_t(iter_t(f, boost::begin(r)), iter_t())
    {}
  };
}

#endif // defined __jerome_ir_parsing_filter_filter_range_hpp
