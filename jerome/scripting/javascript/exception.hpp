//
//  exception.hpp
//
//  Created by Anton Leuski on 8/5/14.
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

#ifndef __jerome_scripting_javascript_exception_hpp__
#define __jerome_scripting_javascript_exception_hpp__
#include <stdexcept>
#include <JavaScriptCore/JavaScript.h>

namespace jerome { namespace javascript {

	struct Exception : public std::logic_error {
		using std::logic_error::logic_error;
	};

#define _EXCEPTION(name) \
struct name : public std::logic_error { \
name() : std::logic_error(#name) {} };
	_EXCEPTION(get_property_undefined)
	_EXCEPTION(set_property_undefined)
	_EXCEPTION(delete_property_undefined)
	_EXCEPTION(object_is_not_function)
	_EXCEPTION(object_is_not_constructor)
	_EXCEPTION(cannot_convert_object)
	_EXCEPTION(illegal_arguments)
	_EXCEPTION(immutable_property)
	_EXCEPTION(not_in_call_context)
#undef _EXCEPTION

}}


#endif // defined __jerome_scripting_javascript_exception_hpp__
