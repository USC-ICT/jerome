//
//  Ranker.hpp
//
//  Created by Anton Leuski on 7/31/15.
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
