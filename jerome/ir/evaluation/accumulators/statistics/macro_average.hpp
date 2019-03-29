//
//  macro_average.hpp
//
//  Created by Anton Leuski on 5/22/13.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_macro_average_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_macro_average_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/average.hpp>

// macro average of F computes an average of F values. We alias
// it directly to average implementation with a different name

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace tag {
          template <typename Feature>
          struct macro_average_of
            : public average_of<Feature>
          {};
        }

        namespace extract {
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, macro_average_of, (typename))
        }

        using extract::macro_average_of;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_macro_average_hpp__
