//
//  engine.hpp
//
//  Created by Anton Leuski on 7/25/15.
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

#ifndef __jerome_npc_engine_hpp__
#define __jerome_npc_engine_hpp__

#include <jerome/types.hpp>
#include <jerome/npc/model.hpp>
#include <jerome/npc/model_cpp.hpp>
#include <jerome/npc/TrainingCallback.hpp>

#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/npc/detail/Ranker.hpp>
#include <jerome/npc/detail/Trainer.hpp>

namespace jerome {
  namespace npc {
    namespace detail {

      class Engine
      //        : public std::enable_shared_from_this<Engine>
      {
      public:

        static void initialize(const String& locale = "en_US.UTF8");

        Engine();

        Result<Ranker> ranker(const String& stateName);

        struct undefined_state_error
          : public Error
        {
          undefined_state_error(const String& name)
            : Error(String("no state defined with name \"") + name + "\"")
          {
          }

        };

        std::vector<Utterance>  search(const String& stateName,
                                       const Utterance& query);
        std::vector<Utterance>  answers(const String& stateName);
        std::vector<Utterance>  answers();
        optional<Utterance> utteranceWithID(const String& inUtteranceID);

        Collection& collection()
        {
          return mCollection;
        }

        const Collection& collection() const
        {
          return mCollection;
        }

        OptionalError loadCollection(std::istream& is,
                                     const ObjectFactory& inObjectFactory =
                                       jerome::npc::cpp::ObjectFactory());

        OptionalError saveCollection(std::ostream& os);

        OptionalError train(const TrainingParameters<Utterance>& params);

        Result<double> evaluate(const EvaluationParameters<Utterance>& params);

        static Utterance queryForString(const String& question);

				void collectionWasUpdated(const OptionalString& inStateName);

      private:
        Collection					mCollection;
        StringMap<Ranker>		mRankers;
				
				void stateWasUpdated(const State& inState);
      };

    }
  }
}

#endif // defined __jerome_npc_engine_hpp__
