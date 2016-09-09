//
//  evaluation.cpp
//
//  Created by Anton Leuski on 9/16/15.
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


#include "evaluation.hpp"

#include <jerome/types.hpp>
#include <jerome/ir/training/Data.hpp>

#include <jerome/ir/evaluation/accumulators/statistics_fwd.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/f_measure.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/accuracy.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/average_precision.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/function.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/average.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/vector.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/query.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/micro_average.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/macro_average.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/p_false_alarm.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/p_miss.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/non_relevant_count.hpp>

#include <jerome/npc/model.hpp>
#include <jerome/npc/detail/Ranker_impl.hpp>
#include <jerome/npc/factories/ReporterFactory.hpp>

// the accumultor-based experiment report, while flexible, slows compilation down
// significantly. Introduce this lflag while developing to speed up the compilation.
// should be off in release code

// Note : Android NDK has problem building the whole reports. Thus needs to switch on this flag to build.
#ifdef __ANDROID__
#define JEROME_INTERNAL_BASIC_REPORT
#endif

#ifndef JEROME_INTERNAL_BASIC_REPORT
//#define JEROME_INTERNAL_BASIC_REPORT
#endif

namespace jerome {
	namespace npc {
		namespace detail {
		
			using namespace jerome::ir::evaluation::accumulators;
			
			OptionalError evaluate(const Record& inReporterModel,
														 std::ostream& os,
														 const Data& inTestData,
														 const Ranker& inRanker)
			{
				typedef ir::evaluation::RankedListCollector<
				Data::answer_type,
				boost::accumulators::features<
					tag::average_precision
				, tag::accuracy
#ifndef JEROME_INTERNAL_BASIC_REPORT
				, tag::fmeasure
				, tag::precision
				, tag::recall
				, tag::pfalsealarm
				, tag::pmiss
				, tag::total_size
				, tag::retrieved_count
				, tag::relevant_count
				, tag::non_relevant_count
				, tag::relevant_size
#endif
				, tag::vector
				, tag::map_of<tag::is_relevant>
				, tag::query<Data::question_type>
				>
				> ranked_list_collector_type;
				
				//        struct echo
				//        {
				//          void operator () (
				//            typename ranked_list_collector_type::result_type const& x) const
				//          {
				//            std::cout << fmeasure(x) << " " << retrieved_count(x) << " "
				//                      << relevant_count(x) << " " << precision(x) << " "
				//                      << recall(x) << " " << relevant_size(x) << std::endl;
				//          }
				//
				//        };
				
				typedef ir::evaluation::DataSetCollector<
				typename ranked_list_collector_type::result_type,
				boost::accumulators::features<
					tag::sample_count
#ifndef JEROME_INTERNAL_BASIC_REPORT
				, tag::average_of<tag::retrieved_count>
				, tag::average_of<tag::relevant_count>
				, tag::average_of<tag::non_relevant_count>
				, tag::average_of<tag::relevant_size>
				, tag::average_of<tag::total_size>
				, tag::micro_average_of<tag::fmeasure>
				, tag::micro_average_of<tag::precision>
				, tag::micro_average_of<tag::recall>
				, tag::micro_average_of<tag::pmiss>
				, tag::micro_average_of<tag::pfalsealarm>

				, tag::macro_average_of<tag::average_precision>
#endif
				, tag::macro_average_of<tag::accuracy>
#ifndef JEROME_INTERNAL_BASIC_REPORT
				, tag::macro_average_of<tag::fmeasure>
				, tag::macro_average_of<tag::precision>
				, tag::macro_average_of<tag::recall>
				, tag::macro_average_of<tag::pmiss>
				, tag::macro_average_of<tag::pfalsealarm>
#endif
				, tag::vector
				>
				> data_set_collector_type;
				
				auto testSet = inRanker.testSetWithData(inTestData);

				typedef jerome::ir::evaluation::DataSetEvaluator<
				Ranker,
				decltype(testSet),
				data_set_collector_type,
				ranked_list_collector_type
				> evaluator_type;
				

				evaluator_type eval(inRanker, testSet);
				auto y = eval(inRanker.values());
				
				auto r = ReporterFactory<decltype(y)>::sharedInstance().make(inReporterModel, os);
				if (!r) return r.error();
				
				r.value().report(inRanker, y);
				return Error::NO_ERROR;
			}
			
		}
	}
}
