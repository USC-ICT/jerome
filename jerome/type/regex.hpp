//
//  regex.hpp
//  jerome
//
//  Created by Anton Leuski on 11/13/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
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
