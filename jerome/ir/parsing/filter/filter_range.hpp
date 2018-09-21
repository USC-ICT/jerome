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
    typedef Iterator source_iterator_type;
    typedef typename Function::result_type value_type;
  private:
    // making the internals mutable. Iterator concept asserts that
    // dereferencing is non-mutable. If we do a transform, we might not have
    // a value from the current state of the source iterator and need to
    // advance it. Or, the transform is costly, so we want to cache the
    // result.
    // We can do pre-roll, when we compute and cache the value after
    // advancement. But that means we have to pre-roll in the constructor
    // as well so the first dereference has something. It requires that
    // the source iterator is ready to dereference at the point of assignment.
    struct Storage {
      Function function;
      source_iterator_type source;
      value_type value;
      bool isAtEnd;
      bool hasValue;
      Storage(Function f, Iterator inIter)
      : function(f)
      , source(inIter)
      , isAtEnd(false)
      , hasValue(false)
      {}
      Storage(Iterator inIter)
      : source(inIter)
      , isAtEnd(true)
      , hasValue(false)
      {}
      Storage()
      : isAtEnd(true)
      , hasValue(false)
      {}
      void advance() {
        hasValue = false;
      }
      value_type& get() {
        if (hasValue || isAtEnd) return value;
        isAtEnd = !function(value, source);
        hasValue = true;
        return value;
      }
      bool equals(Storage const& other) {
        return other.isAtEnd == isAtEnd && other.source == source;
      }
    };
  public:
    functional_filter_iterator(Iterator inIter)
    : mStorage(inIter)
    {}

    functional_filter_iterator(Function f, Iterator inIter)
    : mStorage(f, inIter)
    {}

  private:
    friend class boost::iterator_core_access;
    typedef functional_filter_iterator<Function, Iterator> this_t;
    mutable Storage mStorage;

    void increment() { mStorage.advance(); }

    bool equal(this_t const& other) const
    {
      return this->mStorage.equals(other.mStorage);
    }

    value_type& dereference() const { return mStorage.get(); }
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
    : parent_t(iter_t(f, boost::begin(r)), iter_t(boost::end(r)))
    {}
  };
}

#endif // defined __jerome_ir_parsing_filter_filter_range_hpp
