//
//  sample_count.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_sample_count_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_sample_count_hpp__

#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// =============================================================================
// sample_count

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          struct sample_count_impl
            : subtractable_accumulator_base
          {

            typedef std::size_t result_type;
            
            sample_count_impl(dont_care)
              : value(0)
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
          struct sample_count
            : depends_on<>
          {
            typedef boost::mpl::always<accumulators::impl::sample_count_impl>
              impl;
          };
        }

        namespace extract {
          extractor<tag::sample_count> const sample_count = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(sample_count)
        }

        using extract::sample_count;
      }
    }
  }
}


#endif // __jerome_ir_evaluation_accumulators_statistics_sample_count_hpp__
