//
//  platform.hpp
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

#ifndef __jerome_npc_platform_hpp__
#define __jerome_npc_platform_hpp__

#include <jerome/types.hpp>
#include <jerome/npc/TrainingCallback.hpp>
#include <jerome/scripting/EngineEvent.hpp>
#include <jerome/npc/model_cpp.hpp>
#include <jerome/npc/model.hpp>

/**
* Unless specfifcally stated the API here should not throw any exception.
*/

namespace jerome {
  
  namespace scripting {
    class Engine;
  }

  namespace npc {

    class Collection;
    class Utterance;


    struct Platform : public ReferenceClassInterface<scripting::Engine> {
      typedef ReferenceClassInterface<scripting::Engine> parent_type;

      /**
      * Initializes the API. Needs to be called once before using any of the
      * other
      * functions.
      */
      static void initialize(const String& locale = "en_US.UTF-8");

      Platform();
      virtual ~Platform();

      /**
      * Loads the collection and the classfier parameters from the stream is.
      * Returns an error object if there is a problem during the load.
      */
      OptionalError loadCollection(std::istream& is);

      /**
      * Trains the classifier corresponding to the state stateName.
      */
      OptionalError train(
          const String& stateName,
          const TrainingCallback& callback = [](TrainingState&) {});

      /**
      * Saves the collection to a stream. Returns an error object if there is
      * a problem during the save.
      */
      OptionalError saveCollection(std::ostream& os);

      /**
      * Asks the classifier with name stateName to select the best response to
      * the utterance question. Returns the best reponse or nothing if no
      * response
      * score above the classifier threshold.
      */
      optional<Utterance> respond(const String& stateName,
                                  const String& question);

      // DM-related API
      /**
      * Loads an scxml file with the dialogue manager script. Potentially the
      * load
      * is happening on a different thread. The callback function is called with
      * either the DM state machine name or, in case of the failure, with the
      * error
      * object describing the problem.
      */
      void loadDialogueManager(std::istream& is,
                               const std::function<void(const Result<String>&)>&
                                   cb = [](const Result<String>&) {});

      /**
      * Posts an event with name inName and data inData to the state machine
      * with
      * the name inMachineName. Returns immediately.
      */
      void postEvent(const String& inName,
                     const StringStringMap& inData = StringStringMap(),
                     const String& inMachineName = "");

      /**
      * Sets the platform's engine event handler. The event handler will receive
      * external event send by the SCXML engine. It is up to the platform client
      * how
      * to handle the events. For example, the client may convert the event into
      * a VHMSG.
      */
      void setEngineEventHandler(const scripting::EngineEventHandler& eventHandler);

      /**
      * Find the utterance with the given ID. Return nothing, if the utterance
      * does not exist.
      */
      optional<Utterance> utteranceWithID(const String& inUtteranceID);

      /**
      * Get the reference to the Collection object to add new utterances from
      * the client side.
      */
      Collection& collection();

      /**
      * Tell the platform that you made changes to the collection, so it can 
      * update its caches.
      */
      void collectionWasUpdated(
          const OptionalString& inStateName = OptionalString());
    };
  }
}

#endif  // defined __jerome_npc_platform_hpp__
