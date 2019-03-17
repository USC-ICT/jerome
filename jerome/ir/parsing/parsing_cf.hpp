//
//  parsing_cf.hpp
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

#ifndef __jerome_ir_parsing_parsing_cf_hpp__
#define __jerome_ir_parsing_parsing_cf_hpp__
#ifdef JEROME_IOS

#include <CoreFoundation/CoreFoundation.h>
#include <jerome/types.hpp>

namespace jerome { 

	namespace detail {
		class LocaleImpl {
			CFLocaleRef	mLocale;
		public:
			LocaleImpl(CFLocaleRef inLocale);
			LocaleImpl(const String& inName);
			~LocaleImpl();
			CFLocaleRef locale() const { return mLocale; }
		};
	}

	class Locale : public ReferenceClassInterface<detail::LocaleImpl> {
	public:
		typedef ReferenceClassInterface<detail::LocaleImpl> parent_type;
		static void global(const String& inLocaleString);
		CFLocaleRef	locale() const { return implementation().locale(); }
		Locale();
		Locale(const String& inLocaleString);
		Locale(CFLocaleRef inLocale);
	};

	namespace ir {

	class Tokenizer : public i::TokenStreamImpl {
	private:
		const jerome::Locale			mLocale;
		CFStringRef					mString;
		CFStringTokenizerRef		mTokenizer;
		
		void init(CFStringRef inString);
		 
	public:

		Tokenizer(const String* inString, jerome::Locale const & inLocale = jerome::Locale()); 
		Tokenizer(CFStringRef inString, jerome::Locale const & inLocale = jerome::Locale()); 
		bool getNextToken(Token& ioToken);
		
		~Tokenizer();

		const jerome::Locale& locale() const { return mLocale; }
	};

	class NonTokenizer : public i::TokenStreamImpl {
	private:
		const jerome::Locale			mLocale;
		Token						mToken;
		bool						mHasToken;
		
	public:
		
		NonTokenizer(const String* inString, jerome::Locale const & inLocale = jerome::Locale()); 
		NonTokenizer(CFStringRef inString, jerome::Locale const & inLocale = jerome::Locale());
		
		bool getNextToken(Token& ioToken) {
			if (!mHasToken) return false;
			ioToken = std::move(mToken);
			mHasToken = false;
			return true;
		}

		const jerome::Locale& locale() const { return mLocale; }
	};
	
	namespace filter {

		/**
		 * filter out tokens that do not contain any characters from the given set, i.e., punctuation
		 */
		class CharSet : public TokenFilter {
			CFCharacterSetRef	mCharSet;
		public:
			CharSet(TokenStream inSource, CFCharacterSetRef inCharSet) : TokenFilter(inSource), mCharSet(inCharSet) {}
			bool getNextToken(Token& ioToken);
		};

		/**
		 * filter out tokens that do not contain any alphanumeric characters, i.e., punctuation
		 */
		class Alphanumeric : public CharSet {
		public:
			Alphanumeric(TokenStream inSource) : CharSet(inSource, CFCharacterSetGetPredefined(kCFCharacterSetAlphaNumeric)) {}
		};
		
		/**
		 * filter out tokens that do not contain any alpha characters, i.e., numbers & punctuation
		 */
		class Alpha : public CharSet {
		public:
			Alpha(TokenStream inSource) : CharSet(inSource, CFCharacterSetGetPredefined(kCFCharacterSetLetter)) {}
		};
	}
}}

#endif
#endif // defined __jerome_ir_parsing_parsing_cf_hpp__
