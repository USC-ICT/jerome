//
//  ModelReader.cpp
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
#include <jerome/xml/reader.hpp>
#include <jerome/xml/record_writer.hpp>
#include <jerome/npc/detail/ModelIO.hpp>

#include "ModelReader.hpp"
#include "ModelReaderXML.hpp"
#include "ModelReaderCSV.hpp"

namespace jerome {
  namespace npc {
    
    ModelReaderFactory::ModelReaderFactory()
    {
      registerProviderClass<detail::ModelReaderXML>();
      registerProviderClass<detail::ModelReaderCSV>();
    }
    
    OptionalString 
    ModelReaderFactory::providerIDForFileName(const String& inFileName) const
    {
      for(const auto& id : providerIDs()) {
        auto provider = providerWithID(id);
        if (provider) {
          auto casted = dynamic_cast<
            const ModelReaderFactory::Provider*>(&provider.value());
          if (casted && casted->acceptsFileName(inFileName)) {
            return id;
          }
        }
      }
      return OptionalString();
    }

    Result<Collection>  readCollection(const ObjectFactory& inObjectFactory,
                                       std::istream& stream,
                                       const OptionalString& providerID)
    {
      return ModelReaderFactory::sharedInstance()
        .make(providerID, inObjectFactory, stream);
    }
    
    bool 
    ModelReaderFactory::Provider::acceptsFileName(const String &inFileName) 
    const 
    {
      auto lowercased = to_lower(inFileName);
      for(const auto& ext : acceptableFileNameExtensions()) {
        auto lowercased_ext = "." + to_lower(ext);
        if (hasSuffix(lowercased, lowercased_ext)) {
          return true;
        }
      }
      return false;
    }

    std::vector<String> 
    ModelReaderFactory::Provider::acceptableFileNameExtensions() 
    const 
    {
      return std::vector<String>();
    }
  }
}
