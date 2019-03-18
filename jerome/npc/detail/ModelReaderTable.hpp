//
//  ModelReaderTable.hpp
//
//  Created by Anton Leuski on 3/16/19.
//  Copyright © 2019 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_npc_detail_ModelReaderTable_hpp__
#define __jerome_npc_detail_ModelReaderTable_hpp__

#include <jerome/types.hpp>

namespace jerome {
  namespace npc {
    class Collection;
    class ObjectFactory;
    
    namespace detail {
      class ModelReaderTableImpl;
      class ModelReaderTable
        : public ReferenceClassInterface<ModelReaderTableImpl> 
      {
        typedef ReferenceClassInterface<ModelReaderTableImpl> parent_type;
      public:
        ModelReaderTable(const ObjectFactory& inObjectFactory);
        void processLine(const std::vector<std::string>& inLine);
        Result<Collection> collection() const;
      };
    }
  }
}

#endif // __jerome_npc_detail_ModelReaderTable_hpp__
