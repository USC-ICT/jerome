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

#include <jerome/ir/parsing.hpp>

#ifdef JEROME_IOS

namespace jerome { 

  namespace cf {
    String::String(const jerome::String& inString)
    : parent_type(CFStringCreateWithCString(kCFAllocatorDefault,
                                            inString.c_str(),
                                            kCFStringEncodingUTF8))
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
  }

  static Locale	kDefaultLocale(Locale::move(CFLocaleCopyCurrent()));

	Locale::Locale(const String& inLocale)
  : parent_type(Locale::move(CFLocaleCreate(kCFAllocatorDefault, cf::String(inLocale))))
	{
	}

	Locale::Locale()
  : parent_type(kDefaultLocale)
  {
	}

	void Locale::global(const String& inLocale) {
		kDefaultLocale = Locale(inLocale);
	}

	namespace ir {

    Tokenizer::Tokenizer(CFStringRef inString,
                         jerome::Locale const & inLocale)
    : mLocale(inLocale)
    , mString(inString)
    , mTokenizer(init(mString, mLocale))
    {
    }

    Tokenizer::Tokenizer(const String* inString, jerome::Locale const & inLocale)
    : mLocale(inLocale)
    , mString(*inString)
    , mTokenizer(init(mString, mLocale))
    {
    }

    cf::basic_object<CFStringTokenizerRef>
    Tokenizer::init(CFStringRef inString, CFLocaleRef inLocale) {
      auto tokenzer = CFStringTokenizerCreate(kCFAllocatorDefault,
                                              inString,
                                              CFRangeMake(0, CFStringGetLength(inString)),
                                              kCFStringTokenizerUnitWordBoundary,
                                              inLocale);
      return cf::basic_object<CFStringTokenizerRef>::move(tokenzer);
    }

    bool
    Tokenizer::getNextToken(Token& ioToken) {
      auto	tokenType	= CFStringTokenizerAdvanceToNextToken(mTokenizer);
      if (tokenType == kCFStringTokenizerTokenNone) return false;

      CFRange		range		= CFStringTokenizerGetCurrentTokenRange(mTokenizer);
      cf::String	substring(cf::String::move(CFStringCreateWithSubstring(kCFAllocatorDefault, mString, range)));
      ioToken = Token((String)substring,
                      (Token::size_type)range.location, // silence warnings
                      (Token::size_type)range.length);
      return true;
    }

    NonTokenizer::NonTokenizer(CFStringRef inString,
                               jerome::Locale const & inLocale)
    : mToken((String)cf::String(inString), 0, (Token::size_type)CFStringGetLength(inString))
    , mLocale(inLocale)
    , mHasToken(true)
    {

    }

    NonTokenizer::NonTokenizer(const String* inString,
                               jerome::Locale const & inLocale)
    : mToken(*inString, 0, (Token::size_type)inString->length())
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
      CFMutableStringRef	lowerCaseString =
      CFStringCreateMutableCopy(kCFAllocatorDefault, 0, string);
      CFStringLowercase(lowerCaseString, locale());
      cf::String   lowercased(cf::String::move(lowerCaseString));

      if (kCFCompareEqualTo
          != CFStringCompare(string, lowercased, kCFCompareNonliteral))
      {
        ioToken.text() = (String)lowercased;
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
