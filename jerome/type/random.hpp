//
//  random.hpp
//
//  Created by Anton Leuski on 7/22/14.
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

#ifndef __jerome_type_random_hpp__
#define __jerome_type_random_hpp__

#include <random>

namespace jerome {
	template <class T, class D, class E>
	struct rand_base {
    typedef T	value_type;
    typedef D	distribution_type;
    typedef E	random_engine_type;
    
    rand_base(value_type inMin, value_type inMax)
    : generator()
    , distribution(inMin, inMax)
    {
    }
    
    rand_base(value_type inMin, value_type inMax, value_type inSeed)
    : generator(inSeed)
    , distribution(inMin, inMax)
    {
    }
    
    inline value_type	operator() ()
    {
      return distribution(generator);
    }
    
  private:
    distribution_type distribution;
    random_engine_type generator;
  };
	
	template <typename, typename = void>
	struct random;
  
  template <typename T>
  struct random<T, typename std::enable_if<std::is_integral<T>::value>::type>
    : public rand_base<T, std::uniform_int_distribution<T>, std::default_random_engine > {
      typedef rand_base<T, std::uniform_int_distribution<T>, std::default_random_engine > parent_type;
      using parent_type::parent_type;
  };

  template <typename T>
  struct random<T, typename std::enable_if<std::is_floating_point<T>::value>::type>
    : public rand_base<T, std::uniform_real_distribution<T>, std::default_random_engine > {
      typedef rand_base<T, std::uniform_real_distribution<T>, std::default_random_engine > parent_type;
      using parent_type::parent_type;
  };

}

#endif // defined __jerome_type_random_hpp__
