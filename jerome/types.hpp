//
//  types.hpp
//
//  Created by Anton Leuski on 9/30/11.
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

#ifndef __jerome_types_hpp__
#define __jerome_types_hpp__

#define __STDC_LIMIT_MACROS

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <ostream>
#include <stdint.h>


#if !defined(_WIN32) && !defined(__WIN32__) && !defined(WIN32)
#  define JEROME_SYMBOL_EXPORT __attribute__((__visibility__("default")))
#  define JEROME_SYMBOL_IMPORT
#  define JEROME_SYMBOL_VISIBLE __attribute__((__visibility__("default")))
#endif

#include <jerome/type/string.hpp>
#include <jerome/type/error.hpp>

namespace ptr = std;

namespace jerome {
  template <class T>
  using shared_ptr = std::shared_ptr<T>;
  template <class T>
  using weak_ptr = std::weak_ptr<T>;
  template <class T>
  using enable_shared_from_this = std::enable_shared_from_this<T>;

  template <class T, class U>
  shared_ptr<T> dynamic_pointer_cast(shared_ptr<U> const& r)
  {
    return std::dynamic_pointer_cast<T, U>(r);
  }
  template <class T, class U>
  shared_ptr<T> static_pointer_cast(shared_ptr<U> const& r)
  {
    return std::static_pointer_cast<T, U>(r);
  }

  // basic ordered array of other classes
  template <class T>
  using List = std::vector<T>;
  // hash map
  template <class V>
  using Set = std::unordered_set<V>;
  template <class K, class V>
  using Map = std::unordered_map<K, V>;
  template <class V>
  using StringMap = std::unordered_map<String, V>;

  template <typename Iter>
  struct KeyGetter
    : std::unary_function<typename Iter::value_type,
      typename Iter::value_type::first_type>
  {
    const typename Iter::value_type::first_type& operator () (
      const typename Iter::value_type& p) const
    {
      return p.first;
    }

  };

  typedef StringMap<uint32_t>   StringIDMap;
  typedef StringMap<String>     StringStringMap;

  struct ReferenceClass {};

  template <typename T>
  struct Identity
    : std::unary_function<const T&, const T&>
  {
    const T& operator () (const T& v) const
    {
      return v;
    }

  };

  struct ProgressDelegate
  {
    virtual ~ProgressDelegate()
    {
    }

    virtual bool noteProgress(double inProgress)
    {
      return true;
    }

  };

  //	template <class C, typename E>
  //	inline bool contains(const C& container, const E& element) {
  //		return container.find(element) != container.end();
  //	}
  //
  //	template <class C, typename E>
  //	struct object_contained_in_set : public std::unary_function<E, bool> {
  //		object_contained_in_set(const C& set) : mSet(set) {}
  //		bool operator()(const E& element) const { return mSet.find(element) !=
  // mSet.end(); }
  //	private:
  //		const C& mSet;
  //	};

  template <class C, typename E>
  inline std::function<bool(const E&)> make_object_contained_in_set(
    const C& container)
  {
    return [&container] (const E &element) { return container.find(element) !=
                                                    container.end(); };
  }

  template <class Impl>
  class ReferenceClassInterface
    : public ReferenceClass
  {
  public:
    typedef Impl implementation_type;

    ReferenceClassInterface() = default;

  protected:
    explicit ReferenceClassInterface(implementation_type* inData)
      : mData(inData)
    {
    }

    explicit ReferenceClassInterface(
      const shared_ptr<implementation_type>& inData)
      : mData(inData)
    {
    }

    explicit ReferenceClassInterface(shared_ptr<implementation_type>&& inData)
      : mData(std::forward<shared_ptr<implementation_type>>(inData))
    {
    }

    implementation_type& implementation()
    {
      return *mData;
    }

    const implementation_type& implementation() const
    {
      return *mData;
    }

  private:
    shared_ptr<implementation_type> mData;
  };

}

namespace std {
  template <class T>
  ostream& operator << (ostream& outs, const vector<T>& obj)
  {
    outs << "{";
    bool  f = false;
    for (auto& v : obj) {
      if (f) outs << ", ";
      f = true;
      outs << v;
    }
    outs << "}";
    return outs;
  }
}

#endif // defined __jerome_types_hpp__