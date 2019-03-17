//
//  engine.hpp
//
//  Created by Anton Leuski on 10/26/12.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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
      
      void loadScripts();
      
    private:
      typedef std::enable_shared_from_this<Engine> Me;

      js::Context mContext;
      EngineEventHandler	mEngineEventHandler;
      bool								mScriptingInited;

      void initializeScripting();
    };
  }
}

#endif // defined __jerome_scripting_js_engine_hpp__
