//
//  total_size.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_total_size_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_total_size_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>

// =============================================================================
// total_size

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        BOOST_PARAMETER_NESTED_KEYWORD(tag, total_set_size, value)

        namespace impl {
          struct total_size_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;

            template <typename Args>
            total_size_impl(Args const& args)
              : val(args[total_set_size | std::size_t(0)])
            {
            }

            result_type result(dont_care) const
            {
              return this->val;
            }

          private:
            result_type val;
          };

        } // namespace impl

        namespace tag {
          struct total_size
            : depends_on<>
            , total_set_size
          {
            typedef boost::mpl::always<accumulators::impl::total_size_impl> impl;
          };
        }

        namespace extract {
          extractor<tag::total_size> const total_size = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(total_size)
        }

        using extract::total_size;

      }
    }
  }
}

namespace boost {
  namespace accumulators {
    namespace jerome_ac = jerome::ir::evaluation::accumulators;
    template <>
    struct feature_of<jerome_ac::tag::total_of<jerome_ac::tag::total_size>>
      : feature_of<jerome_ac::tag::total_size>
    {
    };

  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_total_size_hpp__