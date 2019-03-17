//
//  TrainerFactory.hpp
//  jerome
//
//  Created by Anton Leuski on 9/18/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
