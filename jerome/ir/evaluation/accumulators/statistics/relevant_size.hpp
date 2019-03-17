//
//  relevant_size.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_relevant_size_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_relevant_size_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_set.hpp>

// =============================================================================
// relevant_size

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct relevant_size_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;
            
            relevant_size_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              return relevant_set(args).size;
            }

          };
        }
        namespace tag {
          struct relevant_size
            : depends_on<relevant_set>
          {
            typedef boost::mpl::always<accumulators::impl::relevant_size_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::relevant_size> const relevant_size = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(relevant_size)
        }

        using extract::relevant_size;
      }
    }
  }
}

// we are telling the accumulator system that if total_of<tag::relevant_size>
// is available, it's ok to use it in place of relevant_size
// That way if we speccify total_of<tag::relevant_size> on a data set
// collector accumulator, we will be able to compute micro average scores.
namespace boost {
  namespace accumulators {

    namespace jerome_ac = jerome::ir::evaluation::accumulators;

    template <>
    struct feature_of<jerome_ac::tag::total_of<jerome_ac::tag::relevant_size>>
      : feature_of<jerome_ac::tag::relevant_size>
    {
    };

  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_relevant_size_hpp__
