//
//  non_relevant_count.hpp
//
//  Created by Anton Leuski on 5/12/13.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_non_relevant_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_non_relevant_count_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>

// =============================================================================
// non_relevant_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct non_relevant_count_impl
            : subtractable_accumulator_base
          {

            typedef relevant_count_impl::result_type result_type;
            non_relevant_count_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              return retrieved_count(args) - relevant_count(args);
            }

          };
        }
        namespace tag {
          struct non_relevant_count
            : depends_on<retrieved_count, relevant_count>
          {
            typedef boost::mpl::always<accumulators::impl::
               non_relevant_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::non_relevant_count> const non_relevant_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(non_relevant_count)
        }

        using extract::non_relevant_count;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_non_relevant_count_hpp__