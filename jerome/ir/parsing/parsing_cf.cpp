//
//  parsing_cf.cpp
//
//  Created by Anton Leuski on 4/15/12.
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
