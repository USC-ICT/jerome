//
//  average_precision.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_average_precision_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_average_precision_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/is_relevant.hpp>

// =============================================================================
// average_precision

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct average_precision_impl
            : subtractable_accumulator_base
          {

            typedef double result_type;

            template <typename Args>
            average_precision_impl(Args const& args)
              : value(0)
            {
              this->value = relevant_size(args) == 0 ? 1 : 0;
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (retrieved_count(args) == 1 && relevant_size(args) == 0) {
                this->value = 0;
              }
              if (is_relevant(args))
                this->value += (result_type)relevant_count(args) /
                               (result_type)retrieved_count(args);
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (retrieved_count(args) == 0) {
                this->value = relevant_size(args) == 0 ? 1 : 0;
              } else {
                if (is_relevant(args))
                  this->value -= (result_type)(relevant_count(args) + 1) /
                                 (result_type)(retrieved_count(args) + 1);
              }
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type denom = relevant_count(args);
              return denom == 0 ? 0 : (this->value / denom);
            }

          private:
            result_type value;
          };
        }

        namespace tag {
          struct average_precision
            : depends_on<relevant_count, retrieved_count,
              is_relevant, relevant_size>
          {
            typedef boost::mpl::always<accumulators::impl::
               average_precision_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::average_precision> const
          average_precision = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(average_precision)
        }

        using extract::average_precision;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_average_precision_hpp__