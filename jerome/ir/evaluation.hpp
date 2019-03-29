//
//  evaluation.hpp
//
//  Created by Anton Leuski on 9/30/11.
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

#ifndef __jerome_ir_evaluation_hpp__
#define __jerome_ir_evaluation_hpp__

#include <jerome/ir/evaluation/DataSetEvaluator.hpp>
#include <jerome/math/functions.hpp>

#include <jerome/ir/evaluation/collectors/TrainingListCollector.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {

// Design
//	Each evaluation is a two step process: you compute something for each
//	element of a rnaked list and then you compute something for each
//	ranked list. I abstract these operations into "collector"s. A collector
//	has two functions: a call operator that accepts an arbitrary pack of data
//	and a result method that returns the value of the collector.
//
//	see the collectors subtree for more details
//
//	Collectors are generally implemented using accumulators. Please see
//	boost library documentation about the accumulators library. I extend
//	that library by adding many of IR-related statistics and measures.

// The accumulator/collector/feature pattern seems like a total overkill,
// but here are 2 advantages:
//	1. it abstracts away two operators (one on each RL element and one
//		on each RL) from the process of applying these operators. I can
//		change the algorithm (e.g., use OpenCL) or I can changer the
//		operators.
//
//	2. the scary collector/feature classes built on top of boost::accumulators
//		framework let me optimize things. I can construct a class that
//		keeps track of arbitrary number of features, e.g., if I only need
//		fscore, I do not track accuracy, or I can track both of them.
//		Without this support I either have to define a class for every
//		interesting combination of features -- tedious and boring.
//		Or, I define a class that supports all features I ever want. The latter
//		approach would waste storage and cpu cycles computing things I might
//		not need.
//
//		While this might not be a problem for simple one shot evaluation
//		stage, the cost of additonal cycles while constantly evaluating
//		performance during a training phase might be significant. For example,
//		If I only want to track fscore, I do not need to waste time computing
//		and storing the accuracy. That storage can get big very quickly:
//		consider that if I have 10,000 questions and 1,000 answers. When training,
//		I need to store the performance measure for every question/answer pair.
//		If the performance is 1 doouble, it's 40M. If I have to store the
//		structure containing all interesting statistics, e.g., 10, the memory
//		requirement will be 400M -- not a small change...
//
//		Finally, it has a nice benefit where I can specify features hierarchly:
//		If I need to compute fscore, it will compute precision, so if I decide
//		that I want average precision as well, the precision is ready.

// RLcollector -- Ranked List collector. called once for every element of
// a ranked list
// DScollector -- Data Set collector. called once for every ranked list
// produced by each sample in the data set.

// the result is a ranked list (list of scored pairs) of whatever the
// RLCollector returns, e.g., f-score.

			template <class Sample, class Features, class Extractor = jerome::math::identity>
			using RankedListCollector = collector<
				accumulators::accumulator_set<
					jerome::math::ScoredObject<Sample>,
					Features
					>,
				Extractor
				>;

			template <class Sample, class Feature>
			using RankedListCollectorOne = collector<
				accumulators::accumulator_set<
					jerome::math::ScoredObject<Sample>,
					accumulators::features<Feature>
					>,
				accumulators::extractor<Feature>
				>;

			template <class Sample, class Features, class Extractor = jerome::math::identity>
			using DataSetCollector = collector<
				accumulators::accumulator_set<
					Sample,
					Features
					>,
				Extractor
				>;

			template <class Sample, class Feature>
			using DataSetCollectorOne = collector<
				accumulators::accumulator_set<
					Sample,
					accumulators::features<Feature>
					>,
				accumulators::extractor<Feature>
				>;


    }
  }
}

#endif // defined __jerome_ir_evaluation_hpp__
