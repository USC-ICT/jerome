//
//  random.hpp
//
//  Created by Anton Leuski on 7/22/14.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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
