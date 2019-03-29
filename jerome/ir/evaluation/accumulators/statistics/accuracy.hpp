//
//  accuracy.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_accuracy_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_accuracy_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/is_relevant.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>

// =============================================================================
// accuracy

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct accuracy_impl
            : subtractable_accumulator_base
          {						
            typedef short result_type;

            accuracy_impl(dont_care)
              : value(0)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (retrieved_count(args) == 1) {
                this->value = (relevant_size(args) > 0 && is_relevant(args))
                  ? 1 : 0;
              }
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (retrieved_count(args) == 0) {
                this->value = 0;
              }
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              if (retrieved_count(args) == 0 && relevant_size(args) == 0)
                return 1;
              
              assert(this->value == 0 || this->value == 1);
              return this->value;
            }

          private:
            result_type value;
          };
        }
        namespace tag {
          struct accuracy
            : depends_on<retrieved_count, is_relevant, relevant_size>
          {
            typedef boost::mpl::always<accumulators::impl::accuracy_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::accuracy> const accuracy = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(accuracy)
        }

        using extract::accuracy;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_accuracy_hpp__
