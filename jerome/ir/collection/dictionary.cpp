//
//  dictionary.cpp
//
//  Created by Anton Leuski on 9/11/18.
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

#include <jerome/type/optional.hpp>
#include <jerome/type/error.hpp>
#include <jerome/type/persistence.hpp>
#include "dictionary.hpp"
#include "persistence_containers.hpp"

namespace jerome { namespace ir {


//  Dictionary::Dictionary(access_type inAccess, const char* inPath)
//  : parent_type(std::make_shared<detail::Dictionary>(inAccess, inPath))
//  {}
//
//  Dictionary::Dictionary()
//  : parent_type(std::make_shared<detail::Dictionary>())
//  {}
//
//  Dictionary::access_type Dictionary::access() const {
//    return implementation().access();
//  }
//
//  Dictionary::index_type
//  Dictionary::emplace(const std::string& inString) {
//    return implementation().emplace(inString);
//  }
//
//  Dictionary::index_type
//  Dictionary::emplace(const char* inString) {
//    return implementation().emplace(inString);
//  }
//
//  Dictionary::index_type
//  Dictionary::string2index(const std::string& inString) const {
//    return implementation().string2index(inString);
//  }
//
//  Dictionary::index_type
//  Dictionary::string2index(const char* inString) const {
//    return implementation().string2index(inString);
//  }
//
//  jerome::optional<std::string>
//  Dictionary::index2string(index_type inIndex) const {
//    return implementation().index2string(inIndex);
//  }
//
//  void Dictionary::optimize() {
//    return implementation().optimize();
//  }

}}
