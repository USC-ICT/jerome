//
//  collection.hpp
//
//  Created by Anton Leuski on 7/28/14.
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

#ifndef __jerome_ir_rm_weighting_query_collection_hpp__
#define __jerome_ir_rm_weighting_query_collection_hpp__

#include <jerome/ir/rm/weighting.hpp>

namespace jerome {
  namespace ir {
    namespace rm {
      namespace weighting {
        namespace query {

#pragma mark - Query

          struct Query
          {
            //					virtual WeightMatrix computeQueryModelForQuery(const
            // Context& inContext, const Index& inQuery) const = 0;
          };

          using namespace jerome::ir::rm::weighting;

#pragma mark - QueryCollection

          template <class ... Args>
          struct QueryCollection; // : public Collection<Query, Args...> {};

          template <class Arg1>
          struct QueryCollection<Arg1>
            : public Collection<Query, Arg1>
          {
            typedef QueryCollection<Arg1> this_type;
            typedef Collection<Query, Arg1>     parent_type;
            using parent_type::parent_type;

            template <class Index>
            WeightMatrix computeQueryModelForQuery(
              const Context<Index>& inContext,
              const Index& inQuery) const
            {
              return std::get<0>(this->weightings()).computeQueryModelForQuery(
                inContext,
                inQuery);
            }

          };

          template <class Arg1, class Arg2, class ... Args>
          struct QueryCollection<Arg1, Arg2, Args ...>
            : public Collection<Query, Arg1, Arg2, Args ...>
          {
            typedef QueryCollection<Arg1, Arg2, Args ...>  this_type;
            typedef Collection<Query, Arg1, Arg2, Args ...>      parent_type;
            using parent_type::parent_type;

          };

#pragma mark - AddQueryProbabilities

          template <class ... Args>
          struct AddQueryProbabilities
            : public QueryCollection<Args ...> {};

          template <class Arg1>
          struct AddQueryProbabilities<Arg1>
            : public QueryCollection<Arg1>
          {
            static constexpr const char* IDENTIFIER =
              "edu.usc.ict.jerome.weighting.query.add";

            typedef AddQueryProbabilities<Arg1> this_type;
            typedef QueryCollection<Arg1>     parent_type;
            using parent_type::parent_type;

            Record model() const
            {
              Record record = parent_type::model();
              record.emplace_front(Weighting::NAME_KEY, this->name());
              record.emplace_front(Weighting::IDENTIFIER_KEY, IDENTIFIER);
              return record;
            }

          };

          namespace detail {

            template <class Index>
            inline MatrixSize queryModelMatrixSizeForQuery(
              const Context<Index>& inContext,
              const Index& inQuery)
            {
              MatrixSize size;
              size.rowCount = inContext.index().documentCount();
              size.columnCount = inQuery.documentCount();
              return size;
            }

            template <int N, class this_type>
            struct add_query_helper
            {
              template <class Index>
              static WeightMatrix computeQueryModelForQuery(
                const this_type& sup,
                const Context<Index>& inContext,
                const Index& inQuery)
              {
                return sup.alphas()[this_type::size - N] *
                       std::get<this_type::size -
                  N>(sup.weightings()).computeQueryModelForQuery(inContext,
                  inQuery) +
                       add_query_helper<N - 1,
                  this_type>::computeQueryModelForQuery(sup, inContext,
                  inQuery);
              }

            };

            template <class this_type>
            struct add_query_helper<0, this_type>
            {
              template <class Index>
              static auto computeQueryModelForQuery(
                const this_type& sup,
                const Context<Index>& inContext,
                const Index& inQuery)
              -> decltype(WeightMatrixZero(queryModelMatrixSizeForQuery(inContext,
                                                                        inQuery)))
              {
                return WeightMatrixZero(queryModelMatrixSizeForQuery(inContext,
                                                                     inQuery));
              }

            };

          }

