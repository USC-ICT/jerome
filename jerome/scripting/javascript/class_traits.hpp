//
//  class_traits.hpp
//
//  Created by Anton Leuski on 8/21/14.
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

#ifndef __jerome_scripting_javascript_class_traits_hpp__
#define __jerome_scripting_javascript_class_traits_hpp__

#include <jerome/scripting/javascript/user_class.hpp>

namespace jerome { namespace javascript {

	// export a subclass of UserFunction as static callback for property N
#define AL_JS_EXPORT_USER_FUNCTION_AS(UF, N) \
mStaticFunctions.push_back(JSStaticFunction({N, UF::call, UF::attributes}))
	
	// export an object member function M of type T as static callback for property N
	// works in case you have multiple M defined in the class
#define AL_JS_EXPORT_MEMBER_METHOD_OF_TYPE_AS(M, T, N) \
mStaticFunctions.push_back(JSStaticFunction({N, \
jerome::javascript::detail::CppClassMemberCallback<T, &object_type::M>::call, \
jerome::javascript::detail::CppClassMemberCallback<T, &object_type::M>::attributes}))
	
	// export an object member function M as static callback for property N
#define AL_JS_EXPORT_MEMBER_METHOD_AS(M, N) \
AL_JS_EXPORT_MEMBER_METHOD_OF_TYPE_AS( M, decltype(&object_type::M), N)
	
	// export an object member function M as static callback for property M
#define AL_JS_EXPORT_MEMBER_METHOD(M) \
AL_JS_EXPORT_MEMBER_METHOD_AS(M, #M)
	
	
	// export a subclass of UserProperty as static callback for property N
#define AL_JS_EXPORT_USER_PROPERTY_AS(UP, N) \
mStaticValues.push_back(JSStaticValue({N, UP::get, UP::set, UP::attributes}))
	
	// export an object member  M as static callback for property N
#define AL_JS_EXPORT_MEMBER_OBJECT_AS(M, N) \
mStaticValues.push_back(JSStaticValue({N, \
jerome::javascript::detail::CppClassMemberCallback<decltype(&object_type::M), &object_type::M>::get, \
jerome::javascript::detail::CppClassMemberCallback<decltype(&object_type::M), &object_type::M>::set, \
jerome::javascript::detail::CppClassMemberCallback<decltype(&object_type::M), &object_type::M>::attributes}))
	
	// export an object member  M
#define AL_JS_EXPORT_MEMBER_OBJECT(M) \
AL_JS_EXPORT_MEMBER_OBJECT_AS(M, #M)

	template <class C>
	struct DefaultClassTraits : detail::ClassTemplate<C> {
		typedef detail::ClassTemplate<C>	parent_type;
		typedef typename parent_type::object_type		object_type;
		typedef typename parent_type::reference_type	reference_type;
		DefaultClassTraits(const String& inName = typeid(object_type).name()) : parent_type(inName) {}
	};
	
	/*
	 
	A typical class traits subclass is implemented like this:

	namespace jerome { namespace javascript {
		struct ClassTraits<Foo> : public DefaultClassTraits<Foo> {
			ClassTraits()
	 		: DefaultClassTraits<Foo>("Foo")
			{
				AL_JS_EXPORT_MEMBER_METHOD(bar);
				 AL_JS_EXPORT_MEMBER_OBJECT(dom);
			}
		};
	}}
	 */
	
}}

#endif // defined __jerome_scripting_javascript_class_traits_hpp__
