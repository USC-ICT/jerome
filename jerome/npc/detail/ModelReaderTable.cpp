//
//  ModelReaderTable.cpp
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

#include "ModelReaderTable.hpp"

namespace jerome {
  namespace npc {
    namespace detail {
      ModelReaderTableImpl::ModelReaderTableImpl(const ObjectFactory& inObjectFactory)
      : mObjectFactory(inObjectFactory)
      , mCollection(of().makeNew<Collection>())
      {
      }
      
      void ModelReaderTableImpl::processLine(const std::vector<std::string>& inLine) {
        
      }
      
      Result<Collection>  ModelReaderTableImpl::collection() const {
        return mCollection;
      }
    }
  }
}
