//
//  EvaluatorObjectiveFunction.hpp
//
//  Created by Anton Leuski on 8/12/15.
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

#ifndef __jerome_ir_evaluation_EvaluatorObjectiveFunction_hpp__
#define __jerome_ir_evaluation_EvaluatorObjectiveFunction_hpp__

#include <jerome/math/parameters/types.hpp>

namespace jerome {
  namespace ir {
		namespace evaluation {

      template <class DSCollector>
      struct EvaluatorObjectiveFunction {
        typedef const jerome::math::parameters::value_vector& argument_type;
        typedef typename DSCollector::result_type result_type;
      };

		}
	}
}

#endif // defined __jerome_ir_evaluation_EvaluatorObjectiveFunction_hpp__
