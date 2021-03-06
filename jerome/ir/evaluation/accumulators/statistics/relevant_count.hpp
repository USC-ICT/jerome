//
//  relevant_count.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_relevant_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_relevant_count_hpp__

#include <assert.h>
#include <jerome/ir/evaluation/accumulators/statistics/is_relevant.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>

// =============================================================================
// relevant_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct relevant_count_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;

            relevant_count_impl(dont_care)
              : value(0)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (is_relevant(args))
                ++this->value;
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (is_relevant(args)) {
                assert(this->value > 0);
                --this->value;
              }
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
          struct relevant_count
            : depends_on<is_relevant>
          {
            typedef boost::mpl::always<accumulators::impl::relevant_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::relevant_count> const
          relevant_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(relevant_count)
        }

        using extract::relevant_count;

      }
    }
  }
}

namespace boost {
  namespace accumulators {
    namespace jerome_ac = jerome::ir::evaluation::accumulators;
    template <>
    struct feature_of<jerome_ac::tag::total_of<jerome_ac::tag::relevant_count>>
      : feature_of<jerome_ac::tag::relevant_count>
    {
    };

  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_relevant_count_hpp__
