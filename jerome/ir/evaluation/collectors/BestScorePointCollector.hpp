//
//  BestScorePointCollector.hpp
//
//  Created by Anton Leuski on 8/10/15.
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

#ifndef __jerome_ir_evaluation_collectors_BestScorePointCollector_hpp__
#define __jerome_ir_evaluation_collectors_BestScorePointCollector_hpp__

#include <list>
#include <queue>

#include <jerome/ir/evaluation/accumulators/accumulator_set.hpp>
#include <jerome/ir/evaluation/collectors/collector.hpp>
#include <jerome/ir/evaluation/collectors/TrainingListCollector.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>
#include <jerome/math/scored.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {

      template <class Collector>
      class BestScorePointCollector
      {
        typedef typename Collector::sample_type								c_sample_type;
        typedef jerome::math::ScoredObject<c_sample_type>				c_element_type;
        typedef typename Collector::result_type								value_type;
				typedef Collector																			collector_type;

			public:
        typedef std::vector<c_element_type>										sample_type;
        typedef jerome::math::ScoredObject<value_type>					result_type;
				

//        typedef accumulators::subtractable_accumulator_set<V,
//          accumulators::features<accumulators::tag::total<void,
//              void>>> accumulator_type;

			private:
        template <typename T>
        struct ScoredIDObject
          : public jerome::math::ScoredObject<T>
        {
          typedef std::size_t   identifier_type;
          ScoredIDObject(double inScore, identifier_type inID,
            T inObject) : jerome::math::ScoredObject<T>(inScore, inObject),
            mID(inID)
          {
          }

          identifier_type identifier() const
          {
            return mID;
          }

        private:
          identifier_type mID;
        };

        typedef ScoredIDObject<value_type>                  element_type;
        typedef std::list<element_type>                     list_type;

        // priority queue is the fastest way to merge multiple sorted lists
        // I'm using pointers to store the lists because I'll be adding and
        // removing
        // the lists from the queue, so lets avoid expensive copy operation
        typedef list_type*                                  list_ptr_type;

        struct list_ptr_type_compare
        {
          constexpr bool operator () (const list_ptr_type& lhs,
                                      const list_ptr_type& rhs) const
          {
            return lhs->front().score() < rhs->front().score();
          }

        };

        typedef std::priority_queue<
          list_ptr_type,
          std::vector<list_ptr_type>,
          list_ptr_type_compare
          > priority_queue_type;

			public:

        void operator () (sample_type const& args)
        {
          list_ptr_type tmp(new list_type);
          std::size_t identifier  = queue().size();
          std::size_t counter = 0;
          for (auto& x : args) {
            if (counter++ == 0) {
              mModel.initial_values.push_back(x.object());
            } else {
              tmp->push_back(element_type(x.score(), identifier, x.object()));
            }
          }
          queue().push(tmp);
        }
				
        result_type value() const
        {
          // merge all lists into one
          list_type   allLists;
          while (!queue().empty()) {
            list_ptr_type list = queue().top();
            queue().pop();
            allLists.push_back(list->front());
            list->pop_front();
            if (!list->empty()) {
              queue().push(list);
            } else {
              delete list;
            }
          }

          std::vector<value_type>   values(mModel.initial_values);
          collector_type  collector;

          for (auto& o : values) {
            collector += o;
          }

          double      lastScore = std::numeric_limits<double>::max() / 2;
          value_type  bestValue = collector.value();
          double      bestScore = lastScore;

          for (auto& x : allLists) {

            if (lastScore != x.score()) {
              value_type  value = collector.value();
               if (value > bestValue) {
                bestValue = value;
                bestScore = (x.score() + lastScore) / 2;
              }
              lastScore = x.score();
            }

            collector -= values[x.identifier()];
            collector += x.object();
            values[x.identifier()] = x.object();
          }

          value_type  value = collector.value();
          if (value > bestValue) {
            bestValue = value;
            bestScore = lastScore;
          }

          return result_type(bestScore, bestValue);
        }

      protected:
        priority_queue_type& queue()
        {
          return mModel.queue;
        }

        priority_queue_type& queue() const
        {
          return mModel.queue;
        }

      private:
        struct model_type
        {
          mutable priority_queue_type queue;
          std::vector<value_type>  initial_values;
        };

        model_type  mModel;

      };

      template <class Collector>
      struct BestScoreValueCollector
        : public BestScorePointCollector<Collector>
      {
        typedef BestScorePointCollector<Collector> parent_type;
        typedef typename parent_type::result_type::object_type result_type;

        result_type value() const
        {
          return parent_type::value().object();
        }

      };
    }
  }
}

#endif // defined __jerome_ir_evaluation_collectors_BestScorePointCollector_hpp__
