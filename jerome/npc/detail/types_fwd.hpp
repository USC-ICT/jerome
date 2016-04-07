//
//  types_fwd.hpp
//
//  Created by Anton Leuski on 8/1/15.
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

#ifndef __jerome_npc_detail_types_fwd_hpp__
#define __jerome_npc_detail_types_fwd_hpp__

namespace jerome {

	template <typename> class Providable;

	
	namespace ir {

		class HeapIndex;
		template <class Q, class A, class L> class Ranker;
		
		namespace training {
			template <class Q, class A, class L> class Data;
			template <class Q, class D> class TestSet;
		}
		
	}
	
	namespace npc {
	
		class Utterance;
		class Link;
		
		namespace detail {
	
			typedef jerome::ir::training::Data <
				Utterance,
				Utterance,
				Link
			> Data;
		
			typedef jerome::ir::training::Data <
				jerome::ir::HeapIndex,
				Utterance,
				Link
			>	IndexedData;
		
			template <class Q, class A> class RankerImplementation;
			template <class Q, class A> class RankerInterface;
			
			typedef RankerInterface<
				Utterance,
				Utterance
			> Ranker;
			
			typedef RankerInterface<
				jerome::ir::HeapIndex,
				Utterance
			> IndexedRanker;

			template <class Q, class A>
			using TestSet = jerome::ir::training::TestSet<Q, A>;
			
		}
	}
}

#endif // defined __jerome_npc_detail_types_fwd_hpp__
