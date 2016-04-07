//
//  conversion.hpp
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

#ifndef __jerome_scripting_javascript_conversion_hpp__
#define __jerome_scripting_javascript_conversion_hpp__

#include <iostream>
#include <utility>
#include <array>

#include <boost/optional.hpp>

#include <JavaScriptCore/JavaScriptCore.h>
#include <jerome/scripting/javascript/exception.hpp>
#include <jerome/scripting/javascript/types.hpp>
#include <jerome/scripting/javascript/string.hpp>
#include <jerome/scripting/javascript/context.hpp>

namespace jerome { namespace javascript { namespace detail {
	
#pragma mark JSValueRef
	
	template <>
	struct from_valueRef<JSValueRef> {
		static JSValueRef convert(const Context& ctx, JSValueRef valueRef) { return valueRef; }
	};

	template <>
	struct to_valueRef<JSValueRef> {
		static JSValueRef convert(const Context& ctx, JSValueRef valueRef) { return valueRef; }
	};

#pragma mark bool
	
	template <>
	struct from_valueRef<bool> {
		static bool convert(const Context& ctx, JSValueRef valueRef)
		{ return JSValueToBoolean(ctx.contextRef(), valueRef); }
	};
	
	template <>
	struct to_valueRef<bool>  {
		static JSValueRef convert(Context& ctx, bool value)
		{ return JSValueMakeBoolean(ctx.contextRef(), value); }
	};
	
#pragma mark number (arithmetic)
	
	template <typename T>
	struct from_valueRef<T, typename std::enable_if<std::is_arithmetic<T>::value >::type> {
		static T convert(Context& ctx, JSValueRef valueRef) {
			return ctx.callJSCFunction("converting to number", JSValueToNumber, valueRef);
		}
	};
	
	template <typename T>
	struct to_valueRef<T, typename std::enable_if<std::is_arithmetic<T>::value >::type>  {
		static JSValueRef convert(Context& ctx, T value) { return JSValueMakeNumber(ctx.contextRef(), value); }
	};
	
#pragma mark String
	
	template <>
	struct from_valueRef<String> {
		static String convert(const Context& ctx, JSValueRef valueRef) { return JSString(ctx, valueRef); }
	};

	template <>
	struct to_valueRef<String>  {
		static JSValueRef convert(Context& ctx, const String& value) {
			return JSValueMakeString(ctx.contextRef(), JSString(value));
		}
	};
	
#pragma mark const char*
	
	struct CastableString {
		String mString;
		bool hasString;
		CastableString() : hasString(false) {}
		CastableString(const CastableString&) = delete;
		CastableString(CastableString&& x) : mString(std::forward<String>(x.mString)), hasString(true) {}
		CastableString(String&& s) : mString(std::forward<String>(s)), hasString(true) {}
		operator const char*() const { return hasString ? mString.c_str() : nullptr; }
	};
	
	// the result will be valid as long as the temporary CastableString is alive.
	// note that conversion works only as part of another expression!
	// if you try to copy the value to a variable you run into trouble with const char
	//
	// also note the difference from conversion to String. This one will return nullptr for
	// Null and Undefined. The other will always return a string representation.
	template <>
	struct from_valueRef<const char*> {
		static CastableString convert(const Context& ctx, JSValueRef valueRef) {
			if (!valueRef
				|| JSValueIsUndefined(ctx.contextRef(), valueRef)
				|| JSValueIsNull(ctx.contextRef(), valueRef))
				return CastableString();
			return (String)JSString(ctx, valueRef);
		}
	};
	
	template <>
	struct to_valueRef<const char*>  {
		static JSValueRef convert(Context& ctx, const char* value) {
			if (!value) return JSValueMakeUndefined(ctx.contextRef());
			return JSValueMakeString(ctx.contextRef(), JSString(value));
		}
	};
	
	template< std::size_t N >
	struct to_valueRef<char[N]>  {
		static JSValueRef convert(Context& ctx, const char* value) {
			return to_valueRef<const char*>::convert(ctx, value);
		}
	};
	
#pragma mark collection (vector, set, list, etc.)
	
	// vector and more
	template <typename T>
	struct to_valueRef<T, typename std::enable_if<
	is_container<T>::value && !is_associative_container<T>::value
	>::type> {
		static JSValueRef convert(Context& ctx, const T& value) {
			std::vector<JSValueRef>		jsArguments;
			for(const auto& x : value)
				jsArguments.push_back(to_valueRef<typename T::value_type>::convert(ctx, x));
			return ctx.callJSCFunction("making an array", JSObjectMakeArray,
									   jsArguments.size(), jsArguments.data());
		}
	};

