//
//  javascript.hpp
//
//  Created by Anton Leuski on 10/23/12.
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

#ifndef __jerome_scripting_javascript_hpp__
#define __jerome_scripting_javascript_hpp__

#include <string>
#include <vector>
#include <iostream>
#include <JavaScriptCore/JavaScript.h>

#include <jerome/scripting/javascript/exception.hpp>
#include <jerome/scripting/javascript/string.hpp>
#include <jerome/scripting/javascript/types.hpp>
#include <jerome/scripting/javascript/value.hpp>
#include <jerome/scripting/javascript/context.hpp>
#include <jerome/scripting/javascript/class.hpp>

#include <jerome/scripting/javascript/conversion.hpp>

#include <jerome/scripting/javascript/impl/string_imp.hpp>
#include <jerome/scripting/javascript/impl/value_imp.hpp>
#include <jerome/scripting/javascript/impl/class_imp.hpp>

#include <jerome/scripting/javascript/impl/context_imp.hpp>

namespace jerome { namespace javascript {
	
}}

#endif // defined __jerome_scripting_javascript_hpp__
