//
//  average_precision.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_average_precision_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_average_precision_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/is_relevant.hpp>

// =============================================================================
// average_precision

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct average_precision_impl
            : subtractable_accumulator_base
          {

            typedef double result_type;

            average_precision_impl(dont_care)
              : value(0)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (is_relevant(args))
                this->value += (result_type)relevant_count(args) /
                               (result_type)retrieved_count(args);
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (retrieved_count(args) == 0) {
                this->value = 0;
              } else {
                if (is_relevant(args))
                  this->value -= (result_type)(relevant_count(args) + 1) /
                                 (result_type)(retrieved_count(args) + 1);
              }
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              auto rs = relevant_size(args);
              auto rc = retrieved_count(args);
              if (rs == 0 && rc == 0) return 1;
              
              result_type denom = relevant_count(args);
              result_type average_precision = denom == 0
                ? 0 : (this->value / denom);
              
              assert(average_precision >= 0 && average_precision <= 1);
              return average_precision;
            }

          private:
            result_type value;
          };
        }

        namespace tag {
          struct average_precision
            : depends_on<relevant_count, retrieved_count,
              is_relevant, relevant_size>
          {
            typedef boost::mpl::always<accumulators::impl::
               average_precision_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::average_precision> const
          average_precision = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(average_precision)
        }

        using extract::average_precision;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_average_precision_hpp__
