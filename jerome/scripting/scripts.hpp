//
//  scripts.hpp
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

#ifndef __jerome_scripting_scripts_hpp__
#define __jerome_scripting_scripts_hpp__

namespace jerome {
  namespace scripting {
    
    struct ScriptLiteral {
      const char* name;
      const char* source;
    };
    
    extern ScriptLiteral SCION_SCRIPT;
    extern ScriptLiteral JEROME_STARTUP_SCRIPT;
    extern ScriptLiteral JEROME_UTTERANCE_COMPILER_SCRIPT;
    
  }
}

#endif // __jerome_scripting_scripts_hpp__
