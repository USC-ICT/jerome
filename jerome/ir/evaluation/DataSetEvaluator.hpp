//
//  DataSetEvaluator.hpp
//
//  Created by Anton Leuski on 8/12/15.
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

#ifndef __jerome_ir_evaluation_DataSetEvaluator_hpp__
#define __jerome_ir_evaluation_DataSetEvaluator_hpp__

#include <jerome/async.hpp>
#include <jerome/ir/ranker.hpp>

#include <jerome/ir/evaluation/accumulators/statistics/total_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_set.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/query.hpp>

#include <jerome/ir/evaluation/DataSetEntry.hpp>
#include <jerome/ir/evaluation/EvaluatorObjectiveFunction.hpp>

namespace jerome {
  namespace ir {
		namespace evaluation {

      template <class Ranker, class DataSet, class DSCollector,
        class RLCollector>
      class DataSetEvaluator
        : public EvaluatorObjectiveFunction<DSCollector>
      {
      public:
        typedef Ranker ranker_type;
        typedef DataSet data_set_type;
        typedef DSCollector data_set_collector_type;
        typedef RLCollector ranked_list_collector_type;

      private:
        data_set_collector_type mDataSetCollector;
        ranker_type mRanker;
        const data_set_type mDataSet;

        typedef typename ranker_type::document_type document_type;
        typedef typename data_set_type::value_type data_set_entry_type;

        dispatch::queue mWorker;
        dispatch::queue mAdder;
        dispatch::group mWorkerGroup;
        dispatch::group mAdderGroup;

        typedef EvaluatorObjectiveFunction<data_set_collector_type> parent_type;

      public:
        typedef RankedList<document_type> ranked_list_type;
        typedef typename ranker_type::result_type ranker_result_type;

        typedef typename parent_type::result_type result_type;
        typedef typename parent_type::argument_type argument_type;

        DataSetEvaluator(const ranker_type& inRanker,
          const data_set_type& inDataSet)
          : mRanker(inRanker)
          , mDataSet(inDataSet)
          , mWorker(dispatch::queue::CONCURRENT)
          , mAdder(dispatch::queue::SERIAL)
        {
        }

        void evaluateOneEntry(const data_set_entry_type& entry)
        {
          const ranker_result_type& rr = mRanker(entry.query());
          const auto& rl = rr[0];
					
					typedef typename std::remove_reference<decltype(rl)>::type::value_type	sample_type;

          ranked_list_collector_type rl_collector(
            accumulators::tag::total_size::value =
              mRanker.countOfDocuments(),
            accumulators::tag::relevant_set::size =
              entry.relevantSet().size(),
            accumulators::tag::relevant_set::contains =
							[&](const sample_type& x) -> bool {
								return entry.relevance()(x.object());
							},
            accumulators::tag::query<typename data_set_entry_type::query_type>::value =
              entry.query(),
            accumulators::tag::query<typename data_set_entry_type::query_type>::identifier =
              entry.identifier()
						);

          for (auto& rle : rl) {
            rl_collector(rle);
          }

					auto rl_collector_result = rl_collector.value();

          mAdderGroup.call(mAdder, [this, rl_collector_result]{
						this->mDataSetCollector(rl_collector_result); });
        }

        void evaluateAllEntriesOnMultipleCPUThreads()
        {
          auto b = mDataSet.entries().begin();
          auto e = mDataSet.entries().end();

          if (b == e) return;

          evaluateOneEntry(*b);  // to load all internal caches

          // version 1: schedule as many queries per thread as possible.
          // cons: You have to know how many threads you have for that
          // pros: You are not wasting time on extra scheduling related code

          //				std::size_t		n	= e-b;
          //				std::size_t		cnt	= 8;
          //				std::size_t		div	= n / cnt;
          //				std::size_t		rem	= n % cnt;
          //
          //				for(std::size_t i = 0; i < cnt; ++i) {
          //					mWorkerGroup.async(mWorker,
          // boost::bind(&DataSetEvaluator::evaluateEntries,
          //              this, b + i*div + std::min(rem, i), b + (i+1)*div +
          //              std::min(rem, i+1) ));
          //				}

          // version 2: schedule one query at a time
          // it looks like the performance is about the same as version 1. But
          // the code is simpler.

          for (b++; b != e; b++) {
            mWorkerGroup.call(mWorker, [this, b] { this->evaluateOneEntry(*b); });
          }

          mWorkerGroup.wait();
          mAdderGroup.wait();
        }

        result_type operator () (argument_type x)
        {
          mRanker.setValues(x);
          mDataSetCollector = data_set_collector_type();
          evaluateAllEntriesOnMultipleCPUThreads();
          return mDataSetCollector.value();
        }

      };

		}
	}
}


#endif // defined __jerome_ir_evaluation_DataSetEvaluator_hpp__
