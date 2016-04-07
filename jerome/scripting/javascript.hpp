//
//  javascript.hpp
//
//  Created by Anton Leuski on 10/23/12.
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

#ifndef __jerome_scripting_javascript_hpp__
#define __jerome_scripting_javascript_hpp__

#include <string>
#include <vector>
#include <iostream>
#include <JavaScriptCore/JavaScriptCore.h>

#include <jerome/scripting/javascript/exception.hpp>
#include <jerome/scripting/javascript/string.hpp>
#include <jerome/scripting/javascript/types.hpp>
#include <jerome/scripting/javascript/value.hpp>
#include <jerome/scripting/javascript/context.hpp>
#include <jerome/scripting/javascript/class.hpp>

#include <jerome/scripting/javascript/conversion.hpp>

#include <jerome/scripting/javascript/impl/string_imp.hpp>
#include <jerome/scripting/javascript/impl/value_imp.hpp>
#include <jerome/scripting/javascript/impl/class_imp.hpp>

#include <jerome/scripting/javascript/impl/context_imp.hpp>

namespace jerome { namespace javascript {
	
}}

#endif // defined __jerome_scripting_javascript_hpp__
