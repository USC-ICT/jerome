//
//  Ranker.hpp
//
//  Created by Anton Leuski on 7/31/15.
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

#ifndef __jerome_npc_detail_Ranker_hpp__
#define __jerome_npc_detail_Ranker_hpp__

#include <jerome/npc/model.hpp>
#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/npc/detail/Trainer.hpp>
#include <jerome/ir/ranker.hpp>
#include <jerome/ir/training/TestSet.hpp>

namespace jerome {
  namespace npc {
    namespace detail {

      template <class Q, class A>
      class RankerInterface
        : public jerome::ir::Ranker<Q, A, RankerImplementation<Q, A>>
      {
      public:
        typedef jerome::ir::Ranker<Q, A, RankerImplementation<Q, A>> parent_type;
        using parent_type::parent_type;

        typedef RankerInterface<Q, A> this_type;

        JEROME_INTERNAL_RANKER_TYPES(parent_type)
        typedef TestSet<query_type, document_type>  test_set_type;

        void train(Trainer& inTrainer,
                   const Trainer::progress_callback_type& callback)
        {
          this->implementation().train(inTrainer, callback);
        }

        test_set_type testSetWithData(const Data& data) const 
        {
          return this->implementation().testSetWithData(data);
        }

        ir::value_vector values() const
        {
          return parent_type::values();
        }

        State state() const
        {
          return this->implementation().state();
        }
				
				Record model() const
				{
					return this->implementation().model();
				}

      };

      Data dataFromState(const State& state,
                         const Collection::utterance_index_type& utteranceIndex);
    }
  }
}

#endif // defined __jerome_npc_detail_Ranker_hpp__
