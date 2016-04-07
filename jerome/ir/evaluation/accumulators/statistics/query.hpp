//
//  query.hpp
//
//  Created by Anton Leuski on 8/13/15.
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
