//
//  matrix_boost.hpp
//  jerome
//
//  Created by Anton Leuski on 5/13/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_type_matrix_boost_hpp__
#define __jerome_type_matrix_boost_hpp__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"

#define BOOST_UBLAS_MOVE_SEMANTICS

#include <boost/numeric/ublas/vector_sparse.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/operation.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/range/algorithm/max_element.hpp>

#pragma clang diagnostic pop

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

namespace jerome {
  
  namespace ublas = boost::numeric::ublas;
  
  template<class T> using Vector = ublas::vector<T>;
  // sparse array
  template<class T> using SparseVector = ublas::compressed_vector<T>;
  
  typedef ublas::matrix<WeightValue, ublas::column_major>     WeightMatrix;
  typedef ublas::matrix_row<WeightMatrix>                     WeightMatrixRow;
  typedef ublas::matrix_row<const WeightMatrix>               WeightMatrixConstRow;
  typedef ublas::matrix_column<WeightMatrix>                  WeightMatrixColumn;
  typedef ublas::matrix_column<const WeightMatrix>            WeightMatrixConstColumn;
  typedef ublas::vector<WeightMatrix::value_type>             WeightVector;
  typedef ublas::compressed_vector<WeightMatrix::value_type>	SparseWeightVector;
  
  typedef ublas::symmetric_matrix<WeightMatrix::value_type, ublas::upper>			SymmetricWeightMatrix;
  typedef ublas::matrix_range<SymmetricWeightMatrix>					SymmetricWeightMatrixRange;
  
  template <class M>
  struct traits {
    typedef typename M::size_type size_type;
    typedef typename M::value_type value_type;
  };
    
  struct MatrixSize {
    traits<WeightMatrix>::size_type rowCount;
    traits<WeightMatrix>::size_type columnCount;
    
    MatrixSize() = default;
    
    MatrixSize(const WeightMatrix& m)
    : rowCount(m.size1()), columnCount(m.size2())
    {}
  };
  
  typedef ublas::scalar_matrix<WeightMatrix::value_type>
  WeightMatrixScalar;
  
  typedef ublas::scalar_vector<WeightMatrix::value_type>
  WeightVectorScalar;
  
  inline WeightMatrixScalar WeightMatrixZero(const MatrixSize& size)
  {
    std::cout << "zero " << size.rowCount << "," << size.columnCount << std::endl;
    return WeightMatrixScalar(size.rowCount, size.columnCount, 0);
  }
  
  inline WeightMatrixScalar WeightMatrixOnes(const MatrixSize& size)
  {
    std::cout << "ones " << size.rowCount << "," << size.columnCount << std::endl;
    return WeightMatrixScalar(size.rowCount, size.columnCount, 1);
  }
  
  inline WeightVectorScalar WeightVectorZero(traits<WeightVector>::size_type size)
  {
    std::cout << "zero " << size << std::endl;
    return WeightVectorScalar(size, 0);
  }
  
  inline WeightVectorScalar WeightVectorOnes(traits<WeightVector>::size_type size)
  {
    std::cout << "ones " << size << std::endl;
    return WeightVectorScalar(size, 1);
  }
  
#define JEROME_FOR_EACH_ELEMENT_OF_SPARSE_VECTOR(E, V, T) \
for (auto E = V.begin(), __end = V.end(); E != __end; ++E)
  
#define JEROME_SPARSE_VECTOR_ELEMENT_VALUE(E) (*E)
#define JEROME_SPARSE_VECTOR_ELEMENT_INDEX(E) (E.index())
  
  template <typename M, typename C>
  inline auto column(M&& m, C c)
  -> decltype(ublas::matrix_column<typename std::remove_reference<M>::type>(std::forward<M>(m), c))
  {
    return ublas::matrix_column<typename std::remove_reference<M>::type>(std::forward<M>(m), c);
  }

  template <typename M, typename C>
  inline auto row(M&& m, C c)
  -> decltype(ublas::matrix_row<typename std::remove_reference<M>::type>(std::forward<M>(m), c))
  {
    return ublas::matrix_row<typename std::remove_reference<M>::type>(std::forward<M>(m), c);
  }
  
  template <typename M>
  inline auto sum(const M& m)
  -> decltype(ublas::sum(m))
  {
    return ublas::sum(m);
  }

  template <typename A, typename B>
  inline auto prod(const A& a, const B& b)
  -> decltype(ublas::prod(a, b))
  {
    return ublas::prod(a, b);
  }

  template <typename A, typename B>
  inline auto outer_prod(const A& a, const B& b)
  -> decltype(ublas::outer_prod(a, b))
  {
    return ublas::outer_prod(a, b);
  }

  template <typename A, typename B>
  inline auto element_pow(const A& a, B b)
  -> decltype(ublas::element_pow(a, b))
  {
    return ublas::element_pow(a, b);
  }

