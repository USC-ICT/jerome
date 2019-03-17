//
//  class_imp.hpp
//
//  Created by Anton Leuski on 8/20/14.
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

#ifndef __jerome_javascript_class_imp_hpp__
#define __jerome_javascript_class_imp_hpp__

#include <jerome/scripting/javascript/class_traits.hpp>

namespace jerome { namespace javascript {

	template <typename R, typename ...As>
	struct ClassTraits<std::function<R(As...)>> : DefaultClassTraits<std::function<R(As...)>> {
		virtual JSValueRef callAsFunctionCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
												  size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception) override
		{
			return detail::JSCCallback<R(As...)>::call(this->representedObjectForRef(function),
														  ctx, function, thisObject, argumentCount, arguments, exception);
		}
	};
	
	namespace detail {
		
		
		template <typename TT, typename CC, typename R, typename ...Args, R(CC::*F)(Args...)>
		struct Callback <TT, R(CC::*)(Args...), F> {
			static R callback(Args ...args) {
				return (TT::privateInstance().*F)(std::forward<Args>(args)...);
			}
		};
		
		template <class C, class CR>
		std::shared_ptr<OpaqueJSClass>& ParameterizedClass<C,CR>::classDescriptionData() {
      // STATIC
			static std::shared_ptr<OpaqueJSClass>	sharedData;
			if (!sharedData) {
				JSClassDefinition	definition;
				
				definition.version				= 0;
				definition.className			= privateInstance().className();
				definition.parentClass			= privateInstance().parentClass();
				
				std::vector<JSStaticValue>		sv = privateInstance().staticValues();
				if (sv.size() == 0) {
					definition.staticValues		= nullptr;
				} else {
					sv.push_back(JSStaticValue({0,0,0,0}));
					definition.staticValues		= sv.data();
				}
				
				std::vector<JSStaticFunction>	sf = privateInstance().staticFunctions();
				if (sf.size() == 0) {
					definition.staticFunctions	= nullptr;
				} else {
					sf.push_back(JSStaticFunction({0,0,0}));
					definition.staticFunctions	= sf.data();
				}
				
// generating callbacks for the description structure that reference the instance() member functions
#define AL_JS_GENERATE_CALLBACK(x) \
definition.x = &Callback<traits_type, decltype(&traits_type::x##Callback), &traits_type::x##Callback>::callback
				AL_JS_GENERATE_CALLBACK(initialize);
				AL_JS_GENERATE_CALLBACK(finalize);
				AL_JS_GENERATE_CALLBACK(hasProperty);
				AL_JS_GENERATE_CALLBACK(getProperty);
				AL_JS_GENERATE_CALLBACK(setProperty);
				AL_JS_GENERATE_CALLBACK(deleteProperty);
				AL_JS_GENERATE_CALLBACK(getPropertyNames);
				AL_JS_GENERATE_CALLBACK(callAsFunction);
				AL_JS_GENERATE_CALLBACK(callAsConstructor);
				AL_JS_GENERATE_CALLBACK(hasInstance);
				AL_JS_GENERATE_CALLBACK(convertToType);
#undef AL_JS_GENERATE_CALLBACK
				
				sharedData	= std::shared_ptr<OpaqueJSClass>(JSClassCreate(&definition), Class_deleter());
			}
			return sharedData;
		}

			
		template <typename FN, FN F, JSPropertyAttributes A = kJSPropertyAttributeNone, typename = void> struct CppClassMemberCallback;
		
		template <typename FN, FN F, JSPropertyAttributes A>
		struct CppClassMemberCallback<FN, F, A, typename std::enable_if<std::is_member_function_pointer<FN>::value>::type>
		: public jerome::javascript::UserFunction<A> {
			static JSValueRef call(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
								   const JSValueRef arguments[], JSValueRef* exception)
			{
				typedef typename function_signature<FN>::type	signature;
				typedef typename member_class_type<FN>::type	object_type;
				
				if (!JSValueIsObjectOfClass(ctx, thisObject, ClassTraits<object_type>::instance())) {
					std::cerr << "this object is not of the mapped type" << std::endl;
					assert(false);
				}
				
				return JSCCallback<signature>::call(std::mem_fn(F),
													ctx, function, thisObject, argumentCount, arguments, exception,
													ClassTraits<object_type>::instance().representedObject(thisObject));
			}
		};
		
		template <typename FN, FN F, JSPropertyAttributes A>
		struct CppClassMemberCallback<FN, F, A, typename std::enable_if<std::is_member_object_pointer<FN>::value>::type>
		: public UserProperty<A> {
			static JSValueRef get(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exc)
			{
				typedef typename member_value_type<FN>::type	value_type;
				typedef typename member_class_type<FN>::type	object_type;
				
				if (!JSValueIsObjectOfClass(ctx, object, ClassTraits<object_type>::instance())) {
					std::cerr << "this object is not of the mapped type" << std::endl;
					assert(false);
				}
				detail::CallbackContext myContext(ctx, exc);
				try {
					return to_value<value_type>::convert(myContext, ClassTraits<object_type>::instance().representedObject(object).*F).returnValueRef();
				} catch (const Exception& e) {
					myContext.setException(e);
					return nullptr;
				}
			}
			
			static bool set(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exc)
			{
				typedef typename member_value_type<FN>::type	value_type;
				typedef typename member_class_type<FN>::type	object_type;
				
				if (!JSValueIsObjectOfClass(ctx, object, ClassTraits<object_type>::instance())) {
					std::cerr << "this object is not of the mapped type" << std::endl;
					assert(false);
				}
				
				detail::CallbackContext myContext(ctx, exc);
				try {
					ClassTraits<object_type>::instance().representedObject(object).*F =
												from_valueRef<value_type>::convert(myContext, value);
					return true;
				} catch (const Exception& e) {
					myContext.setException(e);
					return false;
				}
			}
			
		};
		
		template <typename FN, FN F, JSPropertyAttributes A>
		struct CppClassMemberCallback<FN, F, A, typename std::enable_if<is_static_function_pointer<FN>::value>::type>
		: public UserFunction<A> {
			static JSValueRef call(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
								   const JSValueRef arguments[], JSValueRef* exception)
			{
				typedef typename function_signature<FN>::type	signature;
				return JSCCallback<signature>::call(F,
													ctx, function, thisObject, argumentCount, arguments, exception);
			}
		};
		
		template <typename FN, FN F, JSPropertyAttributes A>
		struct CppClassMemberCallback<FN, F, A, typename std::enable_if<is_static_object_pointer<FN>::value>::type>
		: public UserProperty<A> {
			static JSValueRef get(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exc)
			{
				typedef typename std::remove_pointer<FN>::type	value_type;
				
				detail::CallbackContext myContext(ctx, exc);
				try {
					return to_value<value_type>::convert(myContext, *F).returnValueRef();
				} catch (const Exception& e) {
					myContext.setException(e);
					return nullptr;
				}
			}
			
			static bool set(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exc)
			{
				typedef typename std::remove_pointer<FN>::type	value_type;
				
				detail::CallbackContext myContext(ctx, exc);
				try {
					*F = from_valueRef<value_type>::convert(myContext, value);
					return true;
				} catch (const Exception& e) {
					myContext.setException(e);
					return false;
				}
			}
			
		};
		
		//		// the callback function can have any arguments that can be converted from JSValueRef
		//		// it can return any type that can be converted to JSValueRef (or void, in that case the call back will return 'undefined')
		//		// it can throw exceptions (std::exception subclass) that will be caught an converted into JSValueRef
		
		
	}

