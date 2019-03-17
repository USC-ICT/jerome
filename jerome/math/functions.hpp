//
//  functions.hpp
//
//  Created by Anton Leuski on 4/14/14.
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

#ifndef __jerome_math_functions_hpp__
#define __jerome_math_functions_hpp__

namespace jerome {
  namespace math {

    struct identity
    {
      template <typename U>
      struct result;

      template <typename A1>
      struct result<identity(A1)>
      {
        typedef A1 type;
      };

      template <typename U>
      constexpr auto operator () (U&& v) const noexcept
       ->decltype(std::forward<U>(v))
      {
        return std::forward<U>(v);
      }
    };

//	template <typename T>
//	struct identity {
//		typedef T result_type;
//		constexpr auto operator()(T&& v) const noexcept
//		-> decltype(std::forward<T>(v))
//		{
//			return std::forward<T>(v);
//		}
//	};

  }
}

#endif // defined __jerome_math_functions_hpp__
