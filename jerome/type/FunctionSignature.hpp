//
//  FunctionSignature.hpp
//
//  Created by Anton Leuski on 9/29/15.
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

#ifndef __jerome_type_FunctionSignature_hpp__
#define __jerome_type_FunctionSignature_hpp__

namespace jerome {

		template <typename T, typename = void>
		struct is_static_function_pointer : public std::false_type {};
		
		template <typename T>
		struct is_static_function_pointer<T, typename std::enable_if<
		std::is_pointer<T>::value
		&& std::is_function<typename std::remove_pointer<T>::type>::value
		>::type> : public std::true_type {};

		template <class T>
		struct remove_const_from_signature {typedef T type;};
		template <class R, class ...Args>
		struct remove_const_from_signature<R(Args...) const> {typedef R(type)(Args...);};
		
		template <class T>
		struct remove_volatile_from_signature {typedef T type;};
		template <class R, class ...Args>
		struct remove_volatile_from_signature<R(Args...) volatile> {typedef R(type)(Args...);};
		
		template <class T>
		struct remove_cv_from_signature {
			typedef typename remove_volatile_from_signature<typename remove_const_from_signature<T>::type>::type type;
		};
		
		template <typename>
		struct member_traits_impl;
		
		template <typename C, typename M>
		struct member_traits_impl<M C::*> {
			typedef M	value_type;
			typedef C	class_type;
		};
		
		template<typename T>
		struct has_regular_call_operator
		{
			template<typename C> // detect regular operator()
			static char test(decltype(&C::operator()));
			
			template<typename C> // worst match
			static char (&test(...))[2];
			
			static const bool value = (sizeof( test<T>(0)  ) == 1);
		};
		
		template <typename T, typename Enabled = void>
		struct function_signature;// { typedef int type; };
		
		template <typename T>
		struct function_signature<T, typename std::enable_if<has_regular_call_operator<T>::value>::type> {
			typedef typename function_signature<decltype(&T::operator())>::type type;
//			typedef int type;
		};
		
		template <typename T>
		struct function_signature<T, typename std::enable_if<is_static_function_pointer<T>::value>::type> {
			typedef typename remove_cv_from_signature<
				typename std::remove_pointer<
					typename std::remove_cv<T>::type
				>::type
			>::type type;
		};

		template <typename T>
		struct function_signature<T, typename std::enable_if<std::is_member_function_pointer<T>::value>::type> {
			typedef typename remove_cv_from_signature<
				typename member_traits_impl<
					typename std::remove_cv<T>::type
				>::value_type
			>::type type;
		};

		template <typename, typename = void>
		struct is_callable : public std::false_type {};

		template <typename _Fp>
		struct is_callable<_Fp, typename std::enable_if<
			std::is_function<
				typename function_signature<_Fp>::type
			>::value
		>::type>
		: public std::integral_constant<bool, std::is_convertible<_Fp,
			std::function<
				typename function_signature<_Fp>::type
			>
		>::value>{};

		template <typename, typename = void>
		struct is_static_callable : public std::false_type {};
		
		template <typename _Fp>
		struct is_static_callable<_Fp, typename std::enable_if<
			std::is_function<
				typename function_signature<_Fp>::type
			>::value
		>::type>
		: public std::integral_constant<bool, std::is_convertible<_Fp,
			typename std::add_pointer<
				typename function_signature<_Fp>::type
			>::type
		>::value>{};

		// these should appear in a C++14 compatible library
		
		template<std::size_t... Indices>
		struct index_sequence {
			using next = index_sequence<Indices..., sizeof...(Indices)>;
		};
		
		template<std::size_t Size>
		struct make_index_sequence {
			using type = typename make_index_sequence<Size - 1>::type::next;
		};
		
		template<>
		struct make_index_sequence<0> {
			using type = index_sequence<>;
		};
		
		template<class... T>
		using index_sequence_for = typename make_index_sequence<sizeof...(T)>::type;

}

#endif // defined __jerome_type_FunctionSignature_hpp__
