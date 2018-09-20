//
//  cf_string.cpp
//
//  Created by Anton Leuski on 9/19/18.
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

#include "cf_string.hpp"
#ifdef JEROME_IOS

namespace jerome { namespace cf {
  String::String(const jerome::String& inString)
  : parent_type(CFStringCreateWithCString(kCFAllocatorDefault,
                                          inString.c_str(),
                                          kCFStringEncodingUTF8))
  {
    release();
  }

  String::String(const char* inBytes, size_type inCount)
  : parent_type(CFStringCreateWithBytes(kCFAllocatorDefault,
                                        (const uint8_t*)inBytes, inCount,
                                        kCFStringEncodingUTF8, false))
  {
    release();
  }

  String::operator jerome::String () const {
    const char*  constBuffer  = CFStringGetCStringPtr(value(),
                                                      kCFStringEncodingUTF8);
    if (constBuffer != NULL) return jerome::String(constBuffer);

    CFIndex length = 4*CFStringGetLength(value());
    auto buffer = std::make_unique<char>(length);
    if (buffer && CFStringGetCString(value(), buffer.get(),
                                     length, kCFStringEncodingUTF8))
    {
      return jerome::String(buffer.get());
    } else {
      return jerome::String();
    }
  }

  String
  String::substr(size_type pos, size_type count) {
    auto tmp = CFStringCreateWithSubstring(kCFAllocatorDefault,
                                           value(), CFRange {pos, count});
    return String(String::move(tmp));
  }
}}

#endif
