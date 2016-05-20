//
//  string.cpp
//
//  Created by Anton Leuski on 9/30/15.
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

#include "string.hpp"

namespace jerome {
  bool hasSuffix(const String& inS, const String& inSuffix) {
    return (inS.length() >= inSuffix.length())
    ? (0 == inS.compare (inS.length() - inSuffix.length(), inSuffix.length(), inSuffix))
    : false;
  }
  
  String dropSuffix(const String& inS, const String& inSuffix) {
    return inS.substr(0, std::max((std::size_t)0, inS.length()-inSuffix.length()));
  }
  
  void replaceAll(String& ioString, const String& inOld, const String& inNew) {
    for(std::size_t pos = ioString.find(inOld, 0);
        pos != String::npos;
        pos = ioString.find(inOld, pos+inNew.length()))
    {
      ioString.replace(pos, inOld.length(), inNew);
    }
  }
}

