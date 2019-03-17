//
//  relevant_set.hpp
//
//  Created by Anton Leuski on 8/8/15.
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
