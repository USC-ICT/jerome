//
//  meta.hpp
//
//  Created by Anton Leuski on 9/25/18.
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

#ifndef meta_hpp
#define meta_hpp

#include <type_traits>

// stuff that does not exist in C++11

namespace jerome {
  template <bool B, class T = void>
  using enable_if_t = typename ::std::enable_if<B,T>::type;

  template< class T >
  using decay_t = typename ::std::decay<T>::type;

  template< class Base, class Derived >
  constexpr bool is_base_of_v = ::std::is_base_of<Base, Derived>::value;
}

#endif // defined meta_hpp
