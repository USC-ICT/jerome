//
//  retrieved_count.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_retrieved_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_retrieved_count_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>

// =============================================================================
// retrieved_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct retrieved_count_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;
            
            retrieved_count_impl(dont_care)
              : value(0)
            {
            }

            void operator () (dont_care)
            {
              ++this->value;
            }

            void subtract(dont_care)
            {
              assert(this->value > 0);
              --this->value;
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
          struct retrieved_count
            : depends_on<>
          {
            typedef boost::mpl::always<accumulators::impl::retrieved_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::retrieved_count> const retrieved_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(retrieved_count)
        }

        using extract::retrieved_count;
      }
    }
  }
}

// we are telling the accumulator system that if total_of<tag::retrieved_count>
// is available, it's ok to use it in place of retrieved_count
// That way if we speccify total_of<tag::retrieved_count> on a data set
// collector accumulator, we will be able to compute micro average scores.
namespace boost {
  namespace accumulators {

    namespace jerome_ac = jerome::ir::evaluation::accumulators;

    template <>
    struct feature_of<jerome_ac::tag::total_of<jerome_ac::tag::retrieved_count>>
      : feature_of<jerome_ac::tag::retrieved_count>
    {
    };

  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_retrieved_count_hpp__
