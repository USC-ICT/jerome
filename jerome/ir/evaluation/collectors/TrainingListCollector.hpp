//
//  TrainingListCollector.hpp
//
//  Created by Anton Leuski on 8/10/15.
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
