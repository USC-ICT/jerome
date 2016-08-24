//
//  value.hpp
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

#ifndef __jerome_scripting_javascript_value_hpp__
#define __jerome_scripting_javascript_value_hpp__

#include <type_traits>
#include <unordered_map>
#include <JavaScriptCore/JavaScript.h>
#include <jerome/scripting/javascript/exception.hpp>
#include <jerome/scripting/javascript/types.hpp>
#include <jerome/scripting/javascript/context.hpp>
#include <jerome/scripting/javascript/string.hpp>
#include <jerome/scripting/javascript/abstract_value.hpp>

namespace jerome { namespace javascript {

	
	struct ValueBase : public detail::AbstractValueRef<ValueBase> {
		JSValueRef		valueRef() const	{ return mValueRef; }

		ValueBase(JSValueRef inValueRef)
		: mValueRef(inValueRef)
		{}
		
	protected:
		JSValueRef	mValueRef;
	};
	
  namespace detail {
    struct ValueArray;
  }
  
	struct Value : public detail::AbstractValue<Value, ValueBase> {
		typedef detail::AbstractValue<Value, ValueBase> parent_type;
		
		typedef JSType Type;
		
		friend struct detail::from_valueRef<Value>;
    friend struct detail::to_value<Value>;
		
		~Value()
		{
      context().unprotect(mValueRef);
		}
		
		const Context&	context() const		{ return mContext; }
		Context&		context()			{ return mContext; }

		JSValueRef		valueRef() const	{ return mValueRef; }
    JSObjectRef   objectRef() const	{ return (JSObjectRef)valueRef(); }
    
    JSValueRef		returnValueRef() const	{
      context().protect(mValueRef);
      return mValueRef;
    }
    JSValueRef		returnRetainedValueRef() const	{
      context().protect(mValueRef);
      return mValueRef;
    }

		Value(const Value& inOther)
		: parent_type(inOther.mValueRef)
		, mContext(inOther.mContext)
		{
			assert(mValueRef);
      context().protect(mValueRef);
		}

		Value(Value&& inOther)
		: parent_type(inOther.mValueRef)
		, mContext(std::forward<Context>(inOther.mContext))
		{
			inOther.mValueRef = nullptr;
			assert(mValueRef);
		}
		
		template <typename P, typename D>
		Value(const detail::Property<P,D>& inOther)
		: parent_type(inOther.valueRef())
		, mContext(inOther.context())
		{
			assert(mValueRef);
      context().protect(mValueRef);
		}

		using parent_type::operator=;
		
//		template <typename P, typename D>
//		Value& operator=(const detail::AbstractValue<P,D>& inOther)
//		{
//			return assign(inOther.context(), inOther.valueRef());
//		}
//
		Value& operator=(const Value& inOther)
		{
			return assign(inOther.context(), inOther.valueRef());
		}

		Value& operator=(Value&& inOther)
		{
			if (this != &inOther) {
				std::swap<Context>(mContext, inOther.mContext);
				std::swap<JSValueRef>(mValueRef, inOther.mValueRef);
			}
			return *this;
		}

//		template <typename P, typename D>
//		Value& operator=(const detail::Property<P,D>& inOther)
//		{
//			return this->operator=((typename detail::Property<P,D>::parent_type)inOther);
//		}
//
		template <typename P>
		Value& operator=(const detail::NamedProperty<P>& inOther)
		{
			return assign(inOther.context(), inOther.valueRef());
		}

		template <typename P>
		Value& operator=(const detail::IndexedProperty<P>& inOther)
		{
			return assign(inOther.context(), inOther.valueRef());
		}

		template <typename P>
		Value& operator=(const detail::PrototypeProperty<P>& inOther)
		{
			return assign(inOther.context(), inOther.valueRef());
		}

	protected:
    friend struct detail::ValueArray;
    
		template <typename, typename> friend struct detail::AbstractValue;

		Value(const Context& inContext, JSValueRef inValueRef)
		: parent_type(inValueRef)
		, mContext(inContext)
		{
			assert(mValueRef);
      context().protect(mValueRef);
		}
		
		void				setValueRef(JSValueRef inValueRef)
		{
			if (mValueRef != inValueRef) {
        context().unprotect(mValueRef);
				mValueRef 	= inValueRef;
        context().protect(mValueRef);
			}
		}
		
	private:
		
		Value& assign(const Context& inContext, JSValueRef inValueRef)
		{
      if (mContext != inContext) {
        if (mValueRef != inValueRef) {
          context().unprotect(mValueRef);
          mContext 	= inContext;
          mValueRef 	= inValueRef;
          context().protect(mValueRef);
        } else {
          inContext.protect(mValueRef);
          mContext.unprotect(mValueRef);
          mContext = inContext;
        }
      } else {
        setValueRef(inValueRef);
      }
			return *this;
		}
		
		Context		mContext;
	};
		
  
  
  inline std::ostream& operator << (std::ostream& outs, const Value& obj)
  {
    std::string t;
    std::string v = "n/a";
    switch (obj.type()) {
      case kJSTypeUndefined : t = "undefined"; break;
      case kJSTypeNull : t = "null"; v = "null"; break;
      case kJSTypeBoolean : t = "boolean"; v = std::to_string((bool)obj); break;
      case kJSTypeNumber : t = "number"; v = std::to_string((double)obj); break;
      case kJSTypeString : t = "string"; v = (std::string)obj; break;
      case kJSTypeObject :
        if (obj.isFunction()) {
          t = "function";
        } else if (obj.isConstructor()) {
          t = "constructor";
        } else {
          t = "object";
        }
        if (obj.isNull()) {
          v = "NULL";
        } else {
          v = obj.toJSONString(1);
        }
        break;
      default: t = "unknown";
    }
    return outs << "JSValue of type " << t << " = " << v;
  }

  namespace detail {
    struct ValueArray {
      ValueArray() = delete;
      ValueArray(const ValueArray&) = delete;
      
      ~ValueArray() {
        for(auto& x : mStorage) {
          mContext.unprotect(x);
        }
      }
      
      template< class InputIt >
      ValueArray(const Context& inContext, InputIt first, InputIt last)
      : mContext(inContext)
      {
        mStorage.reserve(last-first);
        for(; first != last; ++first) {
          mStorage.push_back(first->returnRetainedValueRef());
        }
      }

      ValueArray(const Context& inContext, std::initializer_list<Value> init)
      : mContext(inContext)
      {
        mStorage.reserve(init.size());
        for(const auto& x : init) {
          mStorage.push_back(x.returnRetainedValueRef());
        }
      }

      Value array() const {
        return Value(mContext, (JSValueRef)mContext.makeArray(size(), data()));
      }
      std::size_t size() const { return mStorage.size(); }
      const JSValueRef* data() const { return mStorage.data(); }
      std::vector<JSValueRef>::const_iterator begin() const { return mStorage.begin(); }
      std::vector<JSValueRef>::const_iterator end() const { return mStorage.end(); }
    private:
      Context		mContext;
      std::vector<JSValueRef> mStorage;
    };
  }
}}



#endif // defined __jerome_scripting_javascript_value_hpp__
