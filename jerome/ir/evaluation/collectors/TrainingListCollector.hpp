//
//  TrainingListCollector.hpp
//
//  Created by Anton Leuski on 8/10/15.
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

#ifndef __jerome_ir_evaluation_collectors_TrainingListCollector_hpp__
#define __jerome_ir_evaluation_collectors_TrainingListCollector_hpp__

#include <vector>
#include <jerome/ir/evaluation/collectors/collector.hpp>
#include <jerome/math/scored.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {

      template <class Collector>
      class TrainingListCollector
      {
        typedef Collector                               collector_type;

        typedef typename collector_type::result_type    object_type;
        typedef jerome::math::ScoredObject<object_type>   element_type;

// TODO DataSetEvaluator is going to make a copy of the result -- the ranked
// list. Consider making the type to be a shared_ptr or a reference collection.

        typedef std::vector<element_type>               list_type;

      public:
        typedef list_type                               result_type;

        template <typename ... Args>
        explicit TrainingListCollector(Args&& ... args)
          : mCollector(std::forward<Args>(args) ...)
        {
          mList.push_back(element_type(0, mCollector.value()));
        }

        template <typename Arg1, typename ... Args>
        void operator () (Arg1&& arg1, Args&& ... args)
        {
          mCollector(std::forward<Arg1>(arg1), std::forward<Args>(args) ...);
          mList.push_back(element_type(arg1.score(), mCollector.value()));
        }

        result_type value() const
        {
          return mList;
        }

      private:
        collector_type  mCollector;
        list_type mList;

      };

    }
  }
}
#endif // defined __jerome_ir_evaluation_collectors_TrainingListCollector_hpp__
