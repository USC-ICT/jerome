//
//  Trainer.hpp
//
//  Created by Anton Leuski on 8/13/15.
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

#ifndef __jerome_ir_training_Trainer_hpp__
#define __jerome_ir_training_Trainer_hpp__

#include <jerome/ir/evaluation/DataSetEvaluator.hpp>
#include <jerome/ir/evaluation/collectors/BestScorePointCollector.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/average.hpp>
#include <chrono>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#include <nlopt.hpp>
#pragma clang diagnostic pop


namespace jerome {
  namespace ir {
    namespace training {

      using namespace evaluation;
      namespace acc = jerome::ir::evaluation::accumulators;

//        typedef jerome::ir::evaluation::RankedListCollectorOne<
//          Data::answer_type,
//          F
//          > ranked_list_collector_type;

// DSCollector is an accumulator that will be applied to the result of
// RLCollector across ranked lists at every score point. For example,
// average<f-score> or jsut average<>.
// You have make sure DScollector and RLcollector correlate in their
// features. It does not make sense to ask for f-score average if the
// RLcollector did not collect f-score in the first place.

      template <class Ranker, class DataSet, class DSCollector,
        class RLCollector>
      using TrainingDataSetEvaluator = DataSetEvaluator<
        Ranker,
        DataSet,

// The result will be the best possible measure at any score threshold.
// For example, "considering all the cutoff thresholds, what is the
// best possible f-score value we get from this ranker?"

        BestScoreValueCollector<DSCollector>,
        TrainingListCollector<RLCollector>
        >;

      template <class Ranker, class DataSet, class DSCollector,
        class RLCollector>
      using TrainingDataSetScoreExtractor = DataSetEvaluator<
        Ranker,
        DataSet,

// This one is similar to BestScoreValueCollector. The only difference is the
// function returns a score-value pair instead of the value. That way you
// can get the best score (threshold).

        BestScorePointCollector<DSCollector>,
        TrainingListCollector<RLCollector>
        >;

      struct Optimizer
        : public nlopt::opt
      {
        typedef nlopt::opt parent_type;
				typedef std::function<double(const std::vector<double>&)> objective_function_type;
				typedef std::function<void(Optimizer&)> callback_type;

        Optimizer(nlopt::algorithm inAlgorithm,
          const range_vector& inRanges);

				template <typename F>
        void set_max_objective(F&& f)
        {
          mObjectiveFunction = std::forward<F>(f);
					parent_type::set_max_objective(nlopt_objective, this);
        }

				template <typename F>
        void set_callback(F&& f)
        {
          mCallback = std::forward<F>(f);
        }

				double lastValue() const;
				std::vector<double> lastArgument() const;
				double elapsedTimeInSeconds() const;
				void stop();
				
				std::vector<double> optimize(const std::vector<double> &x0);
				
			private:
				objective_function_type	mObjectiveFunction;
				callback_type mCallback;
				double mLastValue;
				std::vector<double> mLastArgument;
				
				typedef std::chrono::time_point<std::chrono::system_clock>	time_point_type;
				time_point_type mStartTime;
				
				double applyObjective(const std::vector<double>& x);
        static double nlopt_objective(const std::vector<double>& x,
                                      std::vector<double>& grad,
                                      void * my_func_data);
      };

      template <typename Ranker, typename RLCollector>
      struct ObjectiveFunction
      {
        typedef Ranker ranker_type;
				typedef ObjectiveFunction<Ranker,RLCollector> this_type;
        typedef typename ranker_type::test_set_type test_set_type;
				typedef std::vector<double> arg_type;

        typedef RLCollector ranked_list_collector_type;

				template <class Sample, class Feature>
				using data_set_collector_one_type = collector<
					accumulators::subtractable_accumulator_set<
						Sample,
						accumulators::features<Feature>
						>,
					accumulators::extractor<Feature>
					>;

        typedef data_set_collector_one_type<
          typename ranked_list_collector_type::result_type,
          acc::tag::average
          > data_set_collector_type;

        typedef TrainingDataSetEvaluator<
          ranker_type,
          test_set_type,
          data_set_collector_type,
          ranked_list_collector_type
          > evaluator_type;

        typedef TrainingDataSetScoreExtractor<
          ranker_type,
          test_set_type,
          data_set_collector_type,
          ranked_list_collector_type
          > score_extractor_type;

        typedef typename score_extractor_type::result_type
          score_value_pair_type;

        ObjectiveFunction(ranker_type& ioRanker, test_set_type& inTestSet)
          : mEvaluator(ioRanker, inTestSet)
          , mExtractor(ioRanker, inTestSet)
        {
        }

        score_value_pair_type scoreValuePair(const arg_type& x)
        {
          return mExtractor(x);
        }

        double operator () (const arg_type& x)
        {
          return mEvaluator(x);
        }
				
      private:
        evaluator_type mEvaluator;
        score_extractor_type mExtractor;
      };


    }
  }
}

#endif // defined __jerome_ir_training_Trainer_hpp__
