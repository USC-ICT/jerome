//
//  algorithm.hpp
//  jerome
//
//  Created by Anton Leuski on 9/8/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_algorithm_h__
#define __jerome_algorithm_h__

#include <vector>
#include <random>

#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <jerome/types.hpp>


namespace jerome {

  template <class Result, class Range, class URBG = std::default_random_engine>
  std::pair<Result, Result> split(Range inRange, double inProportion, URBG&& g = URBG())
  {
    typedef typename std::remove_cv<
      typename std::remove_reference<
        Range>::type
      >::type::value_type  value_type;
    std::vector<value_type> randomized(std::begin(inRange), std::end(inRange));
    
    std::size_t maxCount  = randomized.size();
    std::size_t count   = (inProportion < 0 || inProportion >= maxCount)
    ? maxCount
    : (std::size_t)((inProportion < 1)
                    ? inProportion * maxCount
                    : inProportion);

    if (count == maxCount) {
      return std::pair<Result, Result>(randomized, randomized);
    }
    
    std::shuffle(boost::begin(randomized), boost::end(randomized), g);
    
    auto first = boost::make_iterator_range(randomized.begin(),
                                            randomized.begin() + count);
    
    auto second = boost::make_iterator_range(randomized.begin() +
                                             count, randomized.end());
    
    return std::pair<Result, Result>(Result(first.begin(), first.end()),
                                     Result(second.begin(), second.end()));
  }
  
  template <class Result, class Range, class Predicate>
  std::pair<Result, Result> split(Range inRange, Predicate&& p)
  {
    Result  first;
    Result  second;
    
    for(auto x : inRange) {
      if (p(x)) {
        first.push_back(x);
      } else {
        second.push_back(x);
      }
    }
    
    return std::pair<Result, Result>(first, second);
  }
  
  // for iterating over map keys.
  template <typename Iter>
  boost::transform_iterator<KeyGetter<Iter>, Iter,
  const typename Iter::value_type::first_type&> KeyIterator(Iter iterator)
  {
    return boost::transform_iterator<
    KeyGetter<Iter>,
    Iter,
    const typename Iter::value_type::first_type&>
    (iterator, KeyGetter<Iter>());
  }
  
  template <typename T>
  std::vector<typename T::key_type> keys(const T& map) {
    return std::vector<typename T::key_type>(KeyIterator(map.begin()),
                       KeyIterator(map.end()));
  }
  
  template <typename M>
  optional<typename M::mapped_type> map_value_at(const M& map,
                                                 const typename M::key_type& key)
  {
    auto x = map.find(key);
    if (x == map.end()) {
      return optional<typename M::mapped_type>();
    } else {
      return x->second;
    }
  }
  
}

#endif // __jerome_algorithm_h__
