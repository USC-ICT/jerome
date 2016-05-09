//
//  abstract_value.hpp
//
//  Created by Anton Leuski on 8/20/14.
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

#ifndef __jerome_javascript_abstract_value_hpp__
#define __jerome_javascript_abstract_value_hpp__

#include <jerome/types.hpp>

namespace jerome { namespace javascript {

	struct Context;
	
	namespace detail {
		
		template <typename Peer, typename Derived> struct Property;
		template <typename Peer> struct NamedProperty;
		template <typename Peer> struct IndexedProperty;
		template <typename Peer> struct PrototypeProperty;
		template <typename Peer> struct ExceptionHandlerProperty;
		
		template <typename Derived>
		struct AbstractValueRef {
			void*	getPrivate() const
			{ return JSObjectGetPrivate(objectRef());	}
			bool	setPrivate(void* data)
			{ return JSObjectSetPrivate(objectRef(), data); }
		protected:
			template <typename, typename> friend struct Property;
			friend struct jerome::javascript::Value;
			
			AbstractValueRef() {}
			
			const Derived*		derived() const		{ return static_cast<const Derived*>(this); }
			Derived*			derived()			{ return static_cast<Derived*>(this); }
			JSValueRef			valueRef() const	{ return derived()->valueRef(); }
			JSObjectRef			objectRef() const	{ return (JSObjectRef)valueRef(); }
		};
		
		template <typename Derived, typename Base>
		struct AbstractValue : public Base {
			using Base::Base;
			
			JSType	type() const
			{ return JSValueGetType(contextRef(), this->valueRef()); }
			bool	isUndefined() const
			{ return JSValueIsUndefined(contextRef(), this->valueRef()); }
			bool	isNull() const
			{ return JSValueIsNull(contextRef(), this->valueRef()); }
			bool	isBoolean() const
			{ return JSValueIsBoolean(contextRef(), this->valueRef()); }
			bool	isNumber() const
			{ return JSValueIsNumber(contextRef(), this->valueRef()); }
			bool	isString() const
			{ return JSValueIsString(contextRef(), this->valueRef()); }
			bool	isObject() const
			{ return JSValueIsObject(contextRef(), this->valueRef()); }
			bool	isFunction() const
			{ return isObject() && JSObjectIsFunction(contextRef(), this->objectRef()); }
			bool	isConstructor() const
			{ return isObject() && JSObjectIsConstructor(contextRef(), this->objectRef()); }
			bool	isObjectOfClass(const Class& inClass) const
			{ return JSValueIsObjectOfClass(contextRef(), this->valueRef(), inClass); }
			template <typename T>
			bool	isEqual(const AbstractValueRef<T>& b) const
			{ return context().isEqual(this->valueRef(), b.valueRef()); }
			template <typename T>
			bool	isStrictEqual(const AbstractValueRef<T>& b) const
			{ return JSValueIsStrictEqual(contextRef(), this->valueRef(), b.valueRef()); }
			template <typename T>
			bool	isInstanceOfConstructor(const AbstractValueRef<T>& b) const
			{ return context().isInstanceOfConstructor(this->valueRef(), b.objectRef()); }
			String	toJSONString(unsigned indent) const
			{ return context().createJSONString(this->valueRef(), indent).string(); }
			bool	hasProperty(const String& propertyName) const
			{ return JSObjectHasProperty(contextRef(), this->objectRef(),
										 detail::JSString(propertyName)); }
			void	deleteProperty(const String& propertyName)
			{ context().deleteProperty(this->objectRef(), propertyName); }
			
			template <typename T>
			explicit operator T()		const
			{ return detail::from_valueRef<T>::convert(context(), this->valueRef()); }
			
//			template <typename T>
//			AbstractValue& operator = (T&& x)
//			{ return this->operator=(detail::to_value<T>::convert(context(), std::forward<T>(x))); }
//			
//			AbstractValue& operator = (JSValueRef inValueRef)
//			{ this->setValueRef(inValueRef); return *this; }
			
			std::vector<String> propertyNames() const
			{
				::JSPropertyNameArrayRef	array	= JSObjectCopyPropertyNames(contextRef(), this->objectRef());
				std::vector<String>		result;
				for(size_t i = 0, n = JSPropertyNameArrayGetCount(array); i < n; ++i) {
					result.push_back(detail::JSString(JSPropertyNameArrayGetNameAtIndex(array, i)).string());
				}
				JSPropertyNameArrayRelease(array);
				return result;
			}
			
			template <typename ...Args>
			Value	operator() (Args ...args);
			
			template <typename THIS_T, typename ...Args>
			Value	call(THIS_T this_arg, Args ...args);
			
			template <typename ...Args>
			Value	callAsConstructor(Args ...args);
			
			PrototypeProperty<Derived>	prototype();
			Value						prototype() const;
			
			NamedProperty<Derived>		operator[] (const String& propertyName);
			Value						operator[] (const String& propertyName) const;
			
