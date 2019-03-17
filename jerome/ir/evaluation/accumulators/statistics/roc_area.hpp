//
//  roc_area.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_roc_area_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_roc_area_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>

// =============================================================================
// roc_area

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct roc_area_impl
            : subtractable_accumulator_base
          {
						
            typedef double result_type;

            roc_area_impl(dont_care)
              : value(0)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (!is_relevant(args))
                this->value += relevant_count(args);
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (!is_relevant(args)) {
                double rc = relevant_count(args);
                assert(this->value >= rc);
                this->value -= rc;
              }
            }

            // TODO validate that this is correct
            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type relc  = relevant_count(args);
              if (relc == 0) return 0;
              result_type nonc  = retrieved_count(args) - relc;
              if (nonc == 0) return 1;
              return this->value / (relc * nonc);
            }

          private:
            typedef relevant_count_impl::result_type  value_type;
            value_type value;
          };
        }
        namespace tag {
          struct roc_area
            : depends_on<relevant_count, retrieved_count, relevant>
          {
            typedef boost::mpl::always<accumulators::impl::roc_area_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::roc_area> const roc_area = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(roc_area)
        }

        using extract::roc_area;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_roc_area_hpp__