	template <typename ...Args>
	struct to_valueRef<std::tuple<Args...>> {
		static JSValueRef convert(Context& ctx, const std::tuple<Args...>& value) {
			std::array<JSValueRef, sizeof...(Args)>	args = to_array(ctx, value, index_sequence_for<Args...>{});
			return ctx.callJSCFunction("making an array", JSObjectMakeArray,
									   args.size(), args.data());
		}
		static JSValueRef convert(Context& ctx, Args ...as) {
			std::array<JSValueRef, sizeof...(Args)>	args = to_valueRefArray(ctx, std::forward<Args>(as)...);
			return ctx.callJSCFunction("making an array", JSObjectMakeArray,
									   args.size(), args.data());
		}
	private:
		template <std::size_t ...Indices>
		static std::array<JSValueRef, sizeof...(Args)>
		to_array(Context& ctx, const std::tuple<Args...>& value, index_sequence<Indices...>) {
			return { to_valueRef<Args>::convert(ctx, std::get<Indices>(value))... };
		}

	};

#pragma mark unordered_map
	
	template <typename T>
	struct from_valueRef<std::unordered_map<String, T>> {
		static std::unordered_map<String, T> convert(const Context& ctx, JSValueRef valueRef) {
			std::unordered_map<String, T>	result;
			JSPropertyNameArrayRef	array	= JSObjectCopyPropertyNames(ctx.contextRef(), (JSObjectRef)valueRef);
			for(size_t i = 0, n = JSPropertyNameArrayGetCount(array); i < n; ++i) {
				JSString	stringRef = JSPropertyNameArrayGetNameAtIndex(array, i);
				result[stringRef] = from_valueRef<T>::convert(ctx, ctx.callJSCFunction("converting to map",
																					   JSObjectGetProperty,
																					   (JSObjectRef)valueRef,
																					   stringRef));
			}
			JSPropertyNameArrayRelease(array);
			return result;
		}
	};
	
	template <typename T>
	struct to_valueRef<T, typename std::enable_if<
	is_container<T>::value && is_associative_container<T>::value
	>::type> {
		static JSValueRef convert(Context& ctx, const T& value) {
			JSObjectRef	result	= JSObjectMake(ctx.contextRef(), NULL, NULL);
			for(auto i : value) {
				ctx.callJSCFunction("making an object", JSObjectSetProperty, result, (JSStringRef)JSString(i.first),
									to_valueRef<typename T::mapped_type>::convert(ctx, i.second),
									kJSPropertyAttributeNone);
			}
			return result;
		}
	};
	
#pragma mark nullptr = undefined

	template <>
	struct to_valueRef<std::nullptr_t>  {
		static JSValueRef convert(Context& ctx, std::nullptr_t value) {
			return JSValueMakeUndefined(ctx.contextRef());
		}
	};

	template <>
	struct to_valueRef<void>  {
		template <typename T>
		static JSValueRef convert(Context& ctx, T) {
			return JSValueMakeUndefined(ctx.contextRef());
		}
	};

#pragma mark Null = null

	template <>
	struct to_valueRef<Null_t>  {
		static JSValueRef convert(Context& ctx, const Null_t& value) {
			return JSValueMakeNull(ctx.contextRef());
		}
	};
	
#pragma mark Value
	
	template <>
	struct from_valueRef<Value>  {
		static Value convert(const Context& ctx, JSValueRef valueRef)
		{ return valueRef ? Value(ctx, valueRef) : ctx.undefinedValue(); }
	};
	
	
	template <>
	struct to_valueRef<Value>  {
		static JSValueRef convert(Context& ctx, const Value& value) { return value.valueRef(); }
	};
	
	
	template <typename P>
	struct to_valueRef<PrototypeProperty<P>>  {
		static JSValueRef convert(Context& ctx, const PrototypeProperty<P>& value) { return value.valueRef(); }
	};
	template <typename P>
	struct to_valueRef<NamedProperty<P>>  {
		static JSValueRef convert(Context& ctx, const NamedProperty<P>& value) { return value.valueRef(); }
	};
	template <typename P>
	struct to_valueRef<IndexedProperty<P>>  {
		static JSValueRef convert(Context& ctx, const IndexedProperty<P>& value) { return value.valueRef(); }
	};
	
#pragma mark C++ object with ClassTraits
	
	AL_JS_CLASS_DECLARE_MEMBER_TESTER(className)
	
