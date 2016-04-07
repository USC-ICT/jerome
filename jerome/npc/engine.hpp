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
#include <jerome/npc/EngineEvent.hpp>

#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/npc/detail/Ranker.hpp>
#include <jerome/npc/detail/Trainer.hpp>
#include <jerome/scripting/js_engine.hpp>

namespace jerome {
  namespace npc {
    namespace detail {

      class Engine
        : public jerome::scripting::Engine
				, public std::enable_shared_from_this<Engine>
      {

      public:
        typedef jerome::scripting::Engine parent_type;

        Engine();

        Result<Ranker> ranker(const String& stateName);

        struct undefined_state_error
          : public Error
        {
          undefined_state_error(const String& name)
            : Error(String("no state defined with name ") + name)
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

        OptionalError loadCollection(std::istream& is,
                                     const ObjectFactory& inObjectFactory =
                                       jerome::npc::cpp::ObjectFactory());

        OptionalError saveCollection(std::ostream& os);

        OptionalError train(const String& stateName,
                            const TrainingCallback& callback = [] (
                              TrainingState&) {});

        OptionalError evaluate(const String& stateName, const Data& inData);

				typedef std::function<void(const Result<String>&)> load_dialogue_manager_callback;
				void	loadDialogueManager(std::istream& is, const load_dialogue_manager_callback&);
				
        void            postEvent(const String& inName,
                                  const StringStringMap& inData = StringStringMap(),
                                  const String& inMachineName = "");

        static Utterance queryForString(const String& question);

				void initializeScripting();
				
				void setEngineEventHandler(const EngineEventHandler& eventHandler);
				void handleEngineEvent(const EngineEvent& event);

				void collectionWasUpdated(const OptionalString& inStateName);

      private:
        Collection					mCollection;
        StringMap<Ranker>		mRankers;
				EngineEventHandler	mEngineEventHandler;
				bool								mScriptingInited;
				
				void stateWasUpdated(const State& inState);
      };

    }
  }
}

#endif // defined __jerome_npc_engine_hpp__
