//
//  distances.hpp
//  alib
//
//  Created by Anton Leuski on 7/28/14.
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

#ifndef __alib_ir_distances_hpp__
#define __alib_ir_distances_hpp__

namespace alib { namespace ir {

	struct DistanceComputationDelegate : public ProgressDelegate {
		virtual ~DistanceComputationDelegate() {}
		virtual WeightMatrix::value_type& operator ()(std::size_t i, std::size_t j) = 0;
		virtual void setObjectCount(std::size_t count) {}
	};

}}

#endif // defined __alib_ir_distances_hpp__
