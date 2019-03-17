//
//  composite_collector.hpp
//
//  Created by Anton Leuski on 5/22/13.
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

#ifndef __jerome_ir_evaluation_collectors_composite_collector_hpp__
#define __jerome_ir_evaluation_collectors_composite_collector_hpp__

#include <jerome/ir/evaluation/collectors/collector.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {

      // collector that composes sample processing: C2(C1(sample))

      template <typename C1, typename C2>
      struct composite_collector
      {

        typedef typename C1::accumulator_type           accumulator_type;
        // The type of the samples that will be accumulated
        typedef typename accumulator_type::sample_type        sample_type;
        // An MPL sequence of the features that should be accumulated.
        typedef typename accumulator_type::features_type      features_type;
        // The type of the weight parameter. Must be
        typedef typename accumulator_type::weight_type        weight_type;
        typedef composite_collector<C1, C2> this_type;
        typedef typename C2::result_type              result_type;

        composite_collector()
        {
        }

        composite_collector(const this_type& inOther)
          : mC1(inOther.mC1), mC2(inOther.mC2)
        {
        }

        composite_collector(this_type& inOther)
          : mC1(inOther.mC1), mC2(inOther.mC2)
        {
        }

        composite_collector(const this_type&& inOther)
          : mC1(inOther.mC1), mC2(inOther.mC2)
        {
        }

        composite_collector(this_type&& inOther)
          : mC1(inOther.mC1), mC2(inOther.mC2)
        {
        }

        template <typename ... Args>
        explicit composite_collector(Args&& ... args)
          : mC1(std::forward<Args>(args) ...)
        {
        }

//			template <typename ... Args>
//			void operator() (Args&&... args)
//			{
//				mC1(std::forward<Args>(args)...);
//				mC2(mC1.value());
//			}

        template <typename Arg1, typename ... Args>
        void operator () (Arg1&& arg1, Args&& ... args)
        {
          mC1(std::forward<Arg1>(arg1), std::forward<Args>(args) ...);
          mC2(mC1.value(), std::forward<Args>(args) ...);
        }

        template <typename ... Args>
        this_type& operator += (Args&& ... args)
        {
          mC1.operator += (std::forward<Args>(args) ...);
          mC2(mC1.value());
          return *this;
        }

//			template <typename ... Args>
//			this_type& operator -= (Args&&... args)
//			{
//				mAccumulator.operator -= (std::forward<Args>(args)...);
//				return *this;
//			}

        result_type value() const
        {
          return mC2.value();
        }

      private:
        C1 mC1;
        C2 mC2;

      };
    }
  }
}

#endif // __jerome_composite_collector_hpp__
