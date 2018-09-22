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
  String::String(const jerome::String& inString)
  : parent_type(CFStringCreateWithCString(kCFAllocatorDefault,
                                                       inString.c_str(),
                                                       kCFStringEncodingUTF8))
  {}

  String::String(const char* inBytes, size_type inCount)
  : parent_type(CFStringCreateWithBytes(kCFAllocatorDefault,
                                                     (const uint8_t*)inBytes, inCount,
                                                     kCFStringEncodingUTF8, false))
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

  bool isAlpha(const String& string,
               const Locale& inLocale)
  {
    auto set = CFCharacterSetGetPredefined(kCFCharacterSetLetter);
    return isStringMemberOfCharSet(string, set);
  }

  bool isAlphanumeric(const String& string,
                      const Locale& inLocale)
  {
    auto set = CFCharacterSetGetPredefined(kCFCharacterSetAlphaNumeric);
    return isStringMemberOfCharSet(string, set);
  }

}}

#endif
