//
//  ModelReader.hpp
//
//  Created by Anton Leuski on 8/19/15.
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

#ifndef __jerome_npc_detail_ModelReader_hpp__
#define __jerome_npc_detail_ModelReader_hpp__

#include <jerome/types.hpp>
#include <jerome/type/Factory.hpp>

namespace jerome {
  namespace npc {
		struct Collection;
		struct ObjectFactory;
    
    class ModelReaderFactory: public Factory<
      ModelReaderFactory, Collection, const ObjectFactory&, std::istream&> 
    {
      typedef Factory<
        ModelReaderFactory, Collection, 
        const ObjectFactory&, std::istream&> parent_type;
    public:
      ModelReaderFactory();
      OptionalString providerIDForFileName(const String& inFileName) const;
      
      struct Provider: public parent_type::Provider {
        virtual std::vector<String> acceptableFileNameExtensions() const;
        virtual bool acceptsFileName(const String& inFileName) const; 
      };
    };
    
		Result<Collection>	
    readCollection(const ObjectFactory& inObjectFactory,
                   std::istream& stream, 
                   const OptionalString& providerID = 
                    ModelReaderFactory::sharedInstance().defaultProviderID());
	}
}

#endif // defined __jerome_npc_detail_ModelReader_hpp__
