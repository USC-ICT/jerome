//
//  is_relevant.hpp
//
//  Created by Anton Leuski on 8/8/15.
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
