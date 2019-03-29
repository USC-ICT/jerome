//
//  classifier_Tests.m
//
//  Created by Anton Leuski on 12/31/14.
//  Copyright (c) 2014 Anton Leuski & ICT/USC. All rights reserved.
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

#import <Foundation/Foundation.h>
#import <XCTest/XCTest.h>

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
//#include <jerome/npc/classifier.hpp>
#include <jerome/npc/model_cpp.hpp>
#include <jerome/npc/parsing.hpp>
#include <jerome/ir/evaluation.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#include <boost/accumulators/statistics/mean.hpp>
#pragma clang diagnostic pop

#include <jerome/ir/evaluation/accumulators/statistics/average.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/f_measure.hpp>
//#include <jerome/ir/evaluation/accumulators/statistics/ranked_list.hpp>
//#include <jerome/ir/evaluation/accumulators/statistics/best_score_value.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/macro_average.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/micro_average.hpp>

#include <jerome/ir/evaluation/collectors/collector.hpp>
//#include <jerome/ir/evaluation/collectors/feature_collector.hpp>
#include <jerome/ir/evaluation/collectors/composite_collector.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#include <nlopt.hpp>
#pragma clang diagnostic pop

//#include <jerome/ir/rm/distances_in_answer_space.hpp>
//#include <jerome/ir/rm/distances_in_mixed_space.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

@interface ClassifierTests : XCTestCase

@end

using namespace jerome;
using namespace jerome::npc;
using namespace jerome::math;
using namespace jerome::math::parameters;
using namespace boost::accumulators;

static unsigned int sCount = 0;

static double myvfunc(const std::vector<double> &x, std::vector<double> &grad, void *my_func_data)
{
	++sCount;
	jerome::ir::evaluation::objective_function* func = static_cast<jerome::ir::evaluation::objective_function*>(my_func_data);
	double value = (*func)(x, grad);
	std::cout << x << " = " << value << std::endl;
	std::cout.flush();
	return value;
}

struct MyDistanceComputationDelegate : public jerome::ir::DistanceComputationDelegate {
	WeightMatrix::value_type& operator ()(std::size_t i, std::size_t j) override {
		return (*x)(i, j);
	}
	void setObjectCount(std::size_t count) {
		x = new SymmetricWeightMatrix(count, count);
	}
	SymmetricWeightMatrix* x;
};

@implementation ClassifierTests

- (void)setUp
{
	[super setUp];
	// Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
	// Put teardown code here. This method is called after the invocation of each test method in the class.
	[super tearDown];
}

