//
//  composite_collector.hpp
//
//  Created by Anton Leuski on 5/22/13.
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