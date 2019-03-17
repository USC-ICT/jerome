//
//  recall.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_recall_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_recall_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>

// =============================================================================
// recall

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct recall_impl
            : subtractable_accumulator_base
          {
            typedef double result_type;
            
            recall_impl(dont_care)
            {
            }

            // two cases:
            //	1. there are relevant documents. If we retrieved something,
            // return
            //		the proportion of all relevant retrieved;
            //		If did not retrieve anything, return 0;
            //	2. there are no relevant documents. If we did not retrieve
            // anything,
            //		return 1; if we did -- return 0;
            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type relSize = relevant_size(args);
              if (relSize == 0) {
                return (retrieved_count(args) == 0) ? 1 : 0;
              } else {
                return (relevant_count(args) / relSize);
              }
            }

          };
        }

        namespace tag {
          struct recall
            : depends_on<relevant_count, relevant_size,
              retrieved_count>
          {
            typedef boost::mpl::always<accumulators::impl::recall_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::recall> const recall = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(recall)
        }

        using extract::recall;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_recall_hpp__
