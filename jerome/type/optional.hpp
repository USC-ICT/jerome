//
//  optional.hpp
//
//  Created by Anton Leuski on 9/30/15.
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

#ifndef __jerome_type_optional_hpp__
#define __jerome_type_optional_hpp__

// The reason for this file is that optional will be introduced in C++14. We are using
// boost implementation meanwhile

#include <boost/optional.hpp>

namespace jerome {
  template <class T> using optional	= boost::optional<T>;
}

#endif // defined __jerome_type_optional_hpp__
