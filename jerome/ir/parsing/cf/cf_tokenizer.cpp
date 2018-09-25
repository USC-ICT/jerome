//
//  cf_tokenizer.cpp
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

#include "cf_tokenizer.hpp"

#if JEROME_PARSING == JEROME_PARSING_CF

namespace jerome { namespace cf {
  static cf::basic_object<CFStringTokenizerRef>
  _init(CFStringRef inString, CFLocaleRef inLocale) {
    auto tokenzer = CFStringTokenizerCreate(kCFAllocatorDefault,
                                            inString,
                                            CFRangeMake(0, CFStringGetLength(inString)),
                                            kCFStringTokenizerUnitWordBoundary,
                                            inLocale);
    return basic_object<CFStringTokenizerRef>(tokenzer);
  }

  void Tokenizer::init() {
    mTokenizer = _init(mString, mLocale);
  }

  Tokenizer::Tokenizer()
  : mLocale()
  , mString()
  , mTokenizer()
  {
  }

  Tokenizer::Tokenizer(CFStringRef inString,
                       Locale const & inLocale)
  : mLocale(inLocale)
  , mString(inString)
  , mTokenizer()
  {
  }

  Tokenizer::Tokenizer(const jerome::String* inString, Locale const & inLocale)
  : mLocale(inLocale)
  , mString(*inString)
  , mTokenizer()
  {
  }

  Tokenizer::Tokenizer(const jerome::String& inString, Locale const & inLocale)
  : mLocale(inLocale)
  , mString(inString)
  , mTokenizer()
  {
  }

  Tokenizer::Tokenizer(const uint8_t* inBytes,
                       std::size_t inLength,
                       bool inDoCopy,
                       Locale const & inLocale)
  : mLocale(inLocale)
  , mString(inDoCopy
            ? CFStringCreateWithBytes(kCFAllocatorDefault,
                                      inBytes, inLength,
                                      kCFStringEncodingUTF8,
                                      false)
            : CFStringCreateWithBytesNoCopy(kCFAllocatorDefault,
                                            inBytes, inLength,
                                            kCFStringEncodingUTF8,
                                            false,
                                            kCFAllocatorNull))
  , mTokenizer()
  {
  }
}}

#endif
