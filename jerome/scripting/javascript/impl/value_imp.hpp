//
//  value_imp.hpp
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
