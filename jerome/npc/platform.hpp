//
//  platform.hpp
//
//  Created by Anton Leuski on 7/20/15.
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
       Initializes the API. Needs to be called once before using any of the
       other functions.

       @param locale locale specification
       */
      static void initialize(const String& locale = "en_US.UTF-8");

      Platform();
      virtual ~Platform();

      /**
       Loads the collection and the classfier parameters from the stream is.
       Returns an error object if there is a problem during the load.

       @param is source stream

       @return error if any
       */
      OptionalError loadCollection(std::istream& is,
                                   const OptionalString& inFormat = 
                                     OptionalString());

      /**
       Trains the classifier.

       @param params parameters to customize the training process.

       @return an error if any
       */
      OptionalError train(const TrainingParameters<Utterance>& params);

      /**
       Evaluates the classifier

       @param params parameters to customize the evaluation process.

       @return classifier accuracy or error
       */
      Result<double> evaluate(const EvaluationParameters<Utterance>& params);

      /**
       Saves the collection to a stream. Returns an error object if there is
       a problem during the save.

       @param os target stream

       @return error if any
       */
      OptionalError saveCollection(std::ostream& os);

      /**
       Asks the classifier with name stateName to select the best response to
       the utterance question. Returns the best reponse or nothing if no
       response score above the classifier threshold.

       @param stateName classifier name
       @param question  question text

       @return the best reponse if exists
       */
      optional<Utterance> respond(const String& stateName,
                                  const String& question);

      // DM-related API
      /**
       Loads an scxml file with the dialogue manager script. Potentially the
       load is happening on a different thread. The callback function is called 
       with either the DM state machine name or, in case of the failure, with 
       the error object describing the problem.

       @param is  source stream
       @param cb  callback function to call when the script is loaded
       */
      void loadDialogueManager(std::istream& is,
                               const std::function<void(const Result<scripting::DialogueManagerMetadata>&)>&
                                   cb = [](const Result<scripting::DialogueManagerMetadata>&) {});

      /**
       Posts an SCXML event with the given name and data to the state machine
       with the name inMachineName. Returns immediately.

       @param inName event name
       @param inData event data
       @param inMachineName state machine name
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
      const Collection& collection() const;

      /**
      * Tell the platform that you made changes to the collection, so it can 
      * update its caches.
      */
      void collectionWasUpdated(
          const OptionalString& inStateName = OptionalString());


      /**
       Determines whether Jerome uses lookup table to speed up the search.
       If the query exists in the training data, the lookup table returns
       all and only responses linked to that entry.

       @return true if lookup table is enabled.
       */
      bool usingLookupTable() const;
      void setUsingLookupTable(bool flag);
    };
  }
}

#endif  // defined __jerome_npc_platform_hpp__
