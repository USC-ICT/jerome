//
//  platform.cpp
//
//  Created by Anton Leuski on 7/20/15.
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

#include "platform.hpp"
#include <jerome/npc/detail/Ranker.hpp>
#include <jerome/npc/parsing.hpp>
#include <jerome/scripting/js_engine.hpp>

namespace jerome {
	namespace npc {
	
	
		void Platform::initialize(const String& locale)
		{
      detail::Engine::initialize(locale);
		}
		
		Platform::Platform()
		: parent_type(std::make_shared<scripting::Engine>())
		{
      implementation().loadScripts();
		}
		
		Platform::~Platform()
		{
		}
		
    OptionalError Platform::loadCollection(std::istream& is, 
                                           const OptionalString& inFormat)
    {
      return implementation().loadCollection(is, cpp::ObjectFactory(), 
                                             inFormat);
    }
	
    void Platform::loadDialogueManager(std::istream& is,
                                       const std::function<void(const Result<String>&)>& cb)
    {
      implementation().loadDialogueManager(is, cb);
    }

    void  Platform::postEvent(const String& inName,
                              const StringStringMap& inData,
                              const String& inMachineName)
    {
      implementation().postEvent(inName, inData, inMachineName);
    }

    OptionalError Platform::train(const TrainingParameters<Utterance>& params)
    {
      return implementation().train(params);
    }

    Result<double> Platform::evaluate(const EvaluationParameters<Utterance>& params)
    {
      return implementation().evaluate(params);
    }
    
    OptionalError Platform::saveCollection(std::ostream& os)
    {
      return implementation().saveCollection(os);
    }

    optional<Utterance> Platform::respond(const String& stateName,
                                          const String& inQuestion)
    {
      auto rl = implementation().search(stateName,
        detail::Engine::queryForString(inQuestion));
      if (rl.size()) {
        return rl[0];
      } else {
        return optional<Utterance>();
      }
    }

    void Platform::setEngineEventHandler(
      const scripting::EngineEventHandler& inEventHandler)
    {
      implementation().setEngineEventHandler(inEventHandler);
    }

    optional<Utterance> Platform::utteranceWithID(const String& inUtteranceID)
    {
      return implementation().utteranceWithID(inUtteranceID);
    }

		Collection& Platform::collection()
		{
			return implementation().collection();
		}

    const Collection& Platform::collection() const
    {
      return implementation().collection();
    }

		void Platform::collectionWasUpdated(const OptionalString& inStateName)
		{
			implementation().collectionWasUpdated(inStateName);
		}

    bool Platform::usingLookupTable() const
    {
      return implementation().usingLookupTable();
    }

    void Platform::setUsingLookupTable(bool flag)
    {
      implementation().setUsingLookupTable(flag);
    }
  }
}
