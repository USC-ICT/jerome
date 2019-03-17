//
//  async_js.cpp
//
//  Created by Anton Leuski on 2/14/13.
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

#include "async_js.hpp"
#include <jerome/async.hpp>
#include <jerome/scripting/javascript.hpp>

namespace jerome {
	namespace scripting {
		
		dispatch::queue&	strandFromContext(jerome::javascript::Context& inContext)
		{
      // STATIC
			static auto	theMap = new Map<js::ContextGroup, dispatch::queue>;
			auto iter = theMap->find(inContext.group());
			if (iter == theMap->end()) {
				iter = theMap->emplace(inContext.group(), dispatch::queue(dispatch::queue::SERIAL)).first;
			}
			return iter->second;
		}
		
	}
}
