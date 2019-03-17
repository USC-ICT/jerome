//
//  is_relevant.hpp
//
//  Created by Anton Leuski on 8/8/15.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_is_relevant_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_is_relevant_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_set.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct is_relevant_impl
            : subtractable_accumulator_base
          {
						
            typedef bool result_type;

            is_relevant_impl(dont_care)
              : value(false)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              this->value =
                relevant_set(args).contains(args[sample]);
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              this->value =
                relevant_set(args).contains(args[sample]);
            }

            result_type result(dont_care) const
            {
              return this->value;
            }

          private:
            result_type value;
          };
        }
        namespace tag {
          struct is_relevant
            : depends_on<relevant_set>
          {
            typedef boost::mpl::always<accumulators::impl::is_relevant_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::is_relevant> const is_relevant = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(is_relevant)
        }

        using extract::is_relevant;
      }
    }
  }
}

#endif // defined __jerome_ir_evaluation_accumulators_statistics_is_relevant_hpp__
