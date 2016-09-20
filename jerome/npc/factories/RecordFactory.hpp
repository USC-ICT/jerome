//
//  RecordFactory.hpp
//
//  Created by Anton Leuski on 9/1/15.
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
