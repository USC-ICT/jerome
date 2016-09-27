//
//  p_false_alarm.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_p_false_alarm_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_p_false_alarm_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/total_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_set.hpp>

// =============================================================================
// pfalsealarm

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct pfalsealarm_impl
            : subtractable_accumulator_base
          {

            typedef double result_type;
            
            pfalsealarm_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type relevantSize  = relevant_size(args);
              result_type totalSize   = total_size(args);
              result_type denom     = totalSize - relevantSize;
              if (denom == 0) return 0;
              result_type relevantCount = relevant_count(args);
              result_type totalCount    = retrieved_count(args);
              return ((totalCount - relevantCount) / denom);
            }

          };
        }
        namespace tag {
          struct pfalsealarm
            : depends_on<relevant_count, retrieved_count, relevant_set, total_size>
          {
            typedef boost::mpl::always<accumulators::impl::pfalsealarm_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::pfalsealarm> const pfalsealarm = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(pfalsealarm)
        }

        using extract::pfalsealarm;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_p_false_alarm_hpp__
