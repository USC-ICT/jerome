//
//  TrainerFactory.hpp
//  jerome
//
//  Created by Anton Leuski on 9/18/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_npc_factories_TrainerFactory_hpp__
#define __jerome_npc_factories_TrainerFactory_hpp__

#include <jerome/npc/factories/RecordFactory.hpp>
#include <jerome/npc/detail/Trainer.hpp>

namespace jerome {
  
  class Record;
  
  namespace npc {
    
    class TrainerFactory
      : public RecordFactory<
        TrainerFactory,
				detail::Trainer>
    {
      typedef RecordFactory<
        TrainerFactory,
        detail::Trainer> parent_type;
    public:
      TrainerFactory();

      using parent_type::make;
      
      Result<object_type> make(const String& identifier)
      {
        return parent_type::make(identifier, Record());
      }
    };
        
  }
}

#endif // __jerome_npc_factories_TrainerFactory_hpp__
