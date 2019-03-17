//
//  query.hpp
//
//  Created by Anton Leuski on 8/13/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_query_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_query_hpp__

#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// =============================================================================
// query

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        BOOST_PARAMETER_NESTED_KEYWORD(tag, query_value, value)
        BOOST_PARAMETER_NESTED_KEYWORD(tag, query_id, identifier)

        namespace impl {
          template <typename Q>
          struct query_impl
            : subtractable_accumulator_base
          {
            typedef Q query_type;
            typedef std::size_t id_type;

            struct model_type
            {
              model_type(const id_type& inID, const query_type& inQuery)
                : identifier(inID)
                , value(inQuery)
              {
              }

              id_type identifier;
              query_type value;
            };

            typedef const model_type& result_type;

            template <typename Args>
            query_impl(Args const& args)
              : val(
                args[query_id | id_type()],
                args[query_value | query_type()]
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
				
					template <typename Q>
          struct query
            : depends_on<>
            , tag::query_value
            , tag::query_id
          {
            typedef boost::mpl::always<accumulators::impl::query_impl<Q>> impl;
          };
        }

        namespace extract {
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, query, (typename))
        }

        using extract::query;

      }
    }
  }
}

#endif // defined __jerome_ir_evaluation_accumulators_statistics_query_hpp__
