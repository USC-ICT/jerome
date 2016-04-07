//
//  roc_area.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_roc_area_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_roc_area_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/retrieved_count.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_count.hpp>

// =============================================================================
// roc_area

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct roc_area_impl
            : subtractable_accumulator_base
          {
						
            typedef double result_type;

            roc_area_impl(dont_care) : value(0)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (!is_relevant(args))
                this->value += relevant_count(args);
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (!is_relevant(args)) {
                double rc = relevant_count(args);
                assert(this->value >= rc);
                this->value -= rc;
              }
            }

            // TODO validate that this is correct
            template <typename Args>
            result_type result(Args const& args) const
            {
              result_type relc  = relevant_count(args);
              if (relc == 0) return 0;
              result_type nonc  = retrieved_count(args) - relc;
              if (nonc == 0) return 1;
              return this->value / (relc * nonc);
            }

          private:
            typedef relevant_count_impl::result_type  value_type;
            value_type value;
          };
        }
        namespace tag {
          struct roc_area
            : depends_on<relevant_count, retrieved_count, relevant>
          {
            typedef boost::mpl::always<accumulators::impl::roc_area_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::roc_area> const roc_area = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(roc_area)
        }

        using extract::roc_area;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_roc_area_hpp__