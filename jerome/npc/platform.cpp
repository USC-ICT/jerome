//
//  platform.cpp
//
//  Created by Anton Leuski on 7/20/15.
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

#include "platform.hpp"
#include <jerome/npc/detail/Ranker.hpp>
#include <jerome/npc/parsing.hpp>
#include <jerome/scripting/js_engine.hpp>

namespace jerome {
	namespace npc {
	
	
		void Platform::initialize(const String& locale)
		{
			jerome::ir::filter::KStem::init();
			jerome::ir::filter::Dictionary::init();
#if !defined(__ANDROID__) // For some reason this line causes crash in Android.
			jerome::Locale::global(locale);
#else
#warning "Please find the reason why it crashes"
#endif
		}
		
		Platform::Platform()
		: parent_type(std::make_shared<scripting::Engine>())
		{
		}
		
		Platform::~Platform()
		{
		}
		
    OptionalError Platform::loadCollection(std::istream& is)
    {
      return implementation().loadCollection(is);
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

    OptionalError Platform::train(const String& stateName,
                                  const TrainingCallback& callback)
    {
      return implementation().train(stateName, callback);
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

		void Platform::collectionWasUpdated(const OptionalString& inStateName)
		{
			implementation().collectionWasUpdated(inStateName);
		}

  }
}