//
//  context.hpp
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

#ifndef __jerome_scripting_javascript_context_hpp__
#define __jerome_scripting_javascript_context_hpp__

#include <vector>
#include <stack>
#include <JavaScriptCore/JavaScriptCore.h>

#include <jerome/scripting/javascript/types.hpp>
#include <jerome/scripting/javascript/exception.hpp>
#include <jerome/scripting/javascript/abstract_value.hpp>
#include <jerome/scripting/javascript/class.hpp>

namespace jerome { namespace javascript {
	
	namespace detail {
		
		struct Context_deleter
		{
			void operator()(JSGlobalContextRef p)
			{
				if(p) JSGlobalContextRelease(p);
			}
		};
		
		struct null_deleter
		{
			void operator()(void const *) const
			{
			}
		};
		
		template <typename> struct ExceptionHandlerProperty;
	}
	
	struct Context {
		Context()
		: mData(JSGlobalContextCreate(NULL), detail::Context_deleter())
		{}
		
		Context(const Context& inOther)
		: mData(inOther.mData)
		{}

		Context(Context&& inOther)
		: mData(std::move(inOther.mData))
		{}
		
		Context& operator=(const Context& inOther)
		{
			mData = inOther.mData;
			return *this;
		}
		
		Context& operator=(Context&& inOther)
		{
			mData = std::move(inOther.mData);
			return *this;
		}
		
		Value evaluateScript(const String& script,
							 Value& thisObject,
							 const String& sourceURL = "",
							 int startingLineNumber = 0);

		Value evaluateScript(const String& script,
							 const String& sourceURL = "",
							 int startingLineNumber = 0);

		bool checkScriptSyntax(const String& script,
							   const String& sourceURL = "",
							   int startingLineNumber = 0);

		void garbageCollect();
		Value globalObject() const;
		ContextGroup group();
		
		Value valueFromJSONString(const String& value);
		
		// accepts anything convertible to Value
		//  C++ type					JS type
		// ----------------------------------
		//  nullptr						undefined
		//	js::Null					null
		//	bool						bool
		//	number						number
		//	const char*					String
		//	String						String
		//	R(*)(A...)					callable object
		//	any object or pointer		object with mapped class
		//		C with ClassTraits<C>
		//		defined
		//	std collection				array
		//	associative collection		object with key/values mapped to properties
		
		template <typename T>
		Value newValue(T&& value);

		Value nullValue() const;
		Value undefinedValue() const;
		
		Value newDate();
		Value newDate(long milliseconds);
		Value newDate(const String& inDateString);
		Value newDate(int year, int month, int day, int hours, int minutes, int seconds, int milliseconds);
		
		Value newRegExp(const String& pattern, const String& modifiers = "");

		Value newFunction(const String& name,
						  const std::vector<String>& arguments,
						  const String& body,
						  const String& sourceURL = "",
						  int startingLineNumber = 0);
		
		Value newAnonymousFunction(const std::vector<String>& arguments,
								   const String& body,
								   const String& sourceURL = "",
								   int startingLineNumber = 0);
		
		Value newObject();
		Value newObject(const Class& clazz, void* data);
		
		template <class C>
		Value newObjectOfNativeClass(const C& inCObject);
		
		template <class C>
		Value newObjectOfNativeClass();
		
		static Context& currentContext();
		static std::vector<Value> currentArguments();
		static std::size_t currentArgumentCount();
		static Value currentThisObject();
		
		template <typename R, typename ...Args, typename ...Args1>
		R callJSCFunction(const String& inExceptionContext, R(* inFunction)(Args...), Args1 ...args) const {
			JSValueRef	exception = nullptr;
			R	result = inFunction(this->contextRef(), std::forward<Args1>(args)..., &exception);
			if (exception) this->handleException(exception, inExceptionContext);
			return result;
		}

		template <typename ...Args, typename ...Args1>
		void callJSCFunction(const String& inExceptionContext, void(* inFunction)(Args...), Args1 ...args) const {
			JSValueRef	exception = nullptr;
			inFunction(this->contextRef(), std::forward<Args1>(args)..., &exception);
			if (exception) this->handleException(exception, inExceptionContext);
		}

		template <typename R, typename ...Args, typename ...Args1>
		Value callJSCFunctionReturnValue(const String& inExceptionContext,
										 R(*inFunction)(Args...),
										 Args1 ...args) const;

		typedef std::function<void(const Value& exception, const String& exceptionContext)> exception_handler_type;

		exception_handler_type 	exceptionHandler() const;
		detail::ExceptionHandlerProperty<Context> exceptionHandler();
		
		// these are mapped to the corresponding properties of the global object
		detail::NamedProperty<Context>		operator[] (const String& propertyName);
		Value								operator[] (const String& propertyName) const;
		
		detail::NamedProperty<Context>		operator[] (const char* propertyName);
		Value								operator[] (const char* propertyName) const;
		
		detail::IndexedProperty<Context>	operator[] (int propertyIndex);
		Value								operator[] (int propertyIndex) const;

		explicit operator JSGlobalContextRef () const { return contextRef(); }
		
	private:
		
		typedef std::shared_ptr<OpaqueJSContext>	data_type;
		
