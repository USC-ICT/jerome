//
//  p_false_alarm.hpp
//
//  Created by Anton Leuski on 5/12/13.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_p_false_alarm_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_p_false_alarm_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/total_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_set.hpp>

// =============================================================================
// pfalsealarm

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct pfalsealarm_impl
            : subtractable_accumulator_base
          {

            typedef double result_type;
            
            pfalsealarm_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type relevantSize  = relevant_size(args);
              result_type totalSize   = total_size(args);
              result_type denom     = totalSize - relevantSize;
              if (denom == 0) return 0;
              result_type relevantCount = relevant_count(args);
              result_type totalCount    = retrieved_count(args);
              return ((totalCount - relevantCount) / denom);
            }

          };
        }
        namespace tag {
          struct pfalsealarm
            : depends_on<relevant_count, retrieved_count, relevant_set, total_size>
          {
            typedef boost::mpl::always<accumulators::impl::pfalsealarm_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::pfalsealarm> const pfalsealarm = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(pfalsealarm)
        }

        using extract::pfalsealarm;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_p_false_alarm_hpp__
