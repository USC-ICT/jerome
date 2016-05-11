//
//  scripts.hpp
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
