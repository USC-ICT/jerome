//
//  scripts.cpp
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
