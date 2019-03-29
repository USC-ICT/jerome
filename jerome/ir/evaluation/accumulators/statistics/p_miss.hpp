//
//  p_miss.hpp
//
//  Created by Anton Leuski on 5/12/13.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_p_miss_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_p_miss_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>

// =============================================================================
// pmiss

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct pmiss_impl
            : subtractable_accumulator_base
          {
						
            typedef double result_type;
            
            pmiss_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type relevantSize  = relevant_size(args);
              if (relevantSize == 0) return 0;
              result_type relevantCount = relevant_count(args);
              return ((relevantSize - relevantCount) / relevantSize);
            }

          };
        }
        namespace tag {
          struct pmiss
            : depends_on<relevant_count, relevant_size>
          {
            typedef boost::mpl::always<accumulators::impl::pmiss_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::pmiss> const pmiss = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(pmiss)
        }

        using extract::pmiss;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_p_miss_hpp__
