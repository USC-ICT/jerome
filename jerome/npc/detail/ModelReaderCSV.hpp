//
//  ModelReaderCSV.hpp
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

#ifndef __jerome_npc_detail_ModelReaderCSV_hpp__
#define __jerome_npc_detail_ModelReaderCSV_hpp__

#include "ModelReader.hpp"

namespace jerome {
  namespace npc {
    namespace detail {
      class ModelReaderCSV: public ModelReaderFactory::Provider {
      public:
        static constexpr const char* IDENTIFIER = "jerome.reader.csv";
        
        Result<Collection>  provide(const ObjectFactory& inObjectFactory,
                                    std::istream& stream) override;

        std::vector<String> acceptableFileNameExtensions() const override;
      };
    }
  }
}

#endif // __jerome_npc_detail_ModelReaderCSV_hpp__