	// convert c++ objects
	template <typename T>
	struct from_valueRef<T, typename std::enable_if<
	__has_className<ClassTraits<T>>::value &&
	!is_std_function<T>::value&&
	!std::is_pointer<T>::value>::type
	> {
		static T convert(const Context& ctx, JSValueRef valueRef) {
			if (!JSValueIsObjectOfClass(ctx.contextRef(), valueRef, ClassTraits<T>::instance())) {
				std::cerr << "expected an object of class " << ClassTraits<T>::instance().className() << std::endl;
				assert(false);
			}
			return ClassTraits<T>::instance().representedObjectForRef((JSObjectRef)valueRef);
		}
	};
	
	template <typename T>
	struct to_valueRef<T, typename std::enable_if<
	__has_className<ClassTraits<T>>::value &&
	!is_std_function<T>::value &&
	!std::is_pointer<T>::value>::type
	> {
		static JSValueRef convert(Context& ctx, const T& value) {
			return to_valueRef<Value>::convert(ctx, ctx.newObjectOfNativeClass(value));
		}
	};
	
#pragma mark C++ pointer with ClassTraits
	
	// convert pointers
	template <typename T>
	struct from_valueRef<T, typename std::enable_if<
	__has_className<ClassTraits<T>>::value &&
	std::is_pointer<T>::value>::type
	> {
		static T convert(const Context& ctx, JSValueRef valueRef) {
			if (JSValueIsNull(ctx.contextRef(), valueRef)) return nullptr;
			if (JSValueIsUndefined(ctx.contextRef(), valueRef)) return nullptr;
			if (!JSValueIsObjectOfClass(ctx.contextRef(), valueRef, ClassTraits<T>::instance())) {
				std::cerr << "expected an object of class " << ClassTraits<T>::instance().className() << std::endl;
				assert(false);
			}
			return ClassTraits<T>::instance().representedObjectForRef((JSObjectRef)valueRef);
		}
	};
	
	template <typename T>
	struct to_valueRef<T, typename std::enable_if<
	__has_className<ClassTraits<T>>::value &&
	std::is_pointer<T>::value>::type
	> {
		static JSValueRef convert(Context& ctx, const T& value) {
			if (!value) return JSValueMakeUndefined(ctx.contextRef());
			return to_valueRef<Value>::convert(ctx, ctx.newObjectOfNativeClass(value));
		}
	};
	
#pragma mark C++ std::function / callable
	
	// convert functions & other callable objects
	template <typename T>
	struct __wrap_value_as_std_function;
	
	template <typename T>
	struct from_valueRef<T, typename std::enable_if<
	is_callable<T>::value &&
	std::is_convertible< std::function<typename function_signature<T>::type>, T>::value
	>::type> {
		static T convert(const Context& ctx, JSValueRef valueRef) {
			typedef typename function_signature<T>::type signature_type;
			typedef std::function<signature_type> function_type;
			
			if (JSValueIsObjectOfClass(ctx.contextRef(), valueRef, ClassTraits<function_type>::instance())) {
				return (T)ClassTraits<T>::instance().representedObjectForRef((JSObjectRef)valueRef);
			}
			
			return __wrap_value_as_std_function<signature_type>::wrap(ctx, (JSObjectRef)valueRef);
		}
	};
	
	template <typename T>
	struct to_valueRef<T, typename std::enable_if<is_callable<T>::value>::type> {
		static JSValueRef convert(Context& ctx, T value) {
			typedef typename std::function<typename function_signature<T>::type> fp_type;
			return to_valueRef<Value>::convert(ctx, ctx.newObjectOfNativeClass((fp_type)value));
		}
	};

#pragma mark optional

	template <typename T>
	struct from_valueRef<boost::optional<T>> {
		static boost::optional<T> convert(const Context& ctx, JSValueRef valueRef) {
			if (JSValueIsNull(ctx.contextRef(), valueRef)) return boost::optional<T>();
			if (JSValueIsUndefined(ctx.contextRef(), valueRef)) return boost::optional<T>();
			return boost::optional<T>(from_valueRef<T>::convert(ctx, valueRef));
		}
	};

	template <typename T>
	struct to_valueRef<boost::optional<T>> {
		static JSValueRef convert(Context& ctx, const boost::optional<T>& value) {
			return (value) ? to_valueRef<T>::convert(ctx, *value) : JSValueMakeUndefined(ctx.contextRef());
		}
	};
	
#pragma mark argument pack
	
	template<typename ...Args>
	std::array<JSValueRef, sizeof...(Args)> to_valueRefArray(Context& ctx, Args ...args)
	{
		return { to_valueRef<Args>::convert(ctx, args)... };
	}

#pragma mark - callbacks

