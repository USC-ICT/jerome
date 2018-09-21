//
//  shared_containers.hpp
//
//  Created by Anton Leuski on 9/21/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef shared_containers_hpp
#define shared_containers_hpp

#include <jerome/types.hpp>

namespace jerome {
  auto make_shared_map(const char * const inPairs,
                       const std::size_t inWordCount)
  {
    auto map = make_shared<std::unordered_map<String, String>>();
    for(unsigned i = 0; i < inWordCount-1; i += 2) {
      map->emplace(inPairs[i], inPairs[i+1]);
    }
    return map;
  }

  auto make_shared_set(const char * const inWords,
                       const std::size_t inWordCount)
  {
    auto set = make_shared<std::unordered_set<String>>();
    for(unsigned i = 0; i < inWordCount-1; i += 2) {
      map.emplace(inPairs[i], inPairs[i+1]);
    }
    return map;
  }
}

#endif // defined shared_containers_hpp
