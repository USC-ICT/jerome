//
//  matrix_functions.hpp
//
//  Created by Anton Leuski on 2/9/14.
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

#ifndef __jerome_math_matrix_functions_hpp__
#define __jerome_math_matrix_functions_hpp__


namespace boost {
	namespace numeric {
		namespace ublas {

			// (op v) [i] = op( v [i] )
			template<class OP, class E>
			BOOST_UBLAS_INLINE
			typename boost::numeric::ublas::vector_unary_traits<E, OP>::result_type
			apply_to_all (const boost::numeric::ublas::vector_expression<E> &e, const OP& op = OP() ) {
				typedef typename boost::numeric::ublas::vector_unary_traits<E, OP>::expression_type expression_type;
				return expression_type (e ());
			}
			
			template<class T>
			struct scalar_log:
			public scalar_unary_functor<T> {
				typedef typename scalar_unary_functor<T>::argument_type argument_type;
				typedef typename scalar_unary_functor<T>::result_type result_type;
				
				static BOOST_UBLAS_INLINE
				result_type apply (argument_type t) {
					return std::log(t);
				}
			};

			template<class T>
			struct scalar_log_plus1:
			public scalar_unary_functor<T> {
				typedef typename scalar_unary_functor<T>::argument_type argument_type;
				typedef typename scalar_unary_functor<T>::result_type result_type;
				
				static BOOST_UBLAS_INLINE
				result_type apply (argument_type t) {
					return std::log(t+1.0);
				}
			};
			
			template<class T>
			struct scalar_exp:
			public scalar_unary_functor<T> {
				typedef typename scalar_unary_functor<T>::argument_type argument_type;
				typedef typename scalar_unary_functor<T>::result_type result_type;
				
				static BOOST_UBLAS_INLINE
				result_type apply (argument_type t) {
					return std::exp(t);
				}
			};

			template<class T1, class T2>
			struct scalar_pow:
			public scalar_binary_functor<T1, T2> {
				typedef typename scalar_binary_functor<T1, T2>::argument1_type argument1_type;
				typedef typename scalar_binary_functor<T1, T2>::argument2_type argument2_type;
				typedef typename scalar_binary_functor<T1, T2>::result_type result_type;
				
				static BOOST_UBLAS_INLINE
				result_type apply (argument1_type t1, argument2_type t2) {
					return std::pow((double)t1,(double)t2);
				}
			};

			// (log v) [i] = log (v [i])
			template<class E>
			BOOST_UBLAS_INLINE
			typename vector_unary_traits<E, scalar_log<typename E::value_type> >::result_type
			log (const vector_expression<E> &e) {
				typedef typename vector_unary_traits<E, scalar_log<typename E::value_type> >::expression_type expression_type;
				return expression_type (e ());
			}

			// (log v) [i] = log (v [i])
			template<class E>
			BOOST_UBLAS_INLINE
			typename vector_unary_traits<E, scalar_log_plus1<typename E::value_type> >::result_type
			log_plus1 (const vector_expression<E> &e) {
				typedef typename vector_unary_traits<E, scalar_log_plus1<typename E::value_type> >::expression_type expression_type;
				return expression_type (e ());
			}

			// (exp v) [i] = exp (v [i])
			template<class E>
			BOOST_UBLAS_INLINE
			typename vector_unary_traits<E, scalar_exp<typename E::value_type> >::result_type
			exp (const vector_expression<E> &e) {
				typedef typename vector_unary_traits<E, scalar_exp<typename E::value_type> >::expression_type expression_type;
				return expression_type (e ());
			}

			// (log m) [i] [j] = log (m [i] [j])
			template<class E>
			BOOST_UBLAS_INLINE
			typename matrix_unary1_traits<E, scalar_log<typename E::value_type> >::result_type
			log (const matrix_expression<E> &e) {
				typedef typename matrix_unary1_traits<E, scalar_log<typename E::value_type> >::expression_type expression_type;
				return expression_type (e ());
			}

			// (exp m) [i] [j] = exp (m [i] [j])
			template<class E>
			BOOST_UBLAS_INLINE
			typename matrix_unary1_traits<E, scalar_exp<typename E::value_type> >::result_type
			exp (const matrix_expression<E> &e) {
				typedef typename matrix_unary1_traits<E, scalar_exp<typename E::value_type> >::expression_type expression_type;
				return expression_type (e ());
			}

			// (pow(v, t) ) [i] = pow(v [i], t)
			template<class E1, class T2>
			BOOST_UBLAS_INLINE
			typename enable_if< is_convertible<T2, typename E1::value_type >,
			typename vector_binary_scalar2_traits<E1, const T2, scalar_pow<typename E1::value_type, T2> >::result_type
			>::type
			element_pow (const vector_expression<E1> &e1,
						const T2 &e2) {
				typedef typename vector_binary_scalar2_traits<E1, const T2, scalar_pow<typename E1::value_type, T2> >::expression_type expression_type;
				return expression_type (e1 (), e2);
			}


			// (pow(m, t)) [i] [j] = pow(m [i] [j], t)
			template<class E1, class T2>
			BOOST_UBLAS_INLINE
			typename enable_if< is_convertible<T2, typename E1::value_type>,
			typename matrix_binary_scalar2_traits<E1, const T2, scalar_pow<typename E1::value_type, T2> >::result_type
			>::type
			element_pow (const matrix_expression<E1> &e1,
						const T2 &e2) {
				typedef typename matrix_binary_scalar2_traits<E1, const T2, scalar_pow<typename E1::value_type, T2> >::expression_type expression_type;
				return expression_type (e1 (), e2);
			}
			
			
			template<class M>
			BOOST_UBLAS_INLINE
			matrix_vector_slice<M> diag(M &data) {
				const typename M::size_type s = std::min(data.size1(),data.size2());
				return matrix_vector_slice<M>(data, slice(0,1,s), slice(0,1,s));
			};
			
			template<class M>
			BOOST_UBLAS_INLINE
			const matrix_vector_slice<const M> diag(const M &data) {
				const typename M::size_type s = std::min(data.size1(),data.size2());
				return matrix_vector_slice<const M>(data, slice(0,1,s), slice(0,1,s));
			};

			

		}
	}
}

#endif // defined __jerome_math_matrix_functions_hpp__
