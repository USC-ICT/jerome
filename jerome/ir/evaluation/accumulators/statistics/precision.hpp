//
//  precision.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_precision_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_precision_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>

// =============================================================================
// precision

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct precision_impl
            : subtractable_accumulator_base
          {

            typedef double result_type;
            
            precision_impl(dont_care)
            {
            }

            // two cases:
            //	1. there are relevant documents. If we retrieved something,
            // return
            //		the proportion of relevant among retrieved
            //		If did not retrieve anything, return 0;
            //	2. there are no relevant documents. If we did not retrieve
            // anything,
            //		return 1; if we did -- return 0;
            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type totalCount = retrieved_count(args);
              if (totalCount == 0) {
                return relevant_size(args) == 0 ? 1 : 0;
              } else {
                return (relevant_count(args) / totalCount);
              }
            }

          };
        }
        namespace tag {
          struct precision
            : depends_on<relevant_count, retrieved_count,
              relevant_size>
          {
            typedef boost::mpl::always<accumulators::impl::precision_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::precision> const precision = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(precision)
        }

        using extract::precision;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_precision_hpp__
