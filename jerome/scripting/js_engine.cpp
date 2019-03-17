//
//  engine.cpp
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

#include <jerome/scripting/scion.hpp>

//#include <jerome/async.h>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/transform.hpp>

#include <jerome/scripting/async_js.hpp>
#include <jerome/scripting/PlatformJSInterface.hpp>
#include <jerome/scripting/scripts.hpp>

#include "js_engine.hpp"

namespace jerome {
	namespace scripting {
		
		Engine::Engine()
      : mScriptingInited(false)
      , mEngineEventHandler([](const EngineEvent&){})
		{
		}
		
    Engine::~Engine()
    {
      
    }
    
    void Engine::loadScripts()
    {
      context()["__scionPlatform"]	=
      context().newObjectOfNativeClass<jerome::scripting::ScionPlatform>();
      
      evaluateScript(SCION_SCRIPT.source,
                     SCION_SCRIPT.name);
      
      evaluateScript(JEROME_STARTUP_SCRIPT.source,
                     JEROME_STARTUP_SCRIPT.name);
      
      evaluateScript(JEROME_UTTERANCE_COMPILER_SCRIPT.source,
                     JEROME_UTTERANCE_COMPILER_SCRIPT.name);
    }
    
		void Engine::evaluateScript(const String& script, const String& sourceURL,
                                int startingLineNumber,
									const std::function<void(const js::Value&)>& callback) {
      auto weakSelf = std::weak_ptr<Engine>(Me::shared_from_this());
			performBlock([=]() mutable {
        if (auto self = weakSelf.lock()) {
          callback(self->context().evaluateScript(script, sourceURL,
                                                  startingLineNumber));
        }
			});
		}

		void Engine::evaluateScriptWithFilePath(const String& sourcePath,
									const std::function<void(const js::Value&)>& callback) {
			String content;
			String url = "file://" + sourcePath;
			String error;
			if (ScionPlatform::stringWithContentsOfURL(url, content, error)) {
				evaluateScript(content, url, 0, callback);
			}
		}

		void Engine::performBlock(const std::function<void(void)>& block)
		{
			dispatch::queue&	theStrand(strandFromContext(context()));
			theStrand.call(block);
		}

		String Engine::argumentsDescription() {
			String	result;
			for(auto x : js::Context::currentArguments()) {
				if (result.length()) result += ", ";
				result += (String)x;
			}
			return result;
		}

    void Engine::initializeScripting()
    {
      if (mScriptingInited) return;
      mScriptingInited = true;
      context()["classifier"] = context().newObjectOfNativeClass(
        PlatformJSInterface(Me::shared_from_this()));
    }
    
    void Engine::setEngineEventHandler(const EngineEventHandler& eventHandler)
    {
      mEngineEventHandler = eventHandler;
    }
    
    void Engine::handleEngineEvent(const EngineEvent& event)
    {
      mEngineEventHandler(event);
    }
    
    void  Engine::postEvent(const String& inName,
                            const StringStringMap& inData,
                            const String& inMachineName)
    {
      auto weakSelf = std::weak_ptr<Engine>(Me::shared_from_this());
      performBlock([=] () {
        if (auto self = weakSelf.lock()) {
          self->initializeScripting();
          self->context()["postEventToStateMachine"] (inMachineName,
                                                      inName, inData);
        }
      });
    }
    
    void  Engine::loadDialogueManager(std::istream& is,
                                      const load_dialogue_manager_callback& cb)
    {
      std::string s(std::istreambuf_iterator<char>(is), {});
      auto weakSelf = std::weak_ptr<Engine>(Me::shared_from_this());
      performBlock([=] () {
        if (auto self = weakSelf.lock()) {
          self->initializeScripting();
          self->context()["initStateMachineWithString"]
          (s, [cb](const js::Value& inError, const String& inName) {
            if (js::Context::currentArguments().size() == 1) {
              cb(Error((String)inError));
            } else {
              cb(inName);
            }
          });
        }
      });
    }

	}
}

