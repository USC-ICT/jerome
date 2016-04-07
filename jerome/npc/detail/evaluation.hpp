//
//  evaluation.hpp
//
//  Created by Anton Leuski on 9/16/15.
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

#ifndef __jerome_npc_detail_evaluation_hpp__
#define __jerome_npc_detail_evaluation_hpp__

#include <jerome/types.hpp>
#include <jerome/npc/detail/types_fwd.hpp>

namespace jerome {
	
	class Record;

	namespace npc {
		namespace detail {
		
			OptionalError evaluate(const Record& inReporterModel,
														 std::ostream& os,
														 const Data& inTestData,
														 const Ranker& inRanker);
			
		}
	}
}
#endif // defined __jerome_npc_detail_evaluation_hpp__
