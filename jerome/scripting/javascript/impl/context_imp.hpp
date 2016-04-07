//
//  context_imp.hpp
//
//  Created by Anton Leuski on 8/11/14.
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

#ifndef __jerome_scripting_javascript_impl_context_imp_hpp__
#define __jerome_scripting_javascript_impl_context_imp_hpp__

#include <jerome/scripting/javascript/conversion.hpp>
#include <boost/thread/tss.hpp>

namespace jerome { namespace javascript {
	
	template <class C>
	Value Context::newObjectOfNativeClass(const C& inCObject)
	{
		return this->newObject(ClassTraits<C>::instance(), ClassTraits<C>::instance().newVoidPointerWithObject(inCObject));
	}
	
	template <class C>
	Value Context::newObjectOfNativeClass()
	{
		return this->newObject(ClassTraits<C>::instance(), ClassTraits<C>::instance().newVoidPointer());
	}
	
	template <typename R, typename ...Args, typename ...Args1>
	Value Context::callJSCFunctionReturnValue(const String& inExceptionContext,
											  R(*inFunction)(Args...),
											  Args1 ...args) const
	{
		return detail::from_valueRef<Value>::convert(*this,
													 callJSCFunction(inExceptionContext,
																	 inFunction,
																	 std::forward<Args1>(args)...));
	}

	template <typename T>
	Value Context::newValue(T&& value) {
		return detail::from_valueRef<Value>::convert(*this,
					detail::to_valueRef<typename std::remove_reference<T>::type>::convert(*this, value));
	}

	inline detail::NamedProperty<Context>		Context::operator[] (const String& propertyName)
	{
		return detail::NamedProperty<Context>(*this, propertyName);
	}

	inline Value								Context::operator[] (const String& propertyName) const
	{
		return detail::NamedProperty<const Context>(*this, propertyName);
	}
	
	inline detail::NamedProperty<Context>		Context::operator[] (const char* propertyName)
	{
		return detail::NamedProperty<Context>(*this, propertyName);
	}

	inline Value								Context::operator[] (const char* propertyName) const
	{
		return detail::NamedProperty<const Context>(*this, propertyName);
	}
	
	inline detail::IndexedProperty<Context>		Context::operator[] (int propertyIndex)
	{
		return detail::IndexedProperty<Context>(*this, propertyIndex);
	}

	inline Value								Context::operator[] (int propertyIndex) const
	{
		return detail::IndexedProperty<const Context>(*this, propertyIndex);
	}

	namespace detail {
		
		const char* const __exceptionHandler__ = "__exceptionHandler__";
		
		template <typename Peer>
		Context::exception_handler_type	ExceptionHandlerProperty<Peer>::get() const
		{
			Value handler = this->mSource.globalObject()[__exceptionHandler__];
			return handler.isUndefined()
				? (Context::exception_handler_type)Context::defaultExceptionHandler
				: (Context::exception_handler_type)handler;
		}
		
		template <typename Peer>
		void		ExceptionHandlerProperty<Peer>::set(const Context::exception_handler_type& inValue)
		{
			this->mSource.globalObject()[__exceptionHandler__] = inValue;
		}
	}
	
	inline Context::exception_handler_type 	Context::exceptionHandler() const
	{
		const detail::ExceptionHandlerProperty<const Context> result(*this);
		return result;
	}
	
	inline detail::ExceptionHandlerProperty<Context> Context::exceptionHandler()
	{
		return detail::ExceptionHandlerProperty<Context>(*this);
	}

	
#define VALUE_WITH_REF(x)	detail::from_valueRef<Value>::convert(*this, x)
	
	inline Value Context::evaluateScript(const String& script, Value& thisObject, const String& sourceURL, int startingLineNumber)
	{
		//			std::cerr << s.substr(0,std::min(64ul, s.length())) << "\n";
		return callJSCFunctionReturnValue("evaluating script", JSEvaluateScript,
										  detail::JSString(script),
										  (JSObjectRef)detail::to_valueRef<Value>::convert(*this, thisObject),
										  detail::JSString(sourceURL),
										  startingLineNumber);
	}
	
	inline Value Context::evaluateScript(const String &script, const String& sourceURL, int startingLineNumber)
	{
		//			std::cerr << s.substr(0,std::min(64ul, s.length())) << "\n";
		return callJSCFunctionReturnValue("evaluating script", JSEvaluateScript,
										  detail::JSString(script),
										  nullptr,
										  detail::JSString(sourceURL),
										  startingLineNumber);
	}
	
	inline bool Context::checkScriptSyntax(const String& script, const String& sourceURL, int startingLineNumber) {
		return callJSCFunction("checking script", JSCheckScriptSyntax,
							   detail::JSString(script),
							   detail::JSString(sourceURL),
							   startingLineNumber);
	}
	
	inline void Context::garbageCollect() {
		JSGarbageCollect(mData.get());
	}
	
	inline Value Context::globalObject() const {
		return VALUE_WITH_REF(JSContextGetGlobalObject(contextRef()));
	}
	
	inline ContextGroup Context::group() {
		return JSContextGetGroup(this->contextRef());
	}
	
	inline Value Context::undefinedValue() const
	{
		return VALUE_WITH_REF(JSValueMakeUndefined(contextRef()));
	}
	
