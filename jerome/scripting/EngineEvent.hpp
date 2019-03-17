//
//  EngineEvent.hpp
//
//  Created by Anton Leuski on 9/29/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_scripting_EngineEvent_hpp__
#define __jerome_scripting_EngineEvent_hpp__

#include <functional>
#include <jerome/types.hpp>

namespace jerome {
  namespace scripting {
	
		class EngineEvent {
		public:
			virtual ~EngineEvent() {}
			virtual String name() const = 0;
			virtual OptionalString type() const = 0;
			virtual OptionalString target() const = 0;
			virtual OptionalString origin() const = 0;
			virtual StringStringMap data() const = 0;
		};

		typedef std::function<void (const EngineEvent&)> EngineEventHandler;
	
	}
}

#endif // defined __jerome_scripting_EngineEvent_hpp__
