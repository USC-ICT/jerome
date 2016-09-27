//
//  relevant_count.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_relevant_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_relevant_count_hpp__

#include <assert.h>
#include <jerome/ir/evaluation/accumulators/statistics/is_relevant.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>

// =============================================================================
// relevant_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct relevant_count_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;

            relevant_count_impl(dont_care)
              : value(0)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              if (is_relevant(args))
                ++this->value;
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              if (is_relevant(args)) {
                assert(this->value > 0);
                --this->value;
              }
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
          struct relevant_count
            : depends_on<is_relevant>
          {
            typedef boost::mpl::always<accumulators::impl::relevant_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::relevant_count> const
          relevant_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(relevant_count)
        }

        using extract::relevant_count;

      }
    }
  }
}

namespace boost {
  namespace accumulators {
    namespace jerome_ac = jerome::ir::evaluation::accumulators;
    template <>
    struct feature_of<jerome_ac::tag::total_of<jerome_ac::tag::relevant_count>>
      : feature_of<jerome_ac::tag::relevant_count>
    {
    };

  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_relevant_count_hpp__
