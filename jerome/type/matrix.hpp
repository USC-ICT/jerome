//
//  matrix.hpp
//
//  Created by Anton Leuski on 7/22/14.
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

#ifndef __jerome_type_matrix_hpp__
#define __jerome_type_matrix_hpp__


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

//#define VIENNACL_WITH_OPENCL

#ifdef VIENNACL_WITH_OPENCL
#include <viennacl/matrix.hpp>
#include <viennacl/linalg/prod.hpp>
#endif

namespace jerome {

	template<class T> using Vector = boost::numeric::ublas::vector<T>;
	// sparse array
	template<class T> using SparseVector = boost::numeric::ublas::compressed_vector<T>;

	template<class T> using ScalarVector = boost::numeric::ublas::vector<T>;
	
	typedef boost::numeric::ublas::range												Range;
	typedef boost::numeric::ublas::matrix<float, boost::numeric::ublas::column_major>	WeightMatrix;
	typedef boost::numeric::ublas::matrix_range<WeightMatrix>							WeightMatrixRange;
	typedef boost::numeric::ublas::matrix_row<WeightMatrix>								WeightMatrixRow;
	typedef boost::numeric::ublas::matrix_row<const WeightMatrix>						WeightMatrixConstRow;
	typedef boost::numeric::ublas::matrix_column<WeightMatrix>							WeightMatrixColumn;
	typedef boost::numeric::ublas::matrix_column<const WeightMatrix>					WeightMatrixConstColumn;
	typedef boost::numeric::ublas::vector<WeightMatrix::value_type>						WeightVector;
	typedef boost::numeric::ublas::compressed_vector<WeightMatrix::value_type>			SparseWeightVector;
	typedef boost::numeric::ublas::symmetric_matrix<WeightMatrix::value_type,
	boost::numeric::ublas::upper>			SymmetricWeightMatrix;
	typedef boost::numeric::ublas::matrix_range<SymmetricWeightMatrix>					SymmetricWeightMatrixRange;
	typedef std::pair<WeightMatrix::size_type, WeightMatrix::size_type>					MatrixSize;

	
#ifdef VIENNACL_WITH_OPENCL
	typedef	viennacl::matrix<float, viennacl::column_major>	FastMatrix;
	typedef viennacl::vector<float>							FastVector;
#else
	typedef	WeightMatrix	FastMatrix;
	typedef WeightVector	FastVector;
#endif
	
	typedef boost::numeric::ublas::scalar_matrix<WeightMatrix::value_type>	WeightScalarMatrix;
	// the vector of a constant.
	typedef boost::numeric::ublas::scalar_vector<WeightMatrix::value_type>	WeightScalarVector;
	
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


#endif // defined __jerome_type_matrix_hpp__
