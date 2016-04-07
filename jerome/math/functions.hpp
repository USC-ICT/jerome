//
//  functions.hpp
//
//  Created by Anton Leuski on 4/14/14.
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
