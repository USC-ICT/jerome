//
//  accumulator.hpp
//
//  Created by Anton Leuski on 8/11/15.
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

#ifndef __jerome_ir_evaluation_accumulators_accumulator_hpp__
#define __jerome_ir_evaluation_accumulators_accumulator_hpp__

// ==============================================================
// somewhere deep insde the boost bowels someone defines nil or Nil
// data structure. That's would be ok, but OSX legacy header have them
// defined as macros. Conflict. So we undefine the macros here. And
// restore them after the include statements

#ifdef nil
# define al_defined_nil
# pragma push_macro("nil")
# undef nil
# pragma push_macro("Nil")
# undef Nil
#endif

// supress clang warnings
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
// --------------------------------------------------------------

//#include <boost/accumulators/accumulators_fwd.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/accumulators/framework/parameters/sample.hpp>
#include <boost/accumulators/framework/depends_on.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/accumulators/numeric/functional.hpp>

// --------------------------------------------------------------
#pragma clang diagnostic pop

#ifdef al_defined_nil
# undef al_defined_nil
# pragma pop_macro("Nil")
# pragma pop_macro("nil")
#endif
// ==============================================================

#include <jerome/types.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        using namespace boost::accumulators;

        struct subtractable_accumulator_base
          : accumulator_base
        {
          boost::accumulators::detail::void_ subtract(dont_care)
          {
          }

        };

        template <typename T>
        class name_or_default
        {
					template<typename C> // detect name::value
					static constexpr auto test(decltype(&C::name))
						-> decltype(C::name())
					{
						return C::name();
					}
					
					template<typename C> // worst match
					static constexpr auto test(...)
						-> decltype("na")
					{
						return "na";
					}
				public:
					static String name() {
						return test<T>(0);
					}
        };

      }
    }
  }
}

#endif // defined __jerome_ir_evaluation_accumulators_accumulator_hpp__
