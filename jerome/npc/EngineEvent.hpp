//
//  EngineEvent.hpp
//
//  Created by Anton Leuski on 9/29/15.
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

#ifndef __jerome_npc_EngineEvent_hpp__
#define __jerome_npc_EngineEvent_hpp__

#include <functional>
#include <jerome/types.hpp>

namespace jerome {
  namespace npc {
	
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

#endif // defined __jerome_npc_EngineEvent_hpp__
