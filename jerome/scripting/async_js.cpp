//
//  async_js.cpp
//
//  Created by Anton Leuski on 2/14/13.
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

#include "async_js.hpp"
#include <jerome/async.hpp>
#include <jerome/scripting/javascript.hpp>

namespace jerome {
	namespace scripting {
		
		dispatch::queue&	strandFromContext(jerome::javascript::Context& inContext)
		{
			static Map<js::ContextGroup, dispatch::queue>	theMap;
			auto iter = theMap.find(inContext.group());
			if (iter == theMap.end()) {
				iter = theMap.emplace(inContext.group(), dispatch::queue(dispatch::queue::SERIAL)).first;
			}
			return iter->second;
		}
		
	}
}
