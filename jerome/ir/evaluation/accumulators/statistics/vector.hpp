//
//  vector.hpp
//
//  Created by Anton Leuski on 5/12/13.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_vector_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_vector_hpp__

#include <vector>
#include <jerome/math/functions.hpp>
#include <jerome/ir/evaluation/accumulators/accumulator.hpp>

// =============================================================================
// vector

// vector -- list of samples
// vector_of<F> -- list of sample properties extracted by F(sample)
// map_of<F> -- list of current accumulator values F(accumulator(sample))

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace impl {

          template <typename Sample, typename Func>
          struct result_type
          {
            typedef typename Func::template result<Func(Sample)>::type type;
          };

          template <
            typename Sample,
            typename Func = jerome::math::identity,
            typename Element =
							typename jerome::ir::evaluation::accumulators::impl::result_type<Sample, Func>::type
            >
          struct vector_impl
            : subtractable_accumulator_base
          {
            typedef Element element_type;
            typedef std::vector<element_type>   vector_type;
            typedef const vector_type&          result_type;
            typedef shared_ptr<vector_type>      model_type;

            template <typename Args>
            vector_impl(Args const& args)
              : mValue(new vector_type)
            {
            }

            template <typename Args>
            void operator () (Args const& args)
            {
              value().push_back(Func()(
                  args[boost::accumulators::sample]));
            }

            template <typename Args>
            void subtract(Args const& args)
            {
              value().pop_back();
            }

            result_type result(dont_care) const
            {
              return value();
            }

          private:
            model_type mValue;

          protected:
            vector_type& value()
            {
              return *this->mValue;
            }

            const vector_type& value() const
            {
              return *this->mValue;
            }

          };

          template <typename Sample, typename Feature>
          struct map_impl
            : public vector_impl<
              Sample,
              void,
							typename jerome::ir::evaluation::accumulators::impl::result_type<
                accumulator_set<Sample, features<Feature>>,
                extractor<Feature>
                >::type
              >
          {
            typedef vector_impl<
              Sample,
              void,
              typename jerome::ir::evaluation::accumulators::impl::result_type<
                accumulator_set<Sample, features<Feature>>,
                extractor<Feature>
                >::type
              > parent_type;

            using parent_type::parent_type;

            typedef typename parent_type::result_type result_type;

            template <typename Args>
            void operator () (Args const& args)
            {
              this->value().push_back(extractor<Feature>()(
                  args[boost::accumulators::accumulator]));
            }

          };
        }

        namespace tag {

          struct vector
            : depends_on<>
          {
            typedef accumulators::impl::vector_impl<boost::mpl::_1,
              jerome::math::identity> impl;
          };

          template <typename Feature>
          struct vector_of
            : depends_on<>
          {
            typedef accumulators::impl::vector_impl<
              boost::mpl::_1,
              extractor<Feature>
              > impl;
          };

          template <typename Feature>
          struct map_of
            : depends_on<Feature>
          {
            typedef accumulators::impl::map_impl<
              boost::mpl::_1,
              Feature
              > impl;
          };

        }

        namespace extract {
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, map_of, (typename))
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, vector_of, (typename))
          extractor<tag::vector> const vector = {};
          BOOST_ACCUMULATORS_IGNORE_GLOBAL(vector)
        }

        using extract::map_of;
        using extract::vector_of;
        using extract::vector;

      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_vector_hpp__
