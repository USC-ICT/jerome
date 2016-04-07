//
//  collector.hpp
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