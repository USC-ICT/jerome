//
//  sample_count.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_sample_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_sample_count_hpp__

#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// =============================================================================
// sample_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct sample_count_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;
            
            sample_count_impl(dont_care)
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
          struct sample_count
            : depends_on<>
          {
            typedef boost::mpl::always<accumulators::impl::sample_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::sample_count> const sample_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(sample_count)
        }

        using extract::sample_count;
      }
    }
  }
}


#endif // __jerome_ir_evaluation_accumulators_statistics_sample_count_hpp__