			NamedProperty<Derived>		operator[] (const char* propertyName);
			Value						operator[] (const char* propertyName) const;
			
			IndexedProperty<Derived>	operator[] (int propertyIndex);
			Value						operator[] (int propertyIndex) const;
			
		protected:
			template <typename, typename> friend struct Property;
			friend struct jerome::javascript::Value;
			
			const Derived*		derived() const		{ return static_cast<const Derived*>(this); }
			Derived*			derived()			{ return static_cast<Derived*>(this); }
			const Context&		context() const		{ return this->derived()->context(); }

			Context& context()			{ return this->derived()->context(); }

      void handleException(JSValueRef exception, const String& ctx) const
      {
        context().handleException(exception, ctx);
      }

			JSContextRef		contextRef() const	{ return context().contextRef(); }
			void				setValueRef(JSValueRef inValueRef)
			{ this->derived()->setValueRef(inValueRef); }
		};
		
		template <typename Peer, typename Derived>
		struct Property : public AbstractValue<Derived, AbstractValueRef<Derived>> {
			typedef AbstractValue<Derived, AbstractValueRef<Derived>> parent_type;
			template <typename, typename> friend struct AbstractValue;
			
			using parent_type::parent_type;
			
			explicit Property(Peer& inSource)
			: mSource(inSource)
			{}
			
			Property(const Property<Peer, Derived>& inOther)
			: mSource(inOther.mSource)
			{}
			
			Property(Property<Peer, Derived>&& inOther)
			: mSource(std::forward<Peer&>(inOther.mSource))
			{}
			
			using parent_type::operator =;
			
			template <typename D, typename B>
			Property& operator = (const AbstractValue<D, B>& inOther) {
				assert(context() == inOther.context());
				this->setValueRef(inOther.valueRef());
				return *this;
			}
			
		protected:
			Peer&	mSource;

			friend struct jerome::javascript::Value;
			template <typename, typename> friend struct AbstractValue;
			template <typename, typename> friend struct Property;
			
			const Peer&			source() const		{ return mSource; }
			Peer&			source() 		{ return mSource; }
			const Context&	context() const		{ return source().context(); }
			
			Context& context()			{ return source().context(); }
			
			JSObjectRef		sourceRef() const	{ return source().objectRef(); }
		};
		
		template <typename Peer>
		struct PrototypeProperty : public Property<Peer, PrototypeProperty<Peer>> {
			typedef Property<Peer, PrototypeProperty<Peer>>		parent_type;
			
			using parent_type::parent_type;
			using parent_type::operator =;
			
			JSValueRef	valueRef() const
			{ return JSObjectGetPrototype(this->contextRef(), this->sourceRef()); }
			
			void		setValueRef(JSValueRef inValueRef)
			{ JSObjectSetPrototype(this->contextRef(), this->sourceRef(), inValueRef); }
		};
		
		template <typename Peer, typename Derived, typename Selector>
		struct SelectorProperty : public Property<Peer, Derived> {
			typedef Property<Peer, Derived>		parent_type;
			
			SelectorProperty(Peer& inSource, Selector inSelector)
			: parent_type(inSource)
			, mSelector(inSelector)
			{}
			
			SelectorProperty(const NamedProperty<Peer>& inOther)
			: parent_type(inOther)
			, mSelector(inOther.mSelector)
			{}
			
			SelectorProperty(NamedProperty<Peer>&& inOther)
			: parent_type(std::forward<parent_type>(inOther))
			, mSelector(std::move(inOther.mSelector))
			{}
			
			using parent_type::operator =;
			
		protected:
			Selector	mSelector;
		};

		template <typename Peer>
		struct NamedProperty : public SelectorProperty<Peer, NamedProperty<Peer>, String> {
			typedef SelectorProperty<Peer, NamedProperty<Peer>, String>		parent_type;
			
			using parent_type::parent_type;
			using parent_type::operator =;
			
			JSValueRef	valueRef() const
			{
				return this->context().getProperty(this->sourceRef(), this->mSelector);
			}
			void		setValueRef(JSValueRef inValueRef)
			{
				this->context().setProperty(this->sourceRef(), this->mSelector, inValueRef);
			}
		};
		
		template <typename Peer>
		struct IndexedProperty : public SelectorProperty<Peer, IndexedProperty<Peer>, unsigned> {
			typedef SelectorProperty<Peer, IndexedProperty<Peer>, unsigned>		parent_type;
			
			using parent_type::parent_type;
			using parent_type::operator =;
			
			JSValueRef	valueRef() const
			{
				return this->context().getPropertyAtIndex(this->sourceRef(),
                                                  this->mSelector);
			}
			void		setValueRef(JSValueRef inValueRef)
			{
				this->context().setPropertyAtIndex(this->sourceRef(), this->mSelector,
                                           inValueRef);
			}
		};
		
	}


}}

#endif // __jerome_javascript_abstract_value_hpp__