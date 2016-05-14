//
//  matrix_eigen.hpp
//  jerome
//
//  Created by Anton Leuski on 5/13/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#ifndef __jerome_type_matrix_eigen_hpp__
#define __jerome_type_matrix_eigen_hpp__

#include <iostream>

#define EIGEN_SPARSEVECTOR_PLUGIN "EigenSparseVectorPlugin.hpp"

#include <Eigen/Dense>
#include <Eigen/Sparse>

#undef EIGEN_SPARSEVECTOR_PLUGIN

namespace jerome {
  
  template<class T> using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
  // sparse array
  template<class T> using SparseVector = Eigen::SparseVector<T>;
  
  typedef Eigen::Matrix<WeightValue, Eigen::Dynamic, Eigen::Dynamic>	WeightMatrix;
  typedef Eigen::Matrix<WeightValue, Eigen::Dynamic, 1>               WeightVector;
  typedef Eigen::SparseVector<WeightValue>                            SparseWeightVector;
  
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
  
  typedef WeightMatrix::ConstantReturnType WeightMatrixScalar;
  typedef WeightVector::ConstantReturnType WeightVectorScalar;
  
  inline WeightMatrixScalar WeightMatrixZero(const MatrixSize& size)
  {
    std::cout << "zero " << size.rowCount << "," << size.columnCount << std::endl;
    return WeightMatrix::Zero(size.rowCount, size.columnCount);
  }
  
  inline WeightMatrixScalar WeightMatrixOnes(const MatrixSize& size)
  {
    std::cout << "ones " << size.rowCount << "," << size.columnCount << std::endl;
    return WeightMatrix::Ones(size.rowCount, size.columnCount);
  }
  
  inline WeightVectorScalar WeightVectorZero(traits<WeightVector>::size_type size)
  {
    std::cout << "zero " << size << std::endl;
    return WeightVector::Zero(size);
  }
  
  inline WeightVectorScalar WeightVectorOnes(traits<WeightVector>::size_type size)
  {
    std::cout << "ones " << size << std::endl;
    return WeightVector::Ones(size);
  }
  
#define JEROME_FOR_EACH_ELEMENT_OF_SPARSE_VECTOR(E, V, T) \
for (T::InnerIterator E(V); E; ++E)
  
#define JEROME_SPARSE_VECTOR_ELEMENT_VALUE(E) (E.value())
#define JEROME_SPARSE_VECTOR_ELEMENT_INDEX(E) (E.index())
  
  template <typename M, typename C>
  inline auto column(M m, C c) -> decltype(m.col(c))
  {
    return m.col(c);
  }
  
#define JEROME_MATRIX_COLUMN(M, C) M.col(C)
#define JEROME_MATRIX_ROW(M, C) M.row(C)
#define JEROME_MATRIX_CONST_COLUMN(M, C) M.col(C)
#define JEROME_MATRIX_CONST_ROW(M, C) M.row(C)
#define JEROME_MATRIX_PROD(A, B) (A) * (B)
#define JEROME_MATRIX_OUTER_PROD(A, B) (A) * (B).transpose()
  
  template<typename Scalar> struct MakeLogPlusOne {
    EIGEN_EMPTY_STRUCT_CTOR(MakeLogPlusOne)
    typedef Scalar result_type;
    result_type operator()(const Scalar& a) const { return std::log(a+1.0); }
  };
  
#define JEROME_MATRIX_ELEMENT_LOG_PLUS_1(A) (A).unaryExpr(MakeLogPlusOne<WeightValue>())
  
  template<typename Scalar> struct MakeElementPow {
    EIGEN_EMPTY_STRUCT_CTOR(MakeElementPow)
    double mE;
    MakeElementPow(double e) : mE(e) {}
    typedef Scalar result_type;
    result_type operator()(const Scalar& a) const { return std::pow(a,mE); }
  };
  
#define JEROME_MATRIX_ELEMENT_SUM(A) (A).sum()
  
#define JEROME_MATRIX_ELEMENT_POW(A, B) (A).unaryExpr(MakeElementPow<WeightValue>(B))
#define JEROME_MATRIX_ELEMENT_PROD(A, B) (A).cwiseProduct(B)
#define JEROME_MATRIX_MAX_ELEMENT(V)  (V).maxCoeff()
  
#define JEROME_MATRIX_VECTOR_RESIZE(V, S) (V).conservativeResize(S)
#define JEROME_MATRIX_INCREMENT_VECTOR_ELEMENT_AT_INDEX(V, I) (V).coeffRef(I) += 1
#define JEROME_MATRIX_APPEND_VECTOR_TO_VECTOR(SRC, DST, OFF) \
for(auto i = (SRC).begin(), e = (SRC).end(); i != e; ++i) { \
(DST).coeffRef(i.index()+OFF) = *i; \
}
#define JEROME_MATRIX_SET_VECTOR_ELEMENT_AT_INDEX_TO(V, I, X) (V).coeffRef(I) = (X)
#define JEROME_MATRIX_CAST(M, T) (M).template cast<T>()

}

#endif // __jerome_type_matrix_eigen_hpp__
