//
//  relevant_set.hpp
//
//  Created by Anton Leuski on 8/8/15.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_relevant_set_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_relevant_set_hpp__

#include <functional>
#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// =============================================================================
// relevant_set

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        BOOST_PARAMETER_NESTED_KEYWORD(tag, relevant_set_size, size)
        BOOST_PARAMETER_NESTED_KEYWORD(tag, relevant_set_contains, contains)

        namespace impl {
          template <typename Sample>
          struct relevant_set_impl
            : subtractable_accumulator_base
          {
            typedef std::function<bool (const Sample&)>  contains_type;
            typedef std::size_t size_type;

            struct model_type
            {
              model_type(size_type inSize, contains_type inContains)
                : size(inSize)
                , contains(inContains)
              {
              }

              size_type size;
              contains_type contains;
            };

            typedef const model_type& result_type;

            template <typename Args>
            relevant_set_impl(Args const& args)
              : val(
                args[relevant_set_size | size_type()],
                args[relevant_set_contains |[] (const Sample&) {
                       return false;
                     }]
                )
            {
            }

            result_type result(dont_care) const
            {
              return this->val;
            }

          private:
            model_type val;
          };

        } // namespace impl

        namespace tag {
          struct relevant_set
            : depends_on<>
            , tag::relevant_set_size
            , tag::relevant_set_contains
          {
            typedef accumulators::impl::relevant_set_impl<boost::mpl::_1> impl;
          };
        }

        namespace extract {
          extractor<tag::relevant_set> const relevant_set =
          {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(relevant_set)
        }

        using extract::relevant_set;
      }
    }
  }
}

#endif // defined __jerome_ir_evaluation_accumulators_statistics_relevant_set_hpp__
