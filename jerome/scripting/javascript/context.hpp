//
//  context.hpp
//
//  Created by Anton Leuski on 8/5/14.
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

#ifndef __jerome_scripting_javascript_context_hpp__
#define __jerome_scripting_javascript_context_hpp__

#include <vector>
#include <stack>
#include <JavaScriptCore/JavaScript.h>

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
    
    struct ValueArray;
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

    void protect(JSValueRef valueRef) const;
    void unprotect(JSValueRef valueRef) const;
    
    Value makeBoolean(bool value) const;
    Value makeNumber(double value) const;
    Value makeString(const String& value) const;
    
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
		
    
    // Note that you can achieve the same effect by defining a template
    // function. However, that function would have to create the string
    // context description every time you call it. The macros do not have
    // that issue.

#define JEROME_CALL_JS_API(TYPE, F, R, CTX, ARGS...) \
JSValueRef	exception = nullptr; \
TYPE	result = F(this->contextRef(), ARGS, &exception); \
if (exception) this->handleException(exception, CTX); \
return R

#define JEROME_CALL_JS_API_RETURN(TYPE, F, CTX, ARGS...) \
JSValueRef	exception = nullptr; \
TYPE	result = F(this->contextRef(), ARGS, &exception); \
if (exception) this->handleException(exception, CTX); \
return result

#define JEROME_CALL_JS_API_VOID(F, CTX, ARGS...) \
JSValueRef	exception = nullptr; \
F(this->contextRef(), ARGS, &exception); \
if (exception) this->handleException(exception, CTX)

    bool isEqual(JSValueRef a, JSValueRef b) const
    {
      JEROME_CALL_JS_API_RETURN(bool, JSValueIsEqual,
                         "checking equality",
                         a, b);
    }

    bool isInstanceOfConstructor(JSValueRef a, JSObjectRef b) const
    {
      JEROME_CALL_JS_API_RETURN(bool, JSValueIsInstanceOfConstructor,
                         "checking if object is constructor",
                         a, b);
    }

    detail::JSString createJSONString(JSValueRef value, unsigned indent) const
    {
      JEROME_CALL_JS_API(JSStringRef, JSValueCreateJSONString,
                         detail::JSString(result),
                         "converting to JSON",
                         value, indent);
    }
    
    bool deleteProperty(JSObjectRef a, const String& propertyName) const
    {
      JEROME_CALL_JS_API_RETURN(bool, JSObjectDeleteProperty,
                         "deleting property " + propertyName,
                         a, detail::JSString(propertyName).jsStringRef());
    }
    
    JSValueRef getProperty(JSObjectRef a, const String& propertyName) const
    {
      JEROME_CALL_JS_API_RETURN(JSValueRef, JSObjectGetProperty,
                         "getting property " + propertyName,
                         a, detail::JSString(propertyName).jsStringRef());
    }

    void setProperty(JSObjectRef a, const String& propertyName,
                           JSValueRef value) const
    {
      JEROME_CALL_JS_API_VOID(JSObjectSetProperty,
                         "setting property " + propertyName,
                         a, detail::JSString(propertyName).jsStringRef(),
                              value, 0);
    }

    JSValueRef getPropertyAtIndex(JSObjectRef a, unsigned propertyIndex) const
    {
      JEROME_CALL_JS_API_RETURN(JSValueRef, JSObjectGetPropertyAtIndex,
                         "getting property " + std::to_string(propertyIndex),
                         a, propertyIndex);
    }
    
    void setPropertyAtIndex(JSObjectRef a, unsigned propertyIndex,
                     JSValueRef value) const
    {
      JEROME_CALL_JS_API_VOID(JSObjectSetPropertyAtIndex,
                              "setting property " + std::to_string(propertyIndex),
                              a, propertyIndex, value);
    }

    double toNumber(JSValueRef value) const
    {
      JEROME_CALL_JS_API_RETURN(double, JSValueToNumber,
                         "converting to number",
                         value);
    }
    
    JSObjectRef makeArray(size_t argumentCount, const JSValueRef arguments[]) const
    {
      JEROME_CALL_JS_API_RETURN(JSObjectRef, JSObjectMakeArray,
                         "making an array",
                         argumentCount, arguments);
    }
    
    detail::JSString toStringCopy(JSValueRef valueRef) const
    {
      JEROME_CALL_JS_API(JSStringRef, JSValueToStringCopy,
                         detail::JSString(result),
                         "making a string",
                         valueRef);
    }

		typedef std::function<void(const Value& exception,
                               const String& exceptionContext)> exception_handler_type;

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
		template <typename, typename> friend struct detail::to_value;
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
		
    template <typename ...As>
    Value makeRegExp(As ...as);

    template <typename ...As>
    Value makeDate(As ...as);

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
	template <> struct hash<jerome::javascript::ContextGroup> {
    typedef jerome::javascript::ContextGroup argument_type;
    typedef std::size_t result_type;

		size_t operator()(jerome::javascript::ContextGroup __v) const 
		{ return hash<JSContextGroupRef>()((JSContextGroupRef)__v); }
	};
}


#endif // defined __jerome_scripting_javascript_context_hpp__
