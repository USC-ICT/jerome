//
//  engine.cpp
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

#include <jerome/scripting/scion.hpp>

//#include <jerome/async.h>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/transform.hpp>

#include <jerome/scripting/async_js.hpp>
#include <jerome/scripting/PlatformJSInterface.hpp>

#include "js_engine.hpp"

namespace jerome {
	namespace scripting {
		
		// this uses C++11 string literal
		const char * kScionJS =
#include "scion.js.inc"
		;

		const char * kStartupJS =
#include "startup.js.inc"
		;

		const char * kCompilerJS =
#include "utteranceCompiler.js.inc"
		;

		Engine::Engine()
      : mScriptingInited(false)
      , mEngineEventHandler([](const EngineEvent&){})
		{
			context()["__scionPlatform"]	=
        context().newObjectOfNativeClass<jerome::scripting::ScionPlatform>();

			evaluateScript(kScionJS, "scxml.interpreter.js");
			evaluateScript(kStartupJS, "scxml.startup.js");
			evaluateScript(kCompilerJS, "scxml.utteranceCompiler.js");
		}
		
    Engine::~Engine()
    {
      
    }
    
		void Engine::evaluateScript(const String& script, const String& sourceURL,
                                int startingLineNumber,
									const std::function<void(const js::Value&)>& callback) {
      auto weakSelf = std::weak_ptr<Engine>(my_shared_from_this_type::shared_from_this());
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
        PlatformJSInterface(my_shared_from_this_type::shared_from_this()));
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
      auto weakSelf = std::weak_ptr<Engine>(my_shared_from_this_type::shared_from_this());
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
      auto weakSelf = std::weak_ptr<Engine>(my_shared_from_this_type::shared_from_this());
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

