//
//  total_size.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_total_size_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_total_size_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>

// =============================================================================
// total_size

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        BOOST_PARAMETER_NESTED_KEYWORD(tag, total_set_size, value)

        namespace impl {
          struct total_size_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;

            template <typename Args>
            total_size_impl(Args const& args)
              : val(args[total_set_size | std::size_t(0)])
            {
            }

            result_type result(dont_care) const
            {
              return this->val;
            }

          private:
            result_type val;
          };

        } // namespace impl

        namespace tag {
          struct total_size
            : depends_on<>
            , total_set_size
          {
            typedef boost::mpl::always<accumulators::impl::total_size_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::total_size> const total_size = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(total_size)
        }

        using extract::total_size;

      }
    }
  }
}

namespace boost {
  namespace accumulators {
    namespace jerome_ac = jerome::ir::evaluation::accumulators;
    template <>
    struct feature_of<jerome_ac::tag::total_of<jerome_ac::tag::total_size>>
      : feature_of<jerome_ac::tag::total_size>
    {
    };

  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_total_size_hpp__
