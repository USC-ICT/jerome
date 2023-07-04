//
//  optional.hpp
//
//  Created by Anton Leuski on 9/30/15.
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

#ifndef __jerome_type_optional_hpp__
#define __jerome_type_optional_hpp__

// The reason for this file is that optional will be introduced in C++14.
// We are using boost implementation meanwhile

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <boost/optional.hpp>
#pragma clang diagnostic pop

namespace jerome {
  template <class T> using optional	= boost::optional<T>;
}

#endif // defined __jerome_type_optional_hpp__
