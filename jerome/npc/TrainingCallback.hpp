//
//  TrainingCallback.hpp
//
//  Created by Anton Leuski on 9/21/15.
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

#ifndef __jerome_npc_TrainingCallback_hpp__
#define __jerome_npc_TrainingCallback_hpp__

#include <functional>

#include <jerome/type/Record.hpp>
#include <jerome/math/parameters/types.hpp>
#include <jerome/npc/detail/Trainer.hpp>

namespace jerome {
  namespace npc {
		class TrainingState
		{
		public:
			virtual ~TrainingState() {}
			virtual double lastValue() const = 0;
			virtual jerome::math::parameters::value_vector lastArgument() const = 0;
			virtual double elapsedTimeInSeconds() const = 0;
			virtual void stop() = 0;
			virtual String name() const = 0;
		};
    
		typedef std::function<void (TrainingState&)> TrainingCallback;
        
    template <class Q>
    struct TrainingParameters {
      String            stateName;
      TrainingCallback  callback;
      List<Q>           developmentQuestions;
      List<Q>           trainingQuestions;
      detail::Trainer   trainer;
      Record            rankerModel;
    };

    template <class Q>
    struct EvaluationParameters {
      String            stateName;
      List<Q>           testQuestions;
      List<Q>           trainingQuestions;
      Record            reporterModel;
      shared_ptr<std::ostream>     report;
    };

  }
}

#endif // defined __jerome_npc_TrainingCallback_hpp__
