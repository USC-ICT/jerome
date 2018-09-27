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

#if JEROME_PARSING == JEROME_PARSING_CF

namespace jerome { namespace cf {
  String::String(const char* inCString, bool inDoCopy)
  : String((const uint8_t*)inCString, strlen(inCString), inDoCopy)
  {}

  String::String(const jerome::String& inString)
  : String(inString.c_str(), true)
  {}

  String::String(const uint8_t* inBytes, size_type inCount, bool inDoCopy)
  : parent_type(inDoCopy
                ? CFStringCreateWithBytes(kCFAllocatorDefault,
                                          inBytes, inCount,
                                          kCFStringEncodingUTF8, false)
                : CFStringCreateWithBytesNoCopy(kCFAllocatorDefault,
                                                inBytes, inCount,
                                                kCFStringEncodingUTF8,
                                                false,
                                                kCFAllocatorNull)
                )
  {}

  String::operator jerome::String () const {
    const char*  constBuffer  = CFStringGetCStringPtr(value(),
                                                      kCFStringEncodingUTF8);
    if (constBuffer != NULL) return jerome::String(constBuffer);

    CFIndex length = 4*CFStringGetLength(value());
    auto buffer = std::make_unique<char[]>(length);
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
    return String(CFStringCreateWithSubstring(kCFAllocatorDefault,
                                           value(), CFRange {pos, count}));
  }

  bool operator == (const String& lhs, const String& rhs) {
    return kCFCompareEqualTo == CFStringCompare(lhs, rhs, 0);
  }

  String lowercased(const String& string, const Locale& inLocale) {
    CFMutableStringRef  lowerCaseString =
      CFStringCreateMutableCopy(kCFAllocatorDefault, 0, string);
    CFStringLowercase(lowerCaseString, inLocale);
    return String(lowerCaseString);
  }

  static bool isStringMemberOfCharSet(CFStringRef inString,
                                      CFCharacterSetRef inCharSet)
  {
    CFIndex          length = CFStringGetLength(inString);
    CFStringInlineBuffer   inlineBuffer;

    CFStringInitInlineBuffer(inString, &inlineBuffer,
                             CFRangeMake(0, length));

    for (CFIndex cnt = 0; cnt < length; ++cnt) {
      UniChar ch = CFStringGetCharacterFromInlineBuffer(&inlineBuffer, cnt);
      if (CFCharacterSetIsCharacterMember(inCharSet, ch)) return true;
    }
    return false;
  }

  bool is_alpha(const String& string,
                const Locale& inLocale)
  {
    auto set = CFCharacterSetGetPredefined(kCFCharacterSetLetter);
    return isStringMemberOfCharSet(string, set);
  }

  bool is_alphanumeric(const String& string,
                       const Locale& inLocale)
  {
    auto set = CFCharacterSetGetPredefined(kCFCharacterSetAlphaNumeric);
    return isStringMemberOfCharSet(string, set);
  }

}}

#endif
