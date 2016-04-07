//
//  retrieved_count.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_retrieved_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_retrieved_count_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>

// =============================================================================
// retrieved_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct retrieved_count_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;
            retrieved_count_impl(dont_care) : value(0)
            {
            }

            void operator () (dont_care)
            {
              ++this->value;
            }

            void subtract(dont_care)
            {
              assert(this->value > 0);
              --this->value;
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
          struct retrieved_count
            : depends_on<>
          {
            typedef boost::mpl::always<accumulators::impl::retrieved_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::retrieved_count> const retrieved_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(retrieved_count)
        }

        using extract::retrieved_count;
      }
    }
  }
}

// we are telling the accumulator system that if total_of<tag::retrieved_count>
// is available, it's ok to use it in place of retrieved_count
// That way if we speccify total_of<tag::retrieved_count> on a data set
// collector accumulator, we will be able to compute micro average scores.
namespace boost {
  namespace accumulators {

    namespace jerome_ac = jerome::ir::evaluation::accumulators;

    template <>
    struct feature_of<jerome_ac::tag::total_of<jerome_ac::tag::retrieved_count>>
      : feature_of<jerome_ac::tag::retrieved_count>
    {
    };

  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_retrieved_count_hpp__