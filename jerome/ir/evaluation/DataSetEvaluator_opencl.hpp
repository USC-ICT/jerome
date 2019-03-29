//
//  DataSetEvaluator_opencl.hpp
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

#ifndef __jerome_ir_evaluation_DataSetEvaluator_opencl_hpp__
#define __jerome_ir_evaluation_DataSetEvaluator_opencl_hpp__

#include <jerome/async.hpp>
#include <jerome/ir/ranker.hpp>

#include <jerome/ir/evaluation/accumulators/statistics/total_size.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/relevant_set.hpp>

#include <jerome/ir/evaluation/DataSetEntry.hpp>
#include <jerome/ir/evaluation/EvaluatorObjectiveFunction.hpp>

namespace jerome {
  namespace ir {
		namespace evaluation {

      template <class Ranker, class DataSet, class DSCollector,
        class RLCollector, class Index>
      class DataSetEvaluatorOpenCL
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
        data_set_type mDataSet;
        Index mCombinedIndex;

        typedef typename ranker_type::document_type document_type;
        typedef typename ranker_type::query_type query_type;
        typedef typename data_set_type::value_type data_set_entry_type;

        typedef EvaluatorObjectiveFunction<data_set_collector_type> parent_type;

      public:
        typedef RankedList<document_type> ranked_list_type;
        typedef typename ranker_type::result_type ranker_result_type;

        typedef typename parent_type::result_type result_type;
        typedef typename parent_type::argument_type argument_type;

        DataSetEvaluatorOpenCL(const ranker_type& inRanker,
          const data_set_type& inDataSet)
          : mRanker(inRanker), mDataSet(inDataSet)
        {
        }

        void evaluateAllEntriesInOneOp()
        {
          const ranker_result_type& rls = mRanker(mDataSet.index());

          for (std::size_t i = 0, n = mDataSet.size(); i < n; ++i) {
            const DataSetEntry<query_type, document_type>& entry(mDataSet[i]);

						const auto& rl(rls[i]);
						typedef typename decltype(rl)::value_type	sample_type;

            ranked_list_collector_type rl_collector(
              accumulators::tag::total_size::value =
                mRanker.countOfDocuments(),
							accumulators::tag::relevant_set::size =
								entry.relevantSet().size(),
							accumulators::tag::relevant_set::contains =
								[&](const sample_type& x) -> bool {
									return entry.relevance()(x.object());
								}
						);

            for (auto& rle : rl) {
              rl_collector(rle.object());
            }

            mDataSetCollector(rl_collector.value());
          }
        }

        void evaluateAllEntries()
        {
          evaluateAllEntriesInOneOp();
        }

        result_type operator () (argument_type x)
        {
          mRanker.setValues(x);
          mDataSetCollector = data_set_collector_type();
          evaluateAllEntries();
          return mDataSetCollector.value();
        }

      };

		}
	}
}


#endif // defined __jerome_ir_evaluation_DataSetEvaluator_opencl_hpp__
