//
//  micro_average.hpp
//
//  Created by Anton Leuski on 5/22/13.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_micro_average_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_micro_average_hpp__

#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// micro average of F computes F on the current accumulator set values.
// aliasing total_of<count> as count allows us to do this.

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace tag {
          template <typename Feature>
          struct micro_average_of
            : public Feature
          {
            typedef typename Feature::impl impl;
          };
        }

        namespace extract {
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, micro_average_of, (typename))
        }

        using extract::micro_average_of;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_micro_average_hpp__
