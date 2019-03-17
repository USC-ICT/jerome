//
//  string.cpp
//
//  Created by Anton Leuski on 9/30/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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

