//
//  function.hpp
//
//  Created by Anton Leuski on 8/11/15.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_function_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_function_hpp__

#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// =============================================================================
// function
// use to run an arbitrary function at every accumulator input.

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        BOOST_PARAMETER_NESTED_KEYWORD(tag, function_value, value)

        namespace impl {
          template <typename Sample, typename F>
          struct function_impl
            : subtractable_accumulator_base
          {
            typedef void result_type;
            typedef F function_type;

            template <typename Args>
            function_impl(Args const& args)
              : val(args[function_value | function_type()])
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              val(args[sample]);
            }

            dont_care result(dont_care) const
            {
            }

          private:
            function_type val;
          };

        } // namespace impl

        namespace tag {
          template <typename F = void>
          struct function
            : depends_on<>
          {
            typedef accumulators::impl::function_impl<boost::mpl::_1, F> impl;
          };

          template <>
          struct function<void>
            : depends_on<>
          {
            typedef accumulators::impl::function_impl<boost::mpl::_1,
              std::function<void (const boost::mpl::_1&)>> impl;
          };

        }
      }
    }
  }
}

#endif // defined __jerome_ir_evaluation_accumulators_statistics_function_hpp__
