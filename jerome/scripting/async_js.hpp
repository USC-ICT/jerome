//
//  async_js.hpp
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

#ifndef __jerome_scripting_async_js_hpp__
#define __jerome_scripting_async_js_hpp__

namespace jerome {
  
  namespace dispatch {
    class queue;
  }

  namespace javascript {
    class Context;
  }

	namespace scripting {
		
		namespace js = jerome::javascript;
		dispatch::queue&	strandFromContext(js::Context& inContext);
		
	}
}

#endif // defined __jerome_scripting_async_js_hpp__
