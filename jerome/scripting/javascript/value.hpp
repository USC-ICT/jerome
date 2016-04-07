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
#include <JavaScriptCore/JavaScriptCore.h>
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
	
	struct Value : public detail::AbstractValue<Value, ValueBase> {
		typedef detail::AbstractValue<Value, ValueBase> parent_type;
		
		typedef JSType Type;
		
		friend struct detail::from_valueRef<Value>;
		friend struct detail::to_valueRef<Value>;
		
		~Value()
		{
			if (mValueRef) JSValueUnprotect(contextRef(), mValueRef);
		}
		
		const Context&	context() const		{ return mContext; }
		Context&		context()			{ return mContext; }

		JSValueRef		valueRef() const	{ return mValueRef; }

		Value(const Value& inOther)
		: parent_type(inOther.mValueRef)
		, mContext(inOther.mContext)
		{
			assert(mValueRef);
			JSValueProtect(contextRef(), mValueRef);
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
			JSValueProtect(contextRef(), mValueRef);
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
		template <typename, typename> friend struct detail::AbstractValue;

		Value(const Context& inContext, JSValueRef inValueRef)
		: parent_type(inValueRef)
		, mContext(inContext)
		{
			assert(mValueRef);
			JSValueProtect(contextRef(), mValueRef);
		}
		
		void				setValueRef(JSValueRef inValueRef)
		{
			if (mValueRef != inValueRef) {
				if (mValueRef)
					JSValueUnprotect(contextRef(), mValueRef);
				mValueRef 	= inValueRef;
				if (mValueRef)
					JSValueProtect(contextRef(), mValueRef);
			}
		}
		
	private:
		
		Value& assign(const Context& inContext, JSValueRef inValueRef)
		{
			if (mValueRef != inValueRef || mContext != inContext) {
				if (mValueRef)
					JSValueUnprotect(contextRef(), mValueRef);
				mContext 	= inContext;
				mValueRef 	= inValueRef;
				if (mValueRef)
					JSValueProtect(contextRef(), mValueRef);
			}
			return *this;
		}
		
		Context		mContext;
	};
		
	
}}

#endif // defined __jerome_scripting_javascript_value_hpp__
