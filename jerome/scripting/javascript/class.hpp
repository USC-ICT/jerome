//
//  class.hpp
//
//  Created by Anton Leuski on 8/5/14.
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

#ifndef __jerome_scripting_javascript_class_hpp__
#define __jerome_scripting_javascript_class_hpp__

#include <JavaScriptCore/JavaScript.h>

namespace jerome { namespace javascript {

	struct Class {
		Class() {}
		virtual ~Class() {}
		virtual operator JSClassRef() { return nullptr; }
		virtual operator JSClassRef() const { return nullptr; }
	};
	

}}

#endif // defined __jerome_scripting_javascript_class_hpp__
