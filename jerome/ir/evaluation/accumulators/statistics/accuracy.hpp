//
//  accuracy.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_accuracy_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_accuracy_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/is_relevant.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_size.hpp>

// =============================================================================
// accuracy

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct accuracy_impl
            : subtractable_accumulator_base
          {						
            typedef short result_type;

            template <typename Args>
            accuracy_impl(Args const& args)
              : value(0)
            {
              this->value = relevant_size(args) == 0 ? 1 : 0;
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (retrieved_count(args) == 1) {
                if (relevant_size(args) == 0) {
                  this->value = 0;
                } else {
                  this->value = is_relevant(args) ? 1 : 0;
                }
              }
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (retrieved_count(args) == 0) {
                this->value = relevant_size(args) == 0 ? 1 : 0;
              }
            }

            result_type result(dont_care) const
            {
              assert(this->value == 0 || this->value == 1);
              return this->value;
            }

          private:
            result_type value;
          };
        }
        namespace tag {
          struct accuracy
            : depends_on<retrieved_count, is_relevant, relevant_size>
          {
            typedef boost::mpl::always<accumulators::impl::accuracy_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::accuracy> const accuracy = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(accuracy)
        }

        using extract::accuracy;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_accuracy_hpp__