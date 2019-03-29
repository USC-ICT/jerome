//
//  average.hpp
//
//  Created by Anton Leuski on 5/12/13.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_average_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_average_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/total.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/sample_count.hpp>
#include <boost/accumulators/numeric/functional.hpp>

// =============================================================================
// average

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
				
					template <typename T>
					using fdiv = boost::numeric::functional::fdiv<
							T, std::size_t>;
				
          template <typename Sample, typename Feature>
          struct average_impl
            : subtractable_accumulator_base
          {
          private:
            typedef accumulator_set<Sample, features<Feature>>
              feature_accumulator_type;

            typedef typename boost::mpl::apply<
              feature_accumulator_type, Feature>::type
              feature_type;

            typedef typename feature_type::result_type
              feature_result_type;

          public:
					
						typedef typename fdiv<feature_result_type>::result_type result_type;

            average_impl(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              std::size_t denom = sample_count(args);
              return denom == 0 ? 0 : boost::numeric::average(
                total_of<Feature>(args), denom);
            }

          };

          template <typename Sample>
          struct average_impl1
            : subtractable_accumulator_base
          {

						typedef typename fdiv<Sample>::result_type result_type;

            average_impl1(dont_care)
            {
            }

            template <typename Args>
            result_type result(Args const& args) const
            {
              std::size_t denom = sample_count(args);
              return denom ==
                     0 ? 0 : boost::numeric::average(total(args), denom);
            }

          };

        }
        namespace tag {
          template <typename Feature>
          struct average_of
            : depends_on<total_of<Feature>, sample_count>
          {
            typedef accumulators::impl::average_impl<boost::mpl::_1,
              Feature> impl;
          };

          struct average
            : depends_on<total, sample_count>
          {
            typedef accumulators::impl::average_impl1<boost::mpl::_1> impl;
          };
        }

        namespace extract {
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, average_of, (typename))
          extractor<tag::average> const average = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(average)
        }

        using extract::average;
        using extract::average_of;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_average_hpp__
