//
//  ModelReaderTable.cpp
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

#include <jerome/npc/model.hpp>
#include "ModelReaderTable.hpp"

namespace jerome {
  namespace npc {
    namespace detail {
      
      struct ModelReaderTableImpl {
        
        struct Header {
          bool isQuestion;
          String name;
        };
        
        ModelReaderTableImpl(const ObjectFactory& inObjectFactory)
        : mObjectFactory(inObjectFactory)
        , mCollection(of().makeNew<Collection>())
        {
        }
        
        const ObjectFactory& of()
        {
          return mObjectFactory;
        }
                
        void processLine(const std::vector<std::string>& inLine) {
          if (mHeaders.empty()) {
            parseHeaders(inLine);
            return;
          }

          for(size_t i = 0, 
              n = std::min(inLine.size(), mHeaders.size()); i < n; ++i)
          {
            auto value = trim(inLine[i]);
            if (value.empty()) { continue; }
            auto header = mHeaders[i];
            if (header.name.empty()) { continue; }
            
          }          
        }

        void parseHeaders(const std::vector<std::string>& inLine) {
          for(const auto& field: inLine) {
            auto lowercased = to_lower(trim(field));
            std::vector<String> parts;
            boost::algorithm::split(parts, lowercased, boost::is_any_of("."));
            Header header;
            if (parts.size() == 1) {
              header.isQuestion = false;
              header.name = parts[0];
            } else {
              header.isQuestion = parts[0] == "question";
              header.name = parts[1];
            }
            mHeaders.push_back(header);
          }
        }

        Result<Collection> collection() const {
          return mCollection;
        }

        const ObjectFactory& mObjectFactory;
        Collection mCollection;
        List<Header> mHeaders;
      };
      
      ModelReaderTable::ModelReaderTable(const ObjectFactory& inObjectFactory)
      : parent_type(std::make_shared<ModelReaderTableImpl>(inObjectFactory))
      {
      }

      void 
      ModelReaderTable::processLine(const std::vector<std::string>& inLine) {
        implementation().processLine(inLine);
      }
      
      Result<Collection>  
      ModelReaderTable::collection() const {
        return implementation().collection();
      }
    }
  }
}
