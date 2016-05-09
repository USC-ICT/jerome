//
//  value_imp.hpp
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

#ifndef __jerome_javascript_value_imp_hpp__
#define __jerome_javascript_value_imp_hpp__

namespace jerome { namespace javascript { namespace detail {

	template <typename Derived, typename Base>
	template <typename ...Args>
	Value	AbstractValue<Derived, Base>::operator () (Args ...as)
	{
		if (!isFunction()) throw object_is_not_function();

    detail::ValueArray args(this->context(), {to_value<Args>::convert(this->context(), as)... });
    
    JEROME_CALL_JS_API(JSValueRef, JSObjectCallAsFunction,
                       detail::from_valueRef<Value>::convert(this->contextRef(), result),
                       "calling a function",
                       this->objectRef(), nullptr,
                       args.size(), args.data());
	}
	
	template <typename Derived, typename Base>
	template <typename THIS_T, typename ...Args>
	Value	AbstractValue<Derived, Base>::call(THIS_T this_arg, Args ...as)
	{
		if (!isFunction()) throw object_is_not_function();
    
    detail::ValueArray args(this->context(), {to_value<Args>::convert(this->context(), as)... });

    JEROME_CALL_JS_API(JSValueRef, JSObjectCallAsFunction,
                       detail::from_valueRef<Value>::convert(this->contextRef(), result),
                       "calling a function",
                       this->objectRef(),
                       (JSObjectRef)to_value<THIS_T>::convert(context(), this_arg).valueRef(),
                       args.size(), args.data());
	}
	
	template <typename Derived, typename Base>
	template <typename ...Args>
	Value	AbstractValue<Derived, Base>::callAsConstructor(Args ...as)
	{
		if (!isFunction()) throw object_is_not_constructor();
    
    detail::ValueArray args(this->context(), {to_value<Args>::convert(this->context(), as)... });

    JEROME_CALL_JS_API(JSObjectRef, JSObjectCallAsConstructor,
                       detail::from_valueRef<Value>::convert(this->contextRef(), result),
                       "calling object as constructor",
                       this->objectRef(), args.size(), args.data());
	}
		
	template <typename Derived, typename Base>
	PrototypeProperty<Derived>	AbstractValue<Derived, Base>::prototype()
	{
		return PrototypeProperty<Derived>(*this->derived());
	}
	
	template <typename Derived, typename Base>
	Value	AbstractValue<Derived, Base>::prototype() const
	{
		return PrototypeProperty<const Derived>(*this->derived());
	}
	
	template <typename Derived, typename Base>
	NamedProperty<Derived>		AbstractValue<Derived, Base>::operator[] (const String& propertyName)
	{
		return NamedProperty<Derived>(*this->derived(), propertyName);
	}
	
	template <typename Derived, typename Base>
	Value						AbstractValue<Derived, Base>::operator[] (const String& propertyName) const
	{
		return NamedProperty<const Derived>(*this->derived(), propertyName);
	}
	
	template <typename Derived, typename Base>
	NamedProperty<Derived>		AbstractValue<Derived, Base>::operator[] (const char* propertyName)
	{
		return NamedProperty<Derived>(*this->derived(), propertyName);
	}
	
	template <typename Derived, typename Base>
	Value						AbstractValue<Derived, Base>::operator[] (const char* propertyName) const
	{
		return NamedProperty<const Derived>(*this->derived(), propertyName);
	}
	
	template <typename Derived, typename Base>
	IndexedProperty<Derived>	AbstractValue<Derived, Base>::operator[] (int propertyIndex)
	{
		return IndexedProperty<Derived>(*this->derived(), propertyIndex);
	}
	
	template <typename Derived, typename Base>
	Value						AbstractValue<Derived, Base>::operator[] (int propertyIndex) const
	{
		return IndexedProperty<const Derived>(*this->derived(), propertyIndex);
	}
	
	
	template <typename A, typename AB, typename B, typename BB>
	inline bool operator == (const AbstractValue<A, AB>& a, const AbstractValue<B, BB>& b) {
		return a.isEqual(b);
	}
	
	template <typename Derived, typename Base>
	std::ostream& operator<< (std::ostream& os, const AbstractValue<Derived, Base>& v ) {
		return os << (String)v;
	}

}}}

#endif // __jerome_javascript_value_imp_hpp__