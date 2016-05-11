//
//  engine.hpp
//
//  Created by Anton Leuski on 10/26/12.
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

#ifndef __jerome_scripting_js_engine_hpp__
#define __jerome_scripting_js_engine_hpp__


#include <jerome/types.hpp>

#include <jerome/npc/engine.hpp>

#include <jerome/scripting/javascript.hpp>
#include <jerome/scripting/EngineEvent.hpp>

namespace jerome {
  namespace scripting {

    namespace js = jerome::javascript;

    struct JEROME_SYMBOL_VISIBLE Engine
      : public jerome::npc::detail::Engine
      , public std::enable_shared_from_this<Engine>
    {
      Engine();
      virtual ~Engine();

      void evaluateScript(const String& script, const String& sourceURL = "",
                          int startingLineNumber = 0,
                          const std::function<void(const js::Value&)>&
                              callback = [](const js::Value&) {});
      
      void evaluateScriptWithFilePath(
          const String& sourcePath = "",
          const std::function<void(const js::Value&)>& callback =
              [](const js::Value&) {});

      void performBlock(const std::function<void(void)>& block);
      static String argumentsDescription();

      js::Context& context() { return mContext; }

      typedef std::function<void(const Result<String>&)> load_dialogue_manager_callback;
      void	loadDialogueManager(std::istream& is, const load_dialogue_manager_callback&);
      
      void            postEvent(const String& inName,
                                const StringStringMap& inData = StringStringMap(),
                                const String& inMachineName = "");
      
      
      void setEngineEventHandler(const EngineEventHandler& eventHandler);
      void handleEngineEvent(const EngineEvent& event);
      
    private:
      typedef std::enable_shared_from_this<Engine> my_shared_from_this_type;

      js::Context mContext;
      EngineEventHandler	mEngineEventHandler;
      bool								mScriptingInited;

      void initializeScripting();
    };
  }
}

#endif // defined __jerome_scripting_js_engine_hpp__