	struct ObjectAsFunctionCallback {
		template<typename ...As>
		inline static Value call(Context& ctx, JSObjectRef function, JSObjectRef thisObject, As ...as)
		{
			std::array<JSValueRef, sizeof...(As)>	args = to_valueRefArray(ctx, std::forward<As>(as)...);
			return ctx.callJSCFunctionReturnValue("calling object as function", JSObjectCallAsFunction,
												  function, thisObject, args.size(), args.data());
		}
	};
	
	struct ObjectAsConstructorCallback {
		template<typename ...As>
		inline static Value call(Context& ctx, JSObjectRef function, As ...as)
		{
			std::array<JSValueRef, sizeof...(As)>	args = to_valueRefArray(ctx, std::forward<As>(as)...);
			return ctx.callJSCFunctionReturnValue("calling object as constructor", JSObjectCallAsConstructor,
												  function, args.size(), args.data());
		}
	};
	
	template<typename R>
	struct __JSCCallback {
		template<typename FunctionType, typename ...Arguments>
		inline static JSValueRef call (FunctionType F, Context& ctx, Arguments ...args)
		{
			return to_valueRef<R>::convert(ctx, F (std::forward<Arguments>(args)...));
		}
	};

	template<>
	struct __JSCCallback<void> {
		template<typename FunctionType, typename ...Arguments>
		inline static JSValueRef call (FunctionType F, Context& ctx, Arguments ...args)
		{
			F (std::forward<Arguments>(args)...);
			return to_valueRef<std::nullptr_t>::convert(ctx, nullptr);
		}
	};

	template <typename> struct JSCCallback;
	
	template<typename R, typename ...CallbackArguments>
	struct JSCCallback<R(CallbackArguments...)> {
		template<typename FunctionType, typename ...AdditionalArguments>
		static JSValueRef call(FunctionType&& theFunction, JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
							   size_t argumentCount, const JSValueRef arguments[], JSValueRef* exception,
							   AdditionalArguments ...additionalArguments)
		{
			FunctionCallContext		myContext(ctx, function, thisObject, argumentCount, arguments, exception);
			try {
				std::array<JSValueRef, sizeof...(CallbackArguments)>	valueRefArguments;
				for(std::size_t i = 0, n = std::min(valueRefArguments.size(), argumentCount); i < n; ++i)
					valueRefArguments[i] = arguments[i];
				for(std::size_t i = argumentCount, n = valueRefArguments.size(); i < n; ++i)
					valueRefArguments[i] = to_valueRef<std::nullptr_t>::convert(myContext, nullptr);
				
				return call1(std::forward<FunctionType>(theFunction),
							 myContext,
							 valueRefArguments,
							 index_sequence_for<CallbackArguments...>{},
							 std::forward<AdditionalArguments>(additionalArguments)...);
				
			} catch (const std::exception& ex) {
				myContext.setException(ex);
				return JSValueMakeUndefined(ctx);
			}
		}
	private:
		template<typename FunctionType, typename ...AdditionalArguments, std::size_t ...Indices>
		inline static JSValueRef call1(FunctionType&& F,
									   Context& ctx,
									   std::array<JSValueRef, sizeof...(CallbackArguments)>& callbackArguments,
									   index_sequence<Indices...>,
									   AdditionalArguments ...additionalArguments)
		{
			return __JSCCallback<R>::call(std::forward<FunctionType>(F),
										  ctx,
										  std::forward<AdditionalArguments>(additionalArguments)...,
										  from_valueRef<typename std::decay<CallbackArguments>::type>::convert(ctx,
																		callbackArguments[Indices])...);
		}
	};

	template <typename R, typename ...Args>
	struct __wrap_value_as_std_function<R(Args...)> {
		static std::function<R(Args...)> wrap(const Context& ctx, JSValueRef valueRef) {
			// capture the Value by value
			Value v = from_valueRef<Value>::convert(ctx, valueRef);
			return [=](Args ...args) mutable -> R {
				return ObjectAsFunctionCallback::call(v.context(), (JSObjectRef)v.valueRef(),
													  nullptr, std::forward<Args>(args)...);
			};
		}
	};
	
	template <typename ...Args>
	struct __wrap_value_as_std_function<void(Args...)> {
		static std::function<void(Args...)> wrap(const Context& ctx, JSValueRef valueRef) {
			// capture the Value by value
			Value v = from_valueRef<Value>::convert(ctx, valueRef);
			return [=](Args ...args) mutable {
				ObjectAsFunctionCallback::call(v.context(), (JSObjectRef)v.valueRef(),
											   nullptr, std::forward<Args>(args)...);
			};
		}
	};
	


}}}

#endif // defined __jerome_scripting_javascript_conversion_hpp__
