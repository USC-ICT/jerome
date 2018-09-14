//
//  matrix_eigen.hpp
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

#ifndef __jerome_type_matrix_eigen_hpp__
#define __jerome_type_matrix_eigen_hpp__

#include <iostream>

#ifdef EIGEN_SPARSEVECTOR_PLUGIN
#define __jerome_had_no_EIGEN_SPARSEVECTOR_PLUGIN__ 0
#else
#define __jerome_had_no_EIGEN_SPARSEVECTOR_PLUGIN__ 1
#define EIGEN_SPARSEVECTOR_PLUGIN "jerome/type/matrix_eigen_sparse_vector_plugin.hpp"
#endif

#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/Sparse>

#if __jerome_had_no_EIGEN_SPARSEVECTOR_PLUGIN__
#undef EIGEN_SPARSEVECTOR_PLUGIN
#endif

namespace jerome {
  
  template<class T> using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
  template<class T> using VectorSham = Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, 1>>;
  // sparse array
  template<class T> using SparseVector = Eigen::SparseVector<T, 0, std::ptrdiff_t>;
  
  typedef Eigen::Matrix<WeightValue, Eigen::Dynamic, Eigen::Dynamic>	WeightMatrix;
  typedef Eigen::Matrix<WeightValue, Eigen::Dynamic, 1>               WeightVector;
  typedef Eigen::SparseVector<WeightValue, 0, std::ptrdiff_t>         SparseWeightVector;
  
  template <class M>
  struct traits {
    typedef typename M::Index size_type;
    typedef typename M::Scalar value_type;
  };
  
  struct MatrixSize {
    traits<WeightMatrix>::size_type rowCount;
    traits<WeightMatrix>::size_type columnCount;
    
    MatrixSize() = default;
    
    MatrixSize(const WeightMatrix& m)
    : rowCount(m.rows()), columnCount(m.cols())
    {}
  };

  template <typename T>
  inline VectorSham<T> sham(const std::vector<T>& inVector)
  {
    return VectorSham<T>(const_cast<T*>(inVector.data()), inVector.size());
  }

  inline auto WeightMatrixZero(const MatrixSize& size)
  -> decltype(WeightMatrix::Zero(size.rowCount, size.columnCount))
  {
    return WeightMatrix::Zero(size.rowCount, size.columnCount);
  }
  
  inline auto WeightMatrixOnes(const MatrixSize& size)
  -> decltype(WeightMatrix::Ones(size.rowCount, size.columnCount))
  {
    return WeightMatrix::Ones(size.rowCount, size.columnCount);
  }
  
  inline auto WeightVectorZero(traits<WeightVector>::size_type size)
  -> decltype(WeightVector::Zero(size))
  {
    return WeightVector::Zero(size);
  }
  
  inline auto WeightVectorOnes(traits<WeightVector>::size_type size)
  -> decltype(WeightVector::Ones(size))
  {
    return WeightVector::Ones(size);
  }

  template <typename M, typename C>
  inline auto column(M&& m, C c)
  -> decltype(m.col(c))
  {
    return m.col(c);
  }
  
  template <typename M, typename C>
  inline auto row(M&& m, C c)
  -> decltype(m.row(c))
  {
    return m.row(c);
  }
  
  template <typename A, typename B>
  inline auto prod(const A& a, const B& b)
  -> decltype(a * b)
  {
    return a * b;
  }

  template <typename A, typename B>
  inline auto outer_prod(const A& a, const B& b)
  -> decltype(a * b.transpose())
  {
    return a * b.transpose();
  }
  
  template<typename Scalar> struct MakeLogPlusOne {
    EIGEN_EMPTY_STRUCT_CTOR(MakeLogPlusOne)
    typedef Scalar result_type;
    result_type operator()(const Scalar& a) const { return std::log(a+1.0); }
  };
  
  template <typename M>
  inline auto sparse_log_plus1(const M& m)
  -> decltype(m.unaryExpr(MakeLogPlusOne<typename traits<M>::value_type>()))
  {
    return m.unaryExpr(MakeLogPlusOne<typename traits<M>::value_type>());
  }
    
