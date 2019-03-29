//
//  total.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_total_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_total_hpp__

#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// =============================================================================
// total

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {
          template <typename Sample, typename Feature>
          struct total_impl
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
            typedef feature_result_type result_type;

            total_impl(dont_care)
              : value()
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              this->value +=
                extractor<Feature>()(args[sample]);
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              this->value -=
                extractor<Feature>()(args[boost::accumulators::sample]);
            }

            result_type result(dont_care) const
            {
              return this->value;
            }

          private:
            result_type value;
          };

          template <typename Sample>
          struct total_impl2
            : subtractable_accumulator_base
          {
						typedef Sample    result_type;

            total_impl2(dont_care)
              : value()
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              this->value += args[sample];
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              this->value -= args[sample];
            }

            result_type result(dont_care) const
            {
              return this->value;
            }

          private:
            result_type value;
          };

        }
        namespace tag {

          template <class Feature>
          struct total_of
            : depends_on<>
          {
            typedef accumulators::impl::total_impl<boost::mpl::_1, Feature> impl;
          };

          struct total
            : depends_on<>
          {
            typedef accumulators::impl::total_impl2<boost::mpl::_1> impl;
          };
        }

        namespace extract {
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, total_of, (typename))
          extractor<tag::total> const total = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(total)
        }

        using extract::total;
        using extract::total_of;

      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_total_hpp__
