//
//  ordered_map.hpp
//
//  Created by Anton Leuski on 8/28/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#ifndef __jerome_type_ordered_multimap_hpp__
#define __jerome_type_ordered_multimap_hpp__

#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <jerome/types.hpp>

namespace jerome {

// An associate container ordered by the insertion order
// I do not expect the data to be large, so implement it as
// vector of pairs. If optimization is needed, replace with boost::multi_index
// implementation.

  template <
    typename Key,
    typename Value
    >
  class ordered_multimap
  {
  public:
    typedef Key key_type;
    typedef std::pair<Key, Value> value_type;
    typedef Value mapped_type;

  private:
    typedef std::vector<value_type> container_type;

  public:
    ordered_multimap() = default;

    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;

    iterator begin()
    {
      return mContainer.begin();
    }

    iterator end()
    {
      return mContainer.end();
    }

    const_iterator begin() const
    {
      return mContainer.begin();
    }

    const_iterator end() const
    {
      return mContainer.end();
    }

    template <class ... Args>
    void emplace_back(Args&& ... args)
    {
      mContainer.emplace(mContainer.end(), std::forward<Args>(args) ...);
    }

    template <class ... Args>
    void emplace_front(Args&& ... args)
    {
      mContainer.emplace(mContainer.begin(), std::forward<Args>(args) ...);
    }

    template <class ... Args>
    void emplace(Args&& ... args)
    {
      emplace_back(std::forward<Args>(args) ...);
    }

    optional<const Value&> operator [] (const key_type& inKey) const
    {
      return at(inKey);
    }

    optional<const Value&> at(const key_type& inKey) const
    {
      auto i = boost::find_if(mContainer,
        [&] (const value_type& x) {
        return x.first == inKey;
      });
      return i == end()
        ? optional<const Value&>()
        : optional<const Value&>(i->second);
    }

    const Value& at(const key_type& inKey, const mapped_type& inDefault) const
    {
      auto i = boost::find_if(mContainer,
        [&] (const value_type& x) {
        return x.first == inKey;
      });
      return i == end() ? inDefault : i->second;
    }
    
    void replace(const key_type& inKey, const mapped_type& inValue)
    {
      remove(inKey);
      emplace(inKey, inValue);
    }

		void remove(const key_type& inKey)
		{
			boost::remove_if(mContainer, [&] (const value_type& x) {
        return x.first == inKey;
      });
		}

  private:
    container_type mContainer;
  };

}

#endif // defined __jerome_type_ordered_multimap_hpp__
