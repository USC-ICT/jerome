//
//  ModelReader.hpp
//
//  Created by Anton Leuski on 8/19/15.
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
