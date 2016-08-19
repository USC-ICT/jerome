//
//  exception.hpp
//
//  Created by Anton Leuski on 8/5/14.
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

#ifndef __jerome_scripting_javascript_exception_hpp__
#define __jerome_scripting_javascript_exception_hpp__
#include <stdexcept>
#include <JavaScriptCore/JavaScript.h>

namespace jerome { namespace javascript {

	struct Exception : public std::logic_error {
		using std::logic_error::logic_error;
	};
	
	struct get_property_undefined		: Exception{
		 get_property_undefined() : Exception("get_property_undefined") {}};
	struct set_property_undefined		: Exception{
		 set_property_undefined() : Exception("set_property_undefined") {}};
	struct delete_property_undefined	: Exception{
		 delete_property_undefined() : Exception("delete_property_undefined") {}};
	struct object_is_not_function		: Exception{
		 object_is_not_function() : Exception("object_is_not_function") {}};
	struct object_is_not_constructor	: Exception{
		 object_is_not_constructor() : Exception("object_is_not_constructor") {}};
	struct cannot_convert_object		: Exception{
		 cannot_convert_object() : Exception("cannot_convert_object") {}};
	struct illegal_arguments			: Exception{
		 illegal_arguments() : Exception("illegal_arguments") {}};
	struct immutable_property			: Exception{
		 immutable_property() : Exception("immutable_property") {}};
	struct not_in_call_context			: Exception{
		 not_in_call_context() : Exception("not_in_call_context") {}};

}}


#endif // defined __jerome_scripting_javascript_exception_hpp__
