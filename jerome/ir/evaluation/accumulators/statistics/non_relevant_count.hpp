//
//  non_relevant_count.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_non_relevant_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_non_relevant_count_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>

// =============================================================================
// non_relevant_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct non_relevant_count_impl
            : subtractable_accumulator_base
          {

            typedef relevant_count_impl::result_type result_type;
            non_relevant_count_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              return retrieved_count(args) - relevant_count(args);
            }

          };
        }
        namespace tag {
          struct non_relevant_count
            : depends_on<retrieved_count, relevant_count>
          {
            typedef boost::mpl::always<accumulators::impl::
               non_relevant_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::non_relevant_count> const non_relevant_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(non_relevant_count)
        }

        using extract::non_relevant_count;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_non_relevant_count_hpp__
