//
//  npc_model.hpp
//  jerome
//
//  Created by Anton Leuski on 5/10/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef _jerome_scripting_npc_model_hpp__
#define _jerome_scripting_npc_model_hpp__

#include <jerome/scripting/javascript.hpp>
#include <jerome/npc/model.hpp>

namespace jerome {
  namespace javascript {
    template <>
    struct ClassTraits<jerome::npc::Utterance>
    : DefaultClassTraits<jerome::npc::Utterance>
    {
      typedef jerome::npc::Utterance utterance_type;
      typedef DefaultClassTraits<jerome::npc::Utterance>  parent_type;
      ClassTraits();
      
      bool  hasProperty(Value& object, const String& propertyName) override;
      Value getProperty(Value& object, const String& propertyName) override;
      bool  setProperty(Value& object,
                        const String& propertyName,
                        const Value& value) override;
      bool  deleteProperty(Value& object, const String& propertyName) override;
      std::vector<std::string>  getPropertyNames(const Value& object) override;
      AL_JS_USE_ADAPTED_CALLBACKS      
    };
  }
}

#endif // _jerome_scripting_npc_model_hpp__
