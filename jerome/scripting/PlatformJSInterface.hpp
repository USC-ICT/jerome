//
//  PlatformJSInterface.hpp
//
//  Created by Anton Leuski on 8/17/15.
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
