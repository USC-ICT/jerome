//
//  accumulator.hpp
//
//  Created by Anton Leuski on 8/11/15.
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
