//
//  parsing_cf.cpp
//
//  Created by Anton Leuski on 4/15/12.
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

#include <jerome/ir/parsing/parsing_fwd.hpp>

#if JEROME_PARSING == JEROME_PARSING_CF

#include <jerome/ir/parsing.hpp>

namespace jerome { 
	namespace ir {

    bool
    Tokenizer::getNextToken(Token& ioToken) {
      auto token = mTokenizer.nextToken<Token>();
      if (token.isEOS()) return false;
      ioToken = token;
      return true;
    }

    NonTokenizer::NonTokenizer(CFStringRef inString,
                               jerome::Locale const & inLocale)
    : mToken((String)cf::String(inString), 0,
             (Token::size_type)CFStringGetLength(inString),
             Token::Type::word)
    , mLocale(inLocale)
    , mHasToken(true)
    {

    }

    NonTokenizer::NonTokenizer(const String* inString,
                               jerome::Locale const & inLocale)
    : mToken(*inString, 0, (Token::size_type)inString->length(),
             Token::Type::word)
    , mLocale(inLocale)
    , mHasToken(true)
    {
      //		std::cout<< locale().name() << std::endl;
    }

	namespace filter {

// -----------------------------------------------------------------------------	
#pragma mark - Lowercase
	
    bool Lowercase::getNextToken(Token& ioToken) {
      if (!TokenFilter::getNextToken(ioToken)) return false;

      cf::String	string(ioToken.text());
      auto lowercasedString = lowercased(string, locale());

      if (kCFCompareEqualTo
          != CFStringCompare(string, lowercasedString, kCFCompareNonliteral))
      {
        ioToken.text() = (String)lowercasedString;
      }

      return true;
    }

	// -----------------------------------------------------------------------------	
#pragma mark - Alphanumeric

    static bool isStringMemberOfCharSet(CFStringRef inString,
                                        CFCharacterSetRef inCharSet)
    {
      CFIndex					length = CFStringGetLength(inString);
      CFStringInlineBuffer 	inlineBuffer;

      CFStringInitInlineBuffer(inString, &inlineBuffer,
                               CFRangeMake(0, length));

      for (CFIndex cnt = 0; cnt < length; ++cnt) {
        UniChar ch = CFStringGetCharacterFromInlineBuffer(&inlineBuffer, cnt);
        if (CFCharacterSetIsCharacterMember(inCharSet, ch)) return true;
      }
      return false;
    }

    bool CharSet::getNextToken(Token& ioToken) {
      while(TokenFilter::getNextToken(ioToken)) {
        cf::String string(ioToken.text());
        if (isStringMemberOfCharSet(string, mCharSet)) return true;
      }
      return false;
    }
	
	} // namespace filter
}}

#endif
