//
//  parameterized.hpp
//
//  Created by Anton Leuski on 8/5/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#ifndef __jerome_math_parameters_parameterized_hpp__
#define __jerome_math_parameters_parameterized_hpp__

#include <jerome/math/parameters/types.hpp>
#include <jerome/math/parameters/domain.hpp>

namespace jerome {
namespace math {
namespace parameters {
namespace detail {
template<typename T>
struct Visitor {
  typedef T value_type;
  virtual ~Visitor()
  {
  }
};

template<typename T>
struct Mutating : public Visitor<T> {
  virtual void visit(T& v) = 0;
};

template<typename T>
struct NonMutating : public Visitor<T> {
  virtual void visit(const T& v) = 0;
};
}

typedef detail::NonMutating<value_vector::value_type> GetValue;
typedef detail::NonMutating<range_vector::value_type> GetRange;

struct GetTitle : public detail::NonMutating<title_vector::value_type> {
  GetTitle* source;
  String  prefix;

	GetTitle()
	: source(nullptr)
	, prefix()
	{}

  GetTitle(GetTitle* inSource, const String& inPrefix)
    : source(inSource), prefix(inPrefix)
  {
  }

  void visit(const String& v)
  {
		if (source)
			source->visit(prefix + v);
  }
};

namespace detail {

template<class V>
struct Collector : public V {
  typedef typename V::value_type value_type;
  typedef List<value_type>       vector_type;
  vector_type vector;
  void visit(const value_type& v) override
  {
    vector.push_back(v);
  }
};

template<class Visitor, class Visitable>
typename Collector<Visitor>::vector_type
collect(const Visitable& v)
{
  Collector<Visitor> collector;
  v.accept(collector);
  return collector.vector;
}

struct empty_type {};
}

struct SetValue : detail::Mutating<double> {
  typedef value_vector::const_iterator iterator;
  
  SetValue(iterator b, iterator e)
  : iter(b)
  , end(e)
  {}
  
  void visit(value_type& ioValue) override
  {
    if (iter != end)
      ioValue = *iter++;
  }

private:
  iterator iter;
  iterator end;
};

template<class Derived, class Base = detail::empty_type>
struct Parameterized : public Base {
  using Base::Base;

  value_vector values() const
  {
    return detail::collect<GetValue, Derived>(*derived_this());
  }

  title_vector titles() const
  {
    return detail::collect<GetTitle, Derived>(*derived_this());
  }

  range_vector ranges() const
  {
    return detail::collect<GetRange, Derived>(*derived_this());
  }

  void setValues(const value_vector& inValues)
  {
    SetValue visitor(inValues.begin(), inValues.end());
    derived_this()->accept(visitor);
  }

  protected:

    Derived*derived_this()
    {
      return static_cast<Derived*>(this);
    }

    const Derived*derived_this() const
    {
      return static_cast<const Derived*>(this);
    }
};

// you can either define the required accept methods yourself, or use the
// Parameterized template. In the latter case, you have to specify a template
// method:
//  template <class Visitor, class Visitable>
//  void visit(Visitor& visitor, Visitable& visitable)

template<class Derived, class Base = detail::empty_type>
struct ParameterizedHelper : public Parameterized<Derived, Base> {
  typedef Parameterized<Derived, Base> parent_type;
  using parent_type::parent_type;

  using Parameterized<Derived, Base>::derived_this;

  typedef ParameterizedHelper<Derived, Base> this_type;
  typedef Base                               base_type;
  template<class Visitor>
  void accept(Visitor& visitor)
  {
    Derived::visit(visitor, *derived_this());
  }

  template<class Visitor>
  void accept(Visitor& visitor) const
  {
    Derived::visit(visitor, *derived_this());
  }
};

value_vector parseVector(const String& inString);

struct Parameter : public Parameterized<Parameter> {
  typedef double value_type;
  Parameter(const String& inTitle, value_type inValue, double inMinimum, double inMaximum)
    : mTitle(inTitle), mValue(inValue), mRangeDomain(inMinimum, inMaximum)
  {
  }

  Parameter(const Parameter& inSource)
    : mTitle(inSource.title()), mValue(inSource.value()), mRangeDomain(inSource.range())
  {
  }

  const String&title() const
  {
    return mTitle;
  }

  value_type&value()
  {
    return mValue;
  }

  value_type value() const
  {
    return mValue;
  }

  RangeDomain&range()
  {
    return mRangeDomain;
  }

  const RangeDomain&range() const
  {
    return mRangeDomain;
  }

  operator value_type() const {return mValue;
  }

  void accept(GetTitle& inVisitor) const
  {
    inVisitor.visit(title());
  }

  void accept(GetValue& inVisitor) const
  {
    inVisitor.visit(value());
  }

  void accept(GetRange& inVisitor) const
  {
    inVisitor.visit(range());
  }

  void accept(SetValue& inVisitor)
  {
    inVisitor.visit(value());
  }

  private:

    String      mTitle;
    value_type  mValue;
    RangeDomain mRangeDomain;
};
}
}
}

#endif // defined __jerome_math_parameters_parameterized_hpp__
