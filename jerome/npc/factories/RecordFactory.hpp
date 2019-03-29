//
//  RecordFactory.hpp
//
//  Created by Anton Leuski on 9/1/15.
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

#ifndef __jerome_npc_factories_RecordFactory_hpp__
#define __jerome_npc_factories_RecordFactory_hpp__

#include <jerome/type/Factory.hpp>
#include <jerome/type/Record.hpp>

namespace jerome {

  namespace npc {
	
		template <typename Derived, typename Object>
    class RecordFactory
      : public Factory<
        Derived,
				Object,
				const Record&>
    {
    public:
			typedef Factory<
        Derived,
				Object,
				const Record&> parent_type;
		
			typedef typename parent_type::object_type object_type;

      RecordFactory()
			{}
			
      using parent_type::make;
      
			Result<object_type> make(const Record& inRecord)
			{
        return parent_type::make(inRecord.at<String>(parent_type::PROVIDER_KEY),
          inRecord);
			}
			
    };

	}
}

#endif // defined __jerome_npc_factories_RecordFactory_hpp__
