//
//  f_measure.hpp
//
//  Created by Anton Leuski on 5/12/13.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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
