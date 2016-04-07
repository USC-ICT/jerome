//
//  matrix.cpp
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

#include <jerome/type/matrix.hpp>

namespace boost { namespace numeric { namespace ublas {
	
	std::ostream& operator << (std::ostream& outs, const jerome::WeightMatrix& obj) {
		for(std::size_t i = 0; i < obj.size1(); ++i) {
			for(std::size_t j = 0; j < obj.size2(); ++j) {
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
		for(auto & v : obj) {
			if (f) outs << ", ";
			f = true;
			outs << v;
		}
		outs << "}";
		return outs;
	}
	
}}}
