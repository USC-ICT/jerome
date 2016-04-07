//
//  regex.hpp
//  jerome
//
//  Created by Anton Leuski on 11/13/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_type_regex_hpp__
#define __jerome_type_regex_hpp__

#ifdef __ANDROID__
#define USE_BOOST_REGEX 1
#else
#define USE_BOOST_REGEX 0
#endif

#if USE_BOOST_REGEX
#include <boost/regex.hpp>
namespace rgx = boost;
#else
#include <regex>
namespace rgx = std;
#endif

#endif // defined __jerome_type_regex_hpp__