		friend struct ContextGroup;
		friend struct UserClass;
		template <typename, typename> friend struct detail::from_valueRef;
		template <typename, typename> friend struct detail::to_valueRef;
		template <typename, typename> friend struct detail::AbstractValue;
		template <typename> friend struct detail::ExceptionHandlerProperty;
		
		friend bool operator == (const Context& a, const Context& b);
		friend bool operator != (const Context& a, const Context& b);
		
		JSGlobalContextRef contextRef() { return mData.get(); }
		JSGlobalContextRef contextRef() const { return mData.get(); }
		
		data_type	mData;
		
		void handleException(JSValueRef exception, const String& context) const;
		static void defaultExceptionHandler(const Value& exception, const String& exceptionContext);

		std::vector<Value>	valueArrayWithValueRefArray(size_t argumentCount, const JSValueRef inArguments[]) const;
		
		// so we can access global object properties as context properties
		template <typename, typename> friend struct detail::Property;
		Context& context() { return *this; }
		const Context& context() const { return *this; }
		JSObjectRef		objectRef() const { return JSContextGetGlobalObject(contextRef()); }
		
	protected:
		Context(JSContextRef p)
		: mData(const_cast<OpaqueJSContext*>(p), detail::null_deleter())
		{}
		
		Context(const data_type& p)
		: mData(p)
		{}

		typedef std::stack<Context*>	context_stack_type;

		static context_stack_type& stack();
	};

	inline bool operator == (const Context& a, const Context& b) {
		return a.contextRef() == b.contextRef();
	}

	inline bool operator != (const Context& a, const Context& b) {
		return a.contextRef() != b.contextRef();
	}

	namespace detail {
		template <typename Peer>
		struct ExceptionHandlerProperty : public Property<Peer, ExceptionHandlerProperty<Peer>> {
			typedef Property<Peer, ExceptionHandlerProperty<Peer>>		parent_type;
			typedef Context::exception_handler_type						function_type;
			using parent_type::parent_type;
			
			ExceptionHandlerProperty& operator = (const function_type& inHandler)
			{
				set(inHandler);
				return *this;
			}
			
			operator function_type() const
			{
				return get();
			}
			
			function_type	get() const;
			void		set(const function_type& inValue);
		};
    
	}
	
	
	namespace detail {
		
		struct CallbackContext : public Context {
			CallbackContext(JSContextRef ctx, JSValueRef* exc)
			: Context(ctx)
			, mExceptionRef(exc)
			{
				if (mExceptionRef) *mExceptionRef = nullptr;
			}
			
			void	setException(const std::exception& ex);
		private:
			JSValueRef* mExceptionRef;
		};
		
		struct FunctionCallContext : public CallbackContext {
			FunctionCallContext(JSContextRef ctx, JSObjectRef function,
								JSObjectRef thisObject, size_t argumentCount,
								const JSValueRef arguments[], JSValueRef* exc)
			: CallbackContext(ctx, exc)
			, function(function)
			, thisObject(thisObject)
			, argumentCount(argumentCount)
			, arguments(arguments)
			{
				stack().push(this);
			}
			
			~FunctionCallContext()
			{
				stack().pop();
			}
			
			static FunctionCallContext& currentContext() {
				if (stack().empty()) throw not_in_call_context();
				FunctionCallContext*	ctx = static_cast<FunctionCallContext*>(stack().top());
				if (!ctx) throw not_in_call_context();
				return *ctx;
			}
			
		private:
			friend struct Context;
			
			JSObjectRef function;
			JSObjectRef thisObject;
			size_t argumentCount;
			const JSValueRef* arguments;
		};

	}
	

#pragma mark - ContextGroup
	
	namespace detail {
		
		struct ContextGroup_deleter
		{
			void operator()(JSContextGroupRef p)
			{
				if(p) JSContextGroupRelease(p);
			}
		};
		
	}
	
	struct ContextGroup {
		ContextGroup()
		: mData(JSContextGroupCreate(), detail::ContextGroup_deleter())
		{}
		
		operator JSContextGroupRef() { return contextGroup(); }
		operator JSContextGroupRef() const { return contextGroup(); }
		
		Context newContext()
		{
			return Context(Context::data_type(JSGlobalContextCreateInGroup(contextGroup(), NULL),
											  detail::Context_deleter()));
		}
		
		Context newContext(const Class& inClass)
		{
			return Context(Context::data_type(JSGlobalContextCreateInGroup(contextGroup(), inClass),
											  detail::Context_deleter()));
		}
		
	private:
		ContextGroup(JSContextGroupRef group)
		: mData(group ? JSContextGroupRetain(group) : group, detail::ContextGroup_deleter())
		{}
		
		JSContextGroupRef contextGroup() { return mData.get(); }
		JSContextGroupRef contextGroup() const { return mData.get(); }
		
		friend class Context;
		std::shared_ptr<const OpaqueJSContextGroup>	mData;
	};
	
	inline bool operator == (const ContextGroup& a, const ContextGroup& b) {
		return ((JSContextGroupRef)a) == ((JSContextGroupRef)b);
	}
}}


namespace std {
	template <> struct hash<jerome::javascript::ContextGroup> : public std::unary_function<jerome::javascript::ContextGroup, std::size_t> {
		size_t operator()(jerome::javascript::ContextGroup __v) const 
		{ return hash<JSContextGroupRef>()((JSContextGroupRef)__v); }
	};
}


#endif // defined __jerome_scripting_javascript_context_hpp__
