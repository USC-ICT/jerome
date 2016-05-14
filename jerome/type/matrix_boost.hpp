//
//  matrix_boost.hpp
//  jerome
//
//  Created by Anton Leuski on 5/13/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
#include <jerome/math/matrix_functions.hpp>

#pragma clang diagnostic pop

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
  
  typedef ublas::scalar_matrix<WeightMatrix::value_type>	WeightScalarMatrix;
  // the vector of a constant.
  typedef ublas::scalar_vector<WeightMatrix::value_type>	WeightScalarVector;
  
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
  
  inline WeightVectorScalar WeightVectorZero(traits<WeighVector>::size_type size)
  {
    std::cout << "zero " << size << std::endl;
    return WeightVectorScalar(size, 0);
  }
  
  inline WeightVectorScalar WeightVectorOnes(traits<WeighVector>::size_type size)
  {
    std::cout << "ones " << size << std::endl;
    return WeightVectorScalar(size, 1);
  }
  
#define JEROME_FOR_EACH_ELEMENT_OF_SPARSE_VECTOR(E, V, T) \
for (auto E = V.begin(), __end = V.end(); E != __end; ++E)
  
#define JEROME_SPARSE_VECTOR_ELEMENT_VALUE(E) (*E)
#define JEROME_SPARSE_VECTOR_ELEMENT_INDEX(E) (E.index())
  
#define JEROME_MATRIX_COLUMN(M, C) WeightMatrixColumn(M, C)
#define JEROME_MATRIX_CONST_COLUMN(M, C) WeightMatrixConstColumn(M, C)
#define JEROME_MATRIX_ROW(M, C) WeightMatrixRow(M, C)
#define JEROME_MATRIX_CONST_ROW(M, C) WeightMatrixConstRow(M, C)
#define JEROME_MATRIX_PROD(A, B) prod(A,B)
#define JEROME_MATRIX_OUTER_PROD(A, B) outer_prod(A, B)
  
#define JEROME_MATRIX_ELEMENT_POW(A, B) element_pow(A,B)
#define JEROME_MATRIX_ELEMENT_PROD(A, B) element_prod(A,B)
#define JEROME_MATRIX_ELEMENT_LOG_PLUS_1(A) log_plus1(A)
#define JEROME_MATRIX_MAX_ELEMENT(V)  *boost::max_element(V)
  
  
#define JEROME_MATRIX_VECTOR_RESIZE(V, S) (V).resize(S)
#define JEROME_MATRIX_INCREMENT_VECTOR_ELEMENT_AT_INDEX(V, I) (V)[I] += 1
#define JEROME_MATRIX_APPEND_VECTOR_TO_VECTOR(SRC, DST, OFF) \
for(auto i = (SRC).begin(), e = (SRC).end(); i != e; ++i) { \
(DST)[i.index()+OFF] = *i; \
}
#define JEROME_MATRIX_SET_VECTOR_ELEMENT_AT_INDEX_TO(V, I, X) (V)[I] = (X)
#define JEROME_MATRIX_CAST(M, T) (M)

}

namespace boost { namespace numeric { namespace ublas {
  
  std::ostream& operator << (std::ostream& outs, const jerome::WeightMatrix& obj);
  //	std::ostream& operator << (std::ostream& outs, const jerome::SymmetricWeightMatrix& obj);
  //	std::ostream& operator << (std::ostream& outs, const jerome::SymmetricWeightMatrixRange& obj);
  std::ostream& operator << (std::ostream& outs, const jerome::WeightVector& obj);
  
  // fascinating shit: when looking for an overloaded operator, compiler looks into the
  // namespace of the object parameter. Aparently, if you have a typedef, the compiler
  // ignores the namespace of the typedef and goes directly for the namespace of the class used
  // in the typedef. in this case it's boost:numeric::ublas.
  
  // if you do not put the operator in the namespace, the compiler will not find it.
}}}

#endif // __jerome_type_matrix_boost_hpp__