  template <typename A, typename B>
  inline auto element_prod(const A& a, const B& b)
  -> decltype(ublas::element_prod(a, b))
  {
    return ublas::element_prod(a, b);
  }
  
  template <typename M>
  inline SparseWeightVector sparse_log_plus1(const M& m)
  {
    SparseWeightVector result(m.size());
    for (auto a_it = m.begin(), a_end = m.end(); a_it != a_end; ++a_it) {
      result(a_it.index()) = std::log(1.0+ (*a_it));
    }
    
    return result;
  }

  template <typename M>
  inline auto max_element(const M& m)
  -> decltype(*boost::max_element(m))
  {
    return *boost::max_element(m);
  }
  
  template <typename M, typename S>
  inline void resize(M&& m, const S& s)
  {
    m.resize(s);
  }

  template <typename V>
  inline void append_vector_to_vector(const V& src, V&& dst, const typename traits<V>::size_type& off) {
    for(auto i = src.begin(), e = src.end(); i != e; ++i) {
      dst[i.index()+off] = *i;
    }
  }

  template <typename V>
  inline void append_sparse_vector_to_sparse_vector(const V& src, V&& dst,
                                                    const typename traits<V>::size_type& off) {
    for(auto i = src.begin(), e = src.end(); i != e; ++i) {
      dst[i.index()+off] = *i;
    }
  }
  
  template <typename X, typename I, typename V>
  inline auto set_value_at_index_in_vector(const X& x, const I& i, V&& v)
  -> decltype(v[i])
  {
    return v[i] = x;
  }

  template <typename I, typename V>
  inline auto increment_value_at_index_in_vector(const I& i, V&& v)
  -> decltype(v[i])
  {
    return v[i] += 1;
  }

  template <typename T, typename M>
  inline auto matrix_cast(M&& m)
  -> decltype(m)
  {
    return m;
  }

  template <typename V, typename OP>
  inline void for_each(const V& v, OP&& op) {
    for (auto E = v.begin(), __end = v.end(); E != __end; ++E) {
      op(E.index(), *E);
    }
  }
  
  // dense vectors only!!!!
  template <typename V, typename Q>
  inline auto element_div(const V& v, const Q& q)
  -> decltype(ublas::element_div(v, q))
  {
    // ubleas::element_div causes divide by zero. I guess, it tries to compute
    // 1/|Dl| for each values v even for 0 ones....
    // return element_div(term.tfs() * lambda().value(),
    // field.documentLengths());
    return ublas::element_div(v, q);
  }

  template <typename A, typename B, typename C>
  inline void sparse_outer_prod_add_to(const A& a, const B& b, C&& c) {
    for (auto a_it = a.begin(), a_end = a.end(); a_it != a_end; ++a_it) {
      const auto a_index = a_it.index();
      const auto a_value = *a_it;
      for (auto b_it = b.begin(), b_end = b.end(); b_it != b_end; ++b_it) {
        c(a_index, b_it.index()) += a_value * (*b_it);
      }
    }
  }
  
  template <typename A, typename B, typename C>
  inline void sparse_scale_add_to(const A& a, const B& b, C&& c) {
    for (auto a_it = a.begin(), a_end = a.end(); a_it != a_end; ++a_it) {
      c(a_it.index()) += (*a_it) * b;
    }
  }

  // boost UBLAS is horrible at taking advantage of sparse vectors.
  // Every element-wise operation you apply to sparse vectors is applied
  // to every vector element including zero ones. So we have to implement
  // them ourselves.
  template <typename A, typename B, typename C>
  inline SparseWeightVector sparse_scale_div_by(const A& a, const B& b, const C& c) {
    SparseWeightVector result(a.size());
    for (auto a_it = a.begin(), a_end = a.end(); a_it != a_end; ++a_it) {
      result(a_it.index()) = (*a_it) * b / c(a_it.index());
    }
    return result;
  }

  
}

namespace boost { namespace numeric { namespace ublas {
  
  std::ostream& operator << (std::ostream& outs, const jerome::WeightMatrix& obj);
  std::ostream& operator << (std::ostream& outs, const jerome::SymmetricWeightMatrix& obj);
  std::ostream& operator << (std::ostream& outs, const jerome::SymmetricWeightMatrixRange& obj);
  std::ostream& operator << (std::ostream& outs, const jerome::WeightVector& obj);
  
  // fascinating shit: when looking for an overloaded operator, compiler looks into the
  // namespace of the object parameter. Aparently, if you have a typedef, the compiler
  // ignores the namespace of the typedef and goes directly for the namespace of the class used
  // in the typedef. in this case it's boost:numeric::ublas.
  
  // if you do not put the operator in the namespace, the compiler will not find it.
}}}

#endif // __jerome_type_matrix_boost_hpp__
