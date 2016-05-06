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
		{
			context()["__scionPlatform"]	= context().newObjectOfNativeClass<jerome::scripting::ScionPlatform>();

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
      js::Context theContext = context();
			performBlock([=]() mutable {
				callback(theContext.evaluateScript(script, sourceURL, startingLineNumber));
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
		
	}
}

