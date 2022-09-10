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

#include <jerome/ir/parsing.hpp>

#ifdef JEROME_IOS

namespace jerome { 

	static String stringFromCFString(CFStringRef inString) {
		const char*	constBuffer	= CFStringGetCStringPtr(inString, kCFStringEncodingUTF8);
		if (constBuffer != NULL) return String(constBuffer);

		CFIndex		length 		= 4*CFStringGetLength(inString);
		char*		buffer		= new char[length];
		if (buffer && CFStringGetCString(inString, buffer, length, kCFStringEncodingUTF8)) {
			String	result(buffer);
			delete[] buffer;
			return result;
		} else {
			String	result;
			delete[] buffer;
			return result;
		}
	}

	namespace detail {
    struct LocaleImpl {
      CFLocaleRef  mLocale;
      LocaleImpl(CFLocaleRef inLocale);
      LocaleImpl(const String& inName);
      ~LocaleImpl();
      CFLocaleRef locale() const { return mLocale; }
    };

    LocaleImpl::LocaleImpl(CFLocaleRef inLocale) {
			mLocale	= inLocale;
      CFRetain(mLocale);
		}
		LocaleImpl::LocaleImpl(const String& inName) {
			CFStringRef	name	= CFStringCreateWithCString(
        kCFAllocatorDefault, inName.c_str(), kCFStringEncodingUTF8);
			mLocale	= CFLocaleCreate(kCFAllocatorDefault, name);
			CFRelease(name);
		}
		LocaleImpl::~LocaleImpl() {
			CFRelease(mLocale);
		}
	}

  static std::shared_ptr<detail::LocaleImpl>* defaultImplementation() {
    static std::shared_ptr<detail::LocaleImpl> singleton;
    if (!singleton) {
      auto locale = CFLocaleCopyCurrent();
      singleton = std::make_shared<detail::LocaleImpl>(locale);
      CFRelease(locale);
    }
    return &singleton;
  }

	Locale::Locale(const String& inLocale)
  : parent_type(std::make_shared<implementation_type>(inLocale))
	{
	}

	Locale::Locale()
  : parent_type(*defaultImplementation())
  {
	}

  CFLocaleRef  Locale::locale() const { return implementation().locale(); }

	void Locale::global(const String& inLocale) {
    *defaultImplementation() = std::make_shared<detail::LocaleImpl>(inLocale);
	}

	namespace ir {

	Tokenizer::Tokenizer(CFStringRef inString, jerome::Locale const & inLocale)
	: mLocale(inLocale)
	{
		CFRetain(inString);
		init(inString);
	} 

	Tokenizer::Tokenizer(const String* inString, jerome::Locale const & inLocale) 
	: mLocale(inLocale)
	{
		init(CFStringCreateWithCString(kCFAllocatorDefault, inString->c_str(),
                                   kCFStringEncodingUTF8));
	}

	Tokenizer::~Tokenizer() {
		CFRelease(mTokenizer);
		CFRelease(mString);
	}

	void 
	Tokenizer::init(CFStringRef inString) {
		mString	= inString;
		mTokenizer = CFStringTokenizerCreate(kCFAllocatorDefault,
                                         mString,
                                         CFRangeMake(0, CFStringGetLength(mString)),
                                         kCFStringTokenizerUnitWordBoundary,
                                         locale().locale());
	}

	bool
	Tokenizer::getNextToken(Token& ioToken) {
		CFStringTokenizerTokenType	tokenType	= CFStringTokenizerAdvanceToNextToken(mTokenizer);
		if (tokenType == kCFStringTokenizerTokenNone) return false;
		
		CFRange		range		= CFStringTokenizerGetCurrentTokenRange(mTokenizer);
		CFStringRef	substring	= CFStringCreateWithSubstring(kCFAllocatorDefault, mString, range);
		ioToken = Token(stringFromCFString(substring),
                    (Token::size_type)range.location, // silence warnings
                    (Token::size_type)range.length);
		CFRelease(substring);
		return true;
	}

	NonTokenizer::NonTokenizer(CFStringRef inString, jerome::Locale const & inLocale)
	: mToken(stringFromCFString(inString), 0, (Token::size_type)CFStringGetLength(inString))
	, mLocale(inLocale)
	, mHasToken(true)
	{
	
	}

	NonTokenizer::NonTokenizer(const String* inString, jerome::Locale const & inLocale) 
	: mToken(*inString, 0, (Token::size_type)inString->length())
	, mLocale(inLocale)
	, mHasToken(true)
	{
//		log::info() << locale().name();
	}

	namespace filter {

// -----------------------------------------------------------------------------	
#pragma mark - Lowercase
	
	bool Lowercase::getNextToken(Token& ioToken) {
		if (!TokenFilter::getNextToken(ioToken)) return false;

		CFStringRef			string 			= CFStringCreateWithCString(kCFAllocatorDefault, ioToken.text().c_str(), kCFStringEncodingUTF8);
		CFMutableStringRef	lowerCaseString = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, string);
		CFStringLowercase(lowerCaseString, locale().locale());
	
		if (kCFCompareEqualTo != CFStringCompare(string, lowerCaseString, kCFCompareNonliteral)) {
			ioToken.text() = stringFromCFString(lowerCaseString);
		}
		
		CFRelease(lowerCaseString);
		CFRelease(string);
		
		return true;
	}

	// -----------------------------------------------------------------------------	
#pragma mark - Alphanumeric

	static bool isStringMemberOfCharSet(CFStringRef inString, CFCharacterSetRef inCharSet) {
		CFIndex					length = CFStringGetLength(inString);
		CFStringInlineBuffer 	inlineBuffer;
		
		CFStringInitInlineBuffer(inString, &inlineBuffer, CFRangeMake(0, length));
		 
		for (CFIndex cnt = 0; cnt < length; ++cnt) {
			 UniChar ch = CFStringGetCharacterFromInlineBuffer(&inlineBuffer, cnt);
			 if (CFCharacterSetIsCharacterMember(inCharSet, ch)) return true;
		}
		return false;
	}

	bool CharSet::getNextToken(Token& ioToken) {
		while(TokenFilter::getNextToken(ioToken)) {
			CFStringRef	string 	= CFStringCreateWithCString(kCFAllocatorDefault, ioToken.text().c_str(), kCFStringEncodingUTF8);
			bool		keep	= isStringMemberOfCharSet(string, mCharSet);
			CFRelease(string);
			if (keep) return true;
		}
		
		return false;
	}
	
	} // namespace filter
}}

#endif