	inline void UserClass::initializeCallback(JSContextRef ctx, JSObjectRef object) {
		Context myContext(ctx);
		Value	myObject = detail::from_valueRef<Value>::convert(myContext, object);
		initialize(myObject);
	}
	
	inline void UserClass::finalizeCallback(JSObjectRef object) {
		ValueBase	myObject(object);
		finalize(myObject);
	}
	
	inline bool UserClass::adaptHasPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName) {
		Context myContext(ctx);
		String	pname		= detail::JSString(propertyName).string();
		Value	myObject	= detail::from_valueRef<Value>::convert(myContext, object);
		return hasProperty(myObject, pname);
	}
	
	inline JSValueRef UserClass::adaptGetPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exc) {
		detail::CallbackContext myContext(ctx, exc);
		try {
			String	pname		= detail::JSString(propertyName).string();
			Value	myObject	= detail::from_valueRef<Value>::convert(myContext, object);
			return getProperty(myObject, pname).returnValueRef();
		} catch (const Exception& e) {
			myContext.setException(e);
			return NULL;
		}
	}
	
	inline bool UserClass::adaptSetPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exc) {
		detail::CallbackContext myContext(ctx, exc);
		try {
			String	pname		= detail::JSString(propertyName).string();
			Value	myObject	= detail::from_valueRef<Value>::convert(myContext, object);
			Value	myValue		= detail::from_valueRef<Value>::convert(myContext, value);
			return setProperty(myObject, pname, myValue);
		} catch (const Exception& e) {
			myContext.setException(e);
			return false;
		}
	}
	
	inline bool UserClass::adaptDeletePropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exc) {
		detail::CallbackContext myContext(ctx, exc);
		try {
			String	pname		= detail::JSString(propertyName).string();
			Value	myObject	= detail::from_valueRef<Value>::convert(myContext, object);
			return deleteProperty(myObject, pname);
		} catch (const Exception& e) {
			myContext.setException(e);
			return false;
		}
	}
	
	inline void UserClass::adaptGetPropertyNamesCallback(JSContextRef ctx,
                                                       JSObjectRef object,
                                                       JSPropertyNameAccumulatorRef propertyNames)
  {
		Context myContext(ctx);
		Value	myObject	= detail::from_valueRef<Value>::convert(myContext, object);
		std::vector<std::string>	names	= getPropertyNames(myObject);
		for(auto s : names) {
			JSPropertyNameAccumulatorAddName(propertyNames, detail::JSString(s).jsStringRef());
		}
	}
	
	inline JSValueRef UserClass::callAsFunctionCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exc) {
		detail::CallbackContext myContext(ctx, exc);
		try {
			std::vector<Value>	myArguments	= myContext.valueArrayWithValueRefArray(argumentCount, arguments);
			Value	myFuncton	= detail::from_valueRef<Value>::convert(myContext, function);
			Value	myObject	= detail::from_valueRef<Value>::convert(myContext, thisObject);
			return callAsFunction(myFuncton, myObject, myArguments).returnValueRef();
		} catch (const Exception& e) {
			myContext.setException(e);
			return NULL;
		}
	}
	
	inline JSObjectRef UserClass::callAsConstructorCallback(JSContextRef ctx, JSObjectRef function, size_t argumentCount, const JSValueRef arguments[], JSValueRef* exc) {
		detail::CallbackContext myContext(ctx, exc);
		try {
			std::vector<Value>	myArguments	= myContext.valueArrayWithValueRefArray(argumentCount, arguments);
			Value	myFuncton	= detail::from_valueRef<Value>::convert(myContext, function);
			return (JSObjectRef)callAsConstructor(myFuncton, myArguments).returnValueRef();
		} catch (const Exception& e) {
			myContext.setException(e);
			return NULL;
		}
	}
	
	inline bool UserClass::hasInstanceCallback(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance, JSValueRef* exc) {
		detail::CallbackContext myContext(ctx, exc);
		try {
			Value	myConstructor	= detail::from_valueRef<Value>::convert(myContext, constructor);
			Value	myInstance		= detail::from_valueRef<Value>::convert(myContext, possibleInstance);
			return hasInstance(myConstructor, myInstance);
		} catch (const Exception& e) {
			myContext.setException(e);
			return false;
		}
	}
	
	inline JSValueRef UserClass::convertToTypeCallback(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exc) {
		detail::CallbackContext myContext(ctx, exc);
		try {
			Value	myObject	= detail::from_valueRef<Value>::convert(myContext, object);
			return convertToType(myObject, type).returnValueRef();
		} catch (const Exception& e) {
			myContext.setException(e);
			return NULL;
		}
	}
}}

#endif // __jerome_javascript_class_imp_hpp__
