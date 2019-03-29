//
//  engine.hpp
//
//  Created by Anton Leuski on 7/25/15.
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
                                       jerome::npc::cpp::ObjectFactory(),
                                     const OptionalString& inFormat = 
                                       OptionalString());

        OptionalError saveCollection(std::ostream& os);

        OptionalError train(const TrainingParameters<Utterance>& params);

        Result<double> evaluate(const EvaluationParameters<Utterance>& params);

        static Utterance queryForString(const String& question);

				void collectionWasUpdated(const OptionalString& inStateName);

        bool usingLookupTable() const { return mUseLookupTable; }
        void setUsingLookupTable(bool flag) { mUseLookupTable = flag; }
        
      private:
        Collection					mCollection;
        StringMap<Ranker>		mRankers;
        bool                mUseLookupTable;
				
				void stateWasUpdated(const State& inState);
      };

    }
  }
}

#endif // defined __jerome_npc_engine_hpp__
