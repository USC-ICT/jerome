//
//  scripts.cpp
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

#include "scripts.hpp"


namespace jerome {
  namespace scripting {
    
    // this uses C++11 string literal
    static const char * kScionJS =
#include "scion.js.inc"
    ;
    
    static const char * kStartupJS =
#include "startup.js.inc"
    ;
    
    static const char * kCompilerJS =
#include "utteranceCompiler.js.inc"
    ;
    
    ScriptLiteral SCION_SCRIPT
    = { "scion.js", kScionJS };

    ScriptLiteral JEROME_STARTUP_SCRIPT
    = { "jeromeStartup.js", kStartupJS };

    ScriptLiteral JEROME_UTTERANCE_COMPILER_SCRIPT
    = { "jeromeUtteranceCompiler.js", kCompilerJS };
    
  }
}