          template <class Arg1, class Arg2, class ... Args>
          struct AddQueryProbabilities<Arg1, Arg2, Args ...>
            : public QueryCollection<Arg1, Arg2, Args ...>
          {
            static constexpr const char* IDENTIFIER =
              "edu.usc.ict.jerome.weighting.query.add";

            typedef AddQueryProbabilities<Arg1, Arg2, Args ...>  this_type;
            typedef QueryCollection<Arg1, Arg2, Args ...>    parent_type;
            using parent_type::parent_type;

            const static std::size_t size = parent_type::size;

            Record model() const
            {
              Record record = parent_type::model();
              record.emplace_front(Weighting::NAME_KEY, this->name());
              record.emplace_front(Weighting::IDENTIFIER_KEY, IDENTIFIER);
              return record;
            }

            template <class Index>
            WeightMatrix computeQueryModelForQuery(
              const Context<Index>& inContext,
              const Index& inQuery) const
            {
              return detail::add_query_helper<size,
                this_type>::computeQueryModelForQuery(*this, inContext,
                inQuery);
            }

          };

#pragma mark - MultiplyQueryProbabilities

          namespace detail {
            template <int N, class this_type>
            struct mult_query_helper
            {
              typedef mult_query_helper<N - 1,this_type> next_step_type;
              
              template <class Index>
              static WeightMatrix computeQueryModelForQuery(
                const this_type& sup,
                const Context<Index>& inContext,
                const Index& inQuery)
              {
                return jerome::element_prod(
                                                  jerome::element_pow(
                    std::get<this_type::size - N>(sup.weightings())
                      .computeQueryModelForQuery(inContext, inQuery),
                    sup.alphas()[this_type::size - N])
                , next_step_type::computeQueryModelForQuery(
                                                  sup, inContext, inQuery));
              }

            };

            template <class this_type>
            struct mult_query_helper<0, this_type>
            {
              template <class Index>
              static auto computeQueryModelForQuery(
                const this_type& sup,
                const Context<Index>& inContext,
                const Index& inQuery)
              -> decltype(WeightMatrixOnes(queryModelMatrixSizeForQuery(inContext,
                                                                        inQuery)))
              {
                return WeightMatrixOnes(queryModelMatrixSizeForQuery(inContext,
                                                                     inQuery));
              }

            };
          }

          template <class ... Args>
          struct MultiplyQueryProbabilities
            : public QueryCollection<Args ...> {};

          template <class Arg1>
          struct MultiplyQueryProbabilities<Arg1>
            : public QueryCollection<Arg1>
          {
            static constexpr const char* IDENTIFIER =
              "edu.usc.ict.jerome.weighting.query.multiply";

            typedef MultiplyQueryProbabilities<Arg1>  this_type;
            typedef QueryCollection<Arg1>     parent_type;
            using parent_type::parent_type;

            Record model() const
            {
              Record record = parent_type::model();
              record.emplace_front(Weighting::NAME_KEY, this->name());
              record.emplace_front(Weighting::IDENTIFIER_KEY, IDENTIFIER);
              return record;
            }

          };

          template <class Arg1, class Arg2, class ... Args>
          struct MultiplyQueryProbabilities<Arg1, Arg2, Args ...>
            : public QueryCollection<Arg1, Arg2, Args ...>
          {
            static constexpr const char* IDENTIFIER =
              "edu.usc.ict.jerome.weighting.query.multiply";

            typedef MultiplyQueryProbabilities<Arg1, Arg2, Args ...> this_type;
            typedef QueryCollection<Arg1, Arg2, Args ...>    parent_type;
            using parent_type::parent_type;

            const static std::size_t size = parent_type::size;

            Record model() const
            {
              Record record = parent_type::model();
              record.emplace_front(Weighting::NAME_KEY, this->name());
              record.emplace_front(Weighting::IDENTIFIER_KEY, IDENTIFIER);
              return record;
            }

            template <class Index>
            WeightMatrix computeQueryModelForQuery(
              const Context<Index>& inContext,
              const Index& inQuery) const
            {
              return detail::mult_query_helper<size,
                this_type>::computeQueryModelForQuery(*this, inContext,
                inQuery);
            }

          };

        }
      }
    }
  }
}

#endif // defined __jerome_ir_rm_weighting_query_collection_hpp__
