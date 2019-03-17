//
//  ModelReader.cpp
//
//  Created by Anton Leuski on 3/16/19.
//  Copyright © 2019 Anton Leuski & ICT/USC. All rights reserved.
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
