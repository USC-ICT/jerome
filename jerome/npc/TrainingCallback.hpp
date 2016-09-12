//
//  TrainingCallback.hpp
//
//  Created by Anton Leuski on 9/21/15.
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
    };

    template <class Q>
    struct EvaluationParameters {
      String            stateName;
      List<Q>           testQuestions;
      Record            reporterModel;
      shared_ptr<std::ostream>     report;
    };

  }
}

#endif // defined __jerome_npc_TrainingCallback_hpp__
