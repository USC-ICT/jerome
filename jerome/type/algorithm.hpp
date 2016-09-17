//
//  algorithm.hpp
//  jerome
//
//  Created by Anton Leuski on 9/8/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
  
}

#endif // __jerome_algorithm_h__
