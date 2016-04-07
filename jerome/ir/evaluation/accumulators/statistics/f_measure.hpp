//
//  f_measure.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_f_measure_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_f_measure_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/precision.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/recall.hpp>

// =============================================================================
// fmeasure

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct fmeasure_impl
            : subtractable_accumulator_base
          {
						
            typedef double result_type;

            fmeasure_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type p     = precision(args);
              result_type r     = recall(args);
              result_type p_r   = p + r;
              return (p_r == 0) ? 0 : 2 * p * r / p_r;
            }

          };
        }
        namespace tag {
          struct fmeasure
            : depends_on<precision, recall>
          {
            typedef boost::mpl::always<accumulators::impl::fmeasure_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::fmeasure> const fmeasure = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(fmeasure)
        }

        using extract::fmeasure;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_f_measure_hpp__