- (void)test
{
// --train-data $(SRCROOT)/../data/twins.plist.csxml --classifier-name "Anyone - The Twins"
	
	std::string stateName = "Anyone - The Twins";
	
	NSString* baseFolder = [[NSFileManager defaultManager] currentDirectoryPath];
	NSString* dataFilePath = [[[baseFolder stringByDeletingLastPathComponent]
														 stringByAppendingPathComponent:@"data"]
														stringByAppendingPathComponent:@"twins.plist.csxml"];
	
	std::string trainDataFile = [dataFilePath fileSystemRepresentation];
	
	configure([[[baseFolder stringByAppendingPathComponent:@"jerome"]
							stringByAppendingPathComponent:@"irregular_verbs.txt"]
						 fileSystemRepresentation],
						
						[[[[[baseFolder stringByDeletingLastPathComponent]
							 stringByAppendingPathComponent:@"3rd_party"]
							stringByAppendingPathComponent:@"kstem" ]
							stringByAppendingPathComponent:@"kstem-lexicon-beta"]
						 fileSystemRepresentation]
						);
	
	jerome::npc::cpp::ObjectFactory	of;
	jerome::npc::Collection					collection 	= loadCollection(of, trainDataFile.c_str());
	std::cout << "loaded training data from " << trainDataFile << std::endl;
	
	State						state;
	XCTAssertTrue(findStateWithName(collection, stateName.c_str(), state), @"Cannot find state %s", stateName.c_str());
	
	Data						data	= dataFromState(state, collection.utterance_index());
	//		Ranker			ranker		= unigramClassifier(collection, vm[oStateName].as<std::string>().c_str());
	
	std::pair<Data, Data>		testTrainSplit = data.split(0.05);
	std::pair<Data, Data>		devTrainSplit = testTrainSplit.second.split(0.5);
	
	auto	analyzer	= Unigram::makeUnigram("Unigram", "unigram");
	
	auto			ranker	= unigramRanker(state, devTrainSplit.second);
	
	typedef jerome::ir::training::TestSet<jerome::ir::HeapIndex, Utterance>	TestSet;
	
	TestSet	devSet(devTrainSplit.first, ranker.analyzer()); //UnigramBigram::makeAnalyzer()
	TestSet	testSet(testTrainSplit.first, ranker.analyzer());
	
	typedef	collector<accumulator_set<bool, features<tag::fmeasure>>>						RLCollector;
	typedef feature_collector<RLCollector::result_type, tag::ranked_list>						RLCollector1;
	typedef composite_collector<RLCollector, RLCollector1>										RLCollector2;
	
	typedef	feature_collector<RLCollector::result_type, tag::average<tag::fmeasure>>		DSCollector;
	typedef feature_collector<RLCollector2::result_type, tag::best_score_value<DSCollector>>	DSCollector1;
	
	//		jerome::ir::evaluation::DataSetEvaluator<Ranker, TestSet, DSCollector1, RLCollector2>	evaluator(ranker, std::move(devSet));
	//		auto	uttRanker = boost::bind(ranker, boost::bind(analyzer, _1)); // send ranker input through queryAnalzer first
	jerome::ir::evaluation::TrainingDataSetEvaluator<decltype(ranker), TestSet, DSCollector, RLCollector>	evaluator(ranker, std::move(devSet));
	
	std::vector<double>	ignore;
	
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::duration<double, std::micro> US;
	auto t0 = Clock::now();
	
	double value = evaluator.operator()(ranker.values(), ignore);
	
	auto t1 = Clock::now();
	std::cout << US(t1-t0).count() << " microseconds\n";
	
	std::cout << value << std::endl;
	
	MyDistanceComputationDelegate	xx;
	
			t0 = Clock::now();
			jerome::ir::rm::DistanceComputationInMixedSpace<decltype(ranker)>::compute(ranker, xx);
			t1 = Clock::now();
			std::cout << US(t1-t0).count() << " microseconds\n";
	
//			ranker.distances(xx, pp);
//	
//			std::cout << (*xx.x) << std::endl;
	
	range_vector	ranges = ranker.ranges();
	
	std::vector<double>	lb;
	lb.reserve(ranges.size());
	std::vector<double>	ub;
	ub.reserve(ranges.size());
	
	for(const RangeDomain& r : ranges) {
		lb.push_back(r.minimum());
		ub.push_back(r.maximum());
	}
	
	nlopt::opt lopt(nlopt::LN_BOBYQA, (unsigned int)ranges.size());
	
	lopt.set_lower_bounds(lb);
	lopt.set_upper_bounds(ub);
	lopt.set_xtol_abs(1e-4);
	lopt.set_ftol_rel(1e-3);
	lopt.set_max_objective(myvfunc, &evaluator);
	
	
	nlopt::opt gopt(nlopt::GN_DIRECT_L, (unsigned int)ranges.size());
	
	gopt.set_local_optimizer(lopt);
	gopt.set_lower_bounds(lb);
	gopt.set_upper_bounds(ub);
	gopt.set_xtol_abs(1e-4);
	gopt.set_ftol_rel(1e-4);
	
	gopt.set_max_objective(myvfunc, &evaluator);
	
	double				maxf;
	std::vector<double>	x	= ranker.values();
	x[0] = 0.5; x[1] = 0.5;
	
	nlopt::result result = gopt.optimize(x, maxf);
	
	t0 = Clock::now();
	value = evaluator.operator()(x, ignore);
	t1 = Clock::now();
	std::cout << US(t1-t0).count() << " microseconds\n";
	
	std::cout << result << " " << x << std::endl;
	
	std::cout << ::sCount << " calls " << maxf << " " << value << std::endl;
	
	lopt.set_ftol_rel(1e-5);
	result = lopt.optimize(x, maxf);
}

@end
