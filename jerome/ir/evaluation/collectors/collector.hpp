//
//  collector.hpp
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

#ifndef __jerome_ir_evaluation_collectors_collector_hpp__
#define __jerome_ir_evaluation_collectors_collector_hpp__

#include <jerome/math/functions.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {
      template <class Accumulator, class Extractor = jerome::math::identity>
      struct collector
      {

        typedef Accumulator       accumulator_type;
        // The type of the samples that will be accumulated
        typedef typename accumulator_type::sample_type      sample_type;
        // An MPL sequence of the features that should be accumulated.
        typedef typename accumulator_type::features_type    features_type;
        // The type of the weight parameter. Must be
        typedef typename accumulator_type::weight_type      weight_type;
        typedef collector<accumulator_type, Extractor>    this_type;
//			typedef typename Extractor::result_type result_type;
        typedef typename Extractor::template result<Extractor(Accumulator)>::
           type result_type;

					//        collector() = default;
        collector(const this_type& inOther) = default;
        collector(this_type&& inOther) = default;
        this_type& operator = (const this_type& inOther) = default;
        this_type& operator = (this_type&& inOther)    = default;

        template <class E>
        collector(const collector<accumulator_type, E>& inOther)
          : mAccumulator(inOther.mAccumulator)
        {
        }

        template <class E>
        collector(collector<accumulator_type, E>&& inOther)
          : mAccumulator(std::forward<accumulator_type>(inOther.mAccumulator))
        {
        }

        template <typename ... Args>
        explicit collector(Args&& ... args)
          : mAccumulator(std::forward<Args>(args) ...)
        {
        }

        template <typename ... Args>
        void operator () (Args&& ... args)
        {
          mAccumulator(std::forward<Args>(args) ...);
        }

        template <typename ... Args>
        this_type& operator += (Args&& ... args)
        {
          mAccumulator.operator += (std::forward<Args>(args) ...);
          return *this;
        }

        template <typename ... Args>
        this_type& operator -= (Args&& ... args)
        {
          mAccumulator.operator -= (std::forward<Args>(args) ...);
          return *this;
        }

        result_type value() const
        {
          return Extractor()(mAccumulator);
        }

        accumulator_type&   accumulator()
        {
          return mAccumulator;
        }

        const accumulator_type& accumulator() const
        {
          return mAccumulator;
        }

      private:
        accumulator_type mAccumulator;
      };
    }
  }
}

#endif // __jerome_ir_evaluation_collectors_collector_hpp__
