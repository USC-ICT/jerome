//
//  to_record.hpp
//
//  Created by Anton Leuski on 8/30/15.
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

#ifndef __jerome_ir_evaluation_accumulators_to_record_hpp__
#define __jerome_ir_evaluation_accumulators_to_record_hpp__

#include <jerome/type/Record.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace detail {

          template <typename Excluded>
          struct excluded_features
          {
            template <typename Accumulator>
            struct apply
            {
              typedef typename
                boost::accumulators::detail::contains_feature_of<Excluded,
                Accumulator>::type
                feature_is_excluded;

              typedef typename
                boost::mpl::not_<feature_is_excluded>::type
                feature_is_not_excluded;

              typedef typename feature_is_not_excluded::type type;
            };
          };

//          template <typename Query, typename AccumulatorSet>
//          using printable_features = filtered_features <
//            typename AccumulatorSet::features_type,
//            // exclude these features
//            acc::features<
//              acc::tag::vector
//              , acc::tag::map_of<acc::tag::is_relevant>
//              , acc::tag::query<Query>
//              , acc::tag::is_relevant
//              , acc::tag::relevant_set
//              >
//            >;

          template <typename Accumulator>
          struct to_feature_tag
          {
            typedef typename std::remove_cv<
              typename std::remove_reference<
                Accumulator
                >::type
              >::type wrapper_type;
            typedef typename wrapper_type::feature_tag tag_type;
            typedef typename std::remove_cv<
              typename std::remove_reference<
                tag_type
                >::type
              >::type type;
          };

          template <typename S, typename V>
          struct AccumulatorRecordVisitor
          {
            explicit AccumulatorRecordVisitor(V& v, const S& s)
              : visitor(v)
              , arg(s)
            {
            }

            template <typename Accumulator>
            void operator () (Accumulator& accumulator) const
            {
              visitor(accumulators::accumulator_name<
                  typename to_feature_tag<Accumulator>::type
                  >::name(), accumulator.result(arg));
            }

          private:
            AccumulatorRecordVisitor& operator = (
              AccumulatorRecordVisitor const&)
              = delete;
            V& visitor;
            const S&  arg;
          };

          template <typename Arg, typename V>
          AccumulatorRecordVisitor<Arg, V> makeAccumulatorRecordVisitor(V& a,
                                                                     const Arg& arg)
          {
            return AccumulatorRecordVisitor<Arg, V>(a, arg);
          }

        }

        template <typename Excluded, typename S, typename F, typename E, typename V>
        void visitAccumulatorsExcluding(
          const boost::accumulators::accumulator_set<S, F, E>& A,
          const V& visitor)
        {
          typedef boost::accumulators::accumulator_set<S, F, E> AccumulatorSet;
          // this is ugly, but accumulator_set does not define visit_if() const
          AccumulatorSet& mutableA(const_cast<AccumulatorSet&>(A));
          Record result;
          mutableA.template visit_if<detail::excluded_features<Excluded>>(
            detail::makeAccumulatorRecordVisitor(visitor,
              boost::accumulators::detail::
               accumulator_params()(mutableA)));
        }

      }
    }
  }

}

#endif // defined __jerome_ir_evaluation_accumulators_to_record_hpp__