	inline Value Context::nullValue() const
	{
		return VALUE_WITH_REF(JSValueMakeNull(contextRef()));
	}
	
	inline Value Context::valueFromJSONString(const String& value)
	{
		return VALUE_WITH_REF(JSValueMakeFromJSONString(contextRef(), detail::JSString(value)));
	}
	
	namespace detail {
		template <typename ...As>
		Value newDate(Context& ctx, As ...as)
		{
			std::array<JSValueRef, sizeof...(As)>	args = to_valueRefArray(ctx, std::forward<As>(as)...);
			return ctx.callJSCFunctionReturnValue("creating new date", JSObjectMakeDate,
												  args.size(), args.data());
		}

		template <typename ...As>
		Value newRegExp(Context& ctx, As ...as)
		{
			std::array<JSValueRef, sizeof...(As)>	args = to_valueRefArray(ctx, std::forward<As>(as)...);
			return ctx.callJSCFunctionReturnValue("creating new regex", JSObjectMakeRegExp,
												  args.size(), args.data());
		}
	}
	

	inline Value Context::newDate()
	{
		return detail::newDate(*this);
	}

	inline Value Context::newDate(long milliseconds)
	{
		return detail::newDate(*this, milliseconds);
	}

	inline Value Context::newDate(const String& inDateString)
	{
		return detail::newDate(*this, inDateString);
	}
	
	inline Value Context::newDate(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds)
	{
		return detail::newDate(*this, year, month, day, hours, minutes, seconds, milliseconds);
	}

	
	inline Value Context::newRegExp(const String& pattern, const String& modifiers)
	{
		return detail::newRegExp(*this, pattern, modifiers);
	}
	
	inline Value Context::newFunction(const String& name, const std::vector<String>& arguments, const String& body, const String& sourceURL, int startingLineNumber)
	{
		detail::StringRefArray	jsArguments(arguments);
		return callJSCFunctionReturnValue("creating new function " + name, JSObjectMakeFunction,
										  detail::JSString(name),
										  jsArguments.size(), jsArguments.strings(),
										  detail::JSString(body),
										  detail::JSString(sourceURL),
										  startingLineNumber);
	}
	
	inline Value Context::newAnonymousFunction(const std::vector<String>& arguments, const String& body, const String& sourceURL, int startingLineNumber)
	{
		detail::StringRefArray	jsArguments(arguments);
		return callJSCFunctionReturnValue("creating new anonymous function", JSObjectMakeFunction,
										  nullptr, jsArguments.size(), jsArguments.strings(),
										  detail::JSString(body), detail::JSString(sourceURL), startingLineNumber);
	}
	
	inline Value Context::newObject(const jerome::javascript::Class &clazz, void *data) {
		return VALUE_WITH_REF(JSObjectMake(contextRef(), clazz, data));
	}
	
	inline Value Context::newObject() {
		return VALUE_WITH_REF(JSObjectMake(contextRef(), NULL, NULL));
	}
	
	inline void Context::defaultExceptionHandler(const Value& inException, const String& exceptionContext)
	{
		String exceptionAsString = (String)inException;
		throw Exception("JavaScript exception: " + exceptionAsString + " while " + exceptionContext);
	}
	
	inline void Context::handleException(JSValueRef inException, const String& context) const
	{
		Value	exceptionValue = detail::from_valueRef<Value>::convert(*this, inException);
		this->exceptionHandler()(exceptionValue, context);
	}
	
	inline std::vector<Value>	Context::valueArrayWithValueRefArray(size_t argumentCount, const JSValueRef inArguments[]) const
	{
		std::vector<Value>	result;
		for(size_t i = 0; i < argumentCount; ++i) {
			result.push_back(VALUE_WITH_REF(inArguments[i]));
		}
		return result;
	}
#undef VALUE_WITH_REF
	
	namespace detail {
		
		inline void	CallbackContext::setException(const std::exception& ex)
		{
			if (mExceptionRef)
				*mExceptionRef = detail::to_valueRef<String>::convert(*this, ex.what());
		}

	}
	
	inline Context::context_stack_type& Context::stack() {
		static boost::thread_specific_ptr<context_stack_type>	sCurrentContext;
		if (!sCurrentContext.get())
			sCurrentContext.reset(new context_stack_type);
		return *sCurrentContext;
	}
	
	inline Context& Context::currentContext()
	{
		return detail::FunctionCallContext::currentContext();
	}
	
	inline std::vector<Value> Context::currentArguments()
	{
		detail::FunctionCallContext& context(detail::FunctionCallContext::currentContext());
		return context.valueArrayWithValueRefArray(context.argumentCount, context.arguments);
	}
	
	inline std::size_t Context::currentArgumentCount()
	{
		detail::FunctionCallContext& context(detail::FunctionCallContext::currentContext());
		return context.argumentCount;
	}
	
	inline Value Context::currentThisObject()
	{
		detail::FunctionCallContext& context(detail::FunctionCallContext::currentContext());
		return detail::from_valueRef<Value>::convert(context, context.thisObject);
	}
	


}}

#endif // defined __jerome_scripting_javascript_impl_context_imp_hpp__
