//
//  matrix.cpp
//
//  Created by Anton Leuski on 7/22/14.
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

#include <jerome/type/matrix.hpp>

#ifdef __jerome_type_matrix_boost_hpp__

namespace boost { namespace numeric { namespace ublas {
	
	std::ostream& operator << (std::ostream& outs, const jerome::WeightMatrix& obj) {
    jerome::MatrixSize size(obj);
		for(std::size_t i = 0; i < size.rowCount; ++i) {
			for(std::size_t j = 0; j < size.columnCount; ++j) {
				outs << (obj(i,j)) << " ";
			}
			outs << std::endl;
		}
		return outs;
	}
	
	std::ostream& operator << (std::ostream& outs, const jerome::SymmetricWeightMatrix& obj) {
		for(std::size_t i = 0; i < obj.size1(); ++i) {
			for(std::size_t j = 0; j < obj.size2(); ++j) {
				outs << (obj(i,j)) << " ";
			}
			outs << std::endl;
		}
		return outs;
	}
	
	std::ostream& operator << (std::ostream& outs, const jerome::SymmetricWeightMatrixRange& obj) {
		for(std::size_t i = 0; i < obj.size1(); ++i) {
			for(std::size_t j = 0; j < obj.size2(); ++j) {
				outs << (obj(i,j)) << " ";
			}
			outs << std::endl;
		}
		return outs;
	}
	
	std::ostream& operator << (std::ostream& outs, const jerome::WeightVector& obj) {
		outs << "{";
		bool	f = false;
    for(std::size_t i = 0; i < obj.size(); ++i) {
			if (f) outs << ", ";
			f = true;
			outs << obj[i];
		}
		outs << "}";
		return outs;
	}
	
}}}

#endif

