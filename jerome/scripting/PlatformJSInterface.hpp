//
//  PlatformJSInterface.hpp
//
//  Created by Anton Leuski on 8/17/15.
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

#ifndef __jerome_scripting_PlatformJSInterface_hpp__
#define __jerome_scripting_PlatformJSInterface_hpp__

#include <jerome/scripting/js_engine.hpp>

namespace jerome {
  
  namespace npc {
    class Utterance;
  }
  
  namespace scripting {

    class Engine;

    namespace js = jerome::javascript;

    class PlatformJSInterface
      : public ReferenceClassInterface<Engine>
    {
    public:
      typedef ReferenceClassInterface<Engine> parent_type;
      using parent_type::parent_type;

      std::vector<jerome::npc::Utterance>  search(const String& rankerName,
                                     js::Value query);
      std::vector<jerome::npc::Utterance>  answers(const String& rankerName);

      optional<jerome::npc::Utterance> utteranceWithID(const String& inUtteranceID);

      void receiveEvent(const String& name,
                        const OptionalString& type,
                        const OptionalString& target,
                        const OptionalString& origin,
                        const StringStringMap& data);

    private:
      friend class Engine;
    };

  }
}

namespace jerome {
  namespace javascript {
    template <>
    struct ClassTraits<jerome::scripting::PlatformJSInterface>
      : public DefaultClassTraits<jerome::scripting::PlatformJSInterface>
    {
      ClassTraits();
    };
  }
}

#endif // defined __jerome_scripting_PlatformJSInterface_hpp__