  template<typename Scalar> struct MakeElementPow {
    EIGEN_EMPTY_STRUCT_CTOR(MakeElementPow)
    double mE;
    MakeElementPow(double e) : mE(e) {}
    typedef Scalar result_type;
    result_type operator()(const Scalar& a) const { return std::pow(a,mE); }
  };
  
  template <typename A, typename B>
  inline auto element_pow(const A& a, const B& b)
  -> decltype(a.unaryExpr(MakeElementPow<typename traits<A>::value_type>(b)))
  {
    return a.unaryExpr(MakeElementPow<typename traits<A>::value_type>(b));
  }

  template <typename A, typename B>
  inline auto element_prod(const A& a, const B& b)
  -> decltype(a.cwiseProduct(b))
  {
    return a.cwiseProduct(b);
  }
 
  template <typename A, typename B>
  inline auto element_div(const A& a, const B& b)
  -> decltype(a.cwiseProduct(b.cwiseInverse()))
  {
    // if you do not use "prod" here, it will not take advantage of
    // sparse vector nature and generate a dense result. Bad.
    return a.cwiseProduct(b.cwiseInverse());
  }

  template <typename M>
  inline auto sum(const M& m)
  -> decltype(m.sum())
  {
    return m.sum();
  }

  template <typename M>
  inline auto max_element(const M& m)
  -> decltype(m.maxCoeff())
  {
    return m.maxCoeff();
  }
    
  template <typename M, typename S>
  inline void resize(M& m, S s)
  {
    m.conservativeResize(s);
  }

  template <typename V>
  inline void append_vector_to_vector(const V& src, V& dst,
                                      const typename traits<V>::size_type& off)
  {
    for(auto i = src.begin(), e = src.end(); i != e; ++i) {
      dst.coeffRef(i.index()+off) = *i;
    }
  }

  template <typename V>
  inline void append_sparse_vector_to_sparse_vector(const V& src, V& dst,
                                                    const typename traits<V>::size_type& off)
  {
    for (typename V::InnerIterator E(src); E; ++E) {
      dst.coeffRef(E.index()+off) = E.value();
    }
  }

  template <typename X, typename I, typename V>
  inline auto set_value_at_index_in_vector(X&& x, const I& i, V& v)
  -> decltype(v.coeffRef(i))
  {
    return v.coeffRef(i) = x;
  }

  template <typename I, typename V>
  inline auto increment_value_at_index_in_vector(const I& i, V& v)
  -> decltype(v.coeffRef(i))
  {
    return v.coeffRef(i) += 1;
  }

  template <typename T, typename M>
  inline auto matrix_cast(M&& m)
  -> decltype(m.template cast<T>())
  {
    return m.template cast<T>();
  }

  template <typename V, typename OP>
  inline void for_each(const V& v, OP&& op) {
    for (typename V::InnerIterator E(v); E; ++E) {
      op(E.index(), E.value());
    }
  }
  
  template <typename A, typename B, typename C>
  inline void sparse_outer_prod_add_to(const A& a, const B& b, C& c) {
    for (typename A::InnerIterator a_it(a); a_it; ++a_it) {
      const auto a_index = a_it.index();
      const auto a_value = a_it.value();
      for (typename B::InnerIterator b_it(b); b_it; ++b_it) {
        c(a_index, b_it.index()) += a_value * b_it.value();
      }
    }
  }

  template <typename A, typename B, typename C>
  inline void sparse_scale_add_to(const A& a, const B& b, C& c) {
    for (typename A::InnerIterator a_it(a); a_it; ++a_it) {
      c(a_it.index()) += a_it.value() * b;
    }
  }

  template <typename A, typename B, typename C>
  inline auto sparse_scale_div_by(const A& a, const B& b, const C& c)
  -> decltype((a*b).cwiseProduct(c.cwiseInverse()))
  {
    // if you do not use "prod" here, it will not take advantage of
    // sparse vector nature and generate a dense result. Bad.
    return (a*b).cwiseProduct(c.cwiseInverse());
  }

}

#endif // __jerome_type_matrix_eigen_hpp__
