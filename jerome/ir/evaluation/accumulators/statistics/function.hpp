//
//  function.hpp
//
//  Created by Anton Leuski on 8/11/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Jerome is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Jerome is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with Jerome.  If not, see <http://www.gnu.org/licenses/>.
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
