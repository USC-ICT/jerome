//
//  average.hpp
//
//  Created by Anton Leuski on 5/12/13.
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