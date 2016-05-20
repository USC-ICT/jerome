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

#define JEROME_MATRIX_BOOST 1
#define JEROME_MATRIX_EIGEN 2

#ifndef JEROME_MATRIX
#define JEROME_MATRIX JEROME_MATRIX_EIGEN
//#define JEROME_MATRIX JEROME_MATRIX_BOOST
#endif

namespace jerome {
  typedef double WeightValue;
}

// While using boost::ublas as the linear algebra code is convinient,
// I observe that it is not well suited for sparse vector/matrix operations.
// It has the sparse type, however all operations over the sparse objects
// attampt to touch every element, including the omitted zero ones. Which,
// unsurprisingly, slows things down. Even when I optimize the sparse-related
// code, boost is still slower than Eigen by about 2-2.5. I belive, Eigen
// does some clever vectorization in the code to achive thise.

#if JEROME_MATRIX == JEROME_MATRIX_BOOST

#include <jerome/type/matrix_boost.hpp>

#elif JEROME_MATRIX == JEROME_MATRIX_EIGEN

#include <jerome/type/matrix_eigen.hpp>

#else

#error "Use JEROME_MATRIX to select matrix implementation"

#endif

// These are some experiments with offloading model calculations to the GPU
// It does not work extremely well: viennacl is limited in what it can do.
// Also, moving matrices to GPU and back eats a lot of cycles. Ideally,
// we need to rewrite the code for GPU...

//#define VIENNACL_WITH_OPENCL

#ifdef VIENNACL_WITH_OPENCL
#include <viennacl/matrix.hpp>
#include <viennacl/linalg/prod.hpp>
#endif

namespace jerome {
#ifdef VIENNACL_WITH_OPENCL
  typedef	viennacl::matrix<float, viennacl::column_major>	FastMatrix;
  typedef viennacl::vector<float>							FastVector;
#else
  typedef	WeightMatrix	FastMatrix;
  typedef WeightVector	FastVector;
#endif
}


#endif // defined __jerome_type_matrix_hpp__
