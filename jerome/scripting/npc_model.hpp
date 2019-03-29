//
//  npc_model.hpp
//  jerome
//
//  Created by Anton Leuski on 5/10/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
