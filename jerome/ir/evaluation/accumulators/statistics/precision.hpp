//
//  precision.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_precision_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_precision_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>

// =============================================================================
// precision

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct precision_impl
            : subtractable_accumulator_base
          {

            typedef double result_type;
            
            precision_impl(dont_care)
            {
            }

            // two cases:
            //	1. there are relevant documents. If we retrieved something,
            // return
            //		the proportion of relevant among retrieved
            //		If did not retrieve anything, return 0;
            //	2. there are no relevant documents. If we did not retrieve
            // anything,
            //		return 1; if we did -- return 0;
            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type totalCount = retrieved_count(args);
              if (totalCount == 0) {
                return relevant_size(args) == 0 ? 1 : 0;
              } else {
                return (relevant_count(args) / totalCount);
              }
            }

          };
        }
        namespace tag {
          struct precision
            : depends_on<relevant_count, retrieved_count,
              relevant_size>
          {
            typedef boost::mpl::always<accumulators::impl::precision_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::precision> const precision = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(precision)
        }

        using extract::precision;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_precision_hpp__
