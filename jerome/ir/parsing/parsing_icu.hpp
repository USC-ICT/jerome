//
//  parsing_icu.hpp
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

#ifndef __jerome_ir_parsing_parsing_icu_hpp__
#define __jerome_ir_parsing_parsing_icu_hpp__
#ifndef JEROME_IOS

//#include "parsing.h"
#include <boost/locale.hpp>

namespace jerome { 

	class Locale : public std::locale {
	public:
		static void global(const String& inLocaleString);
	};


	namespace ir {

// -----------------------------------------------------------------------------	
// Note that this tokenizer is different from the Java one, as it tokenizes
// English on '-'.

	class Tokenizer : public i::TokenStreamImpl {
	private:
		typedef boost::locale::boundary::sboundary_point_index	index_type;
		
		index_type					mIndex;
		index_type::iterator		mBegin;
		index_type::iterator		mIterator;
		index_type::iterator		mEndIterator;
		const jerome::Locale			mLocale;
		
	public:
		template <class CharIterator>
		Tokenizer(CharIterator inBegin, CharIterator inEnd, jerome::Locale const & inLocale = jerome::Locale()) 
		: mIndex(boost::locale::boundary::word, inBegin, inEnd, inLocale)
		, mBegin(mIndex.begin())
		, mIterator(mIndex.begin())
		, mEndIterator(mIndex.end())
		, mLocale(inLocale)
		{
//		std::cout<< locale().name() << std::endl;
		}

		Tokenizer(const String* inString, jerome::Locale const & inLocale = jerome::Locale()) 
		: mIndex(boost::locale::boundary::word, inString->begin(), inString->end(), inLocale)
		, mBegin(mIndex.begin())
		, mIterator(mIndex.begin())
		, mEndIterator(mIndex.end())
		, mLocale(inLocale)
		{
//		std::cout<< locale().name() << std::endl;
		}
		
		bool getNextToken(Token& ioToken) {
			if (mIterator == mEndIterator) return false;
			index_type::iterator	start	= mIterator;
			++mIterator;
			ioToken = Token(String(start->iterator(), mIterator->iterator())
					, (Token::size_type)(start->iterator() - mBegin->iterator())
					, (Token::size_type)(mIterator->iterator() - start->iterator()));
			return true;
		}

		const jerome::Locale& locale() const { return mLocale; }
	};

	class NonTokenizer : public i::TokenStreamImpl {
		const jerome::Locale			mLocale;
		Token						mToken;
		bool						mHasToken;
		
	public:

		// NB this is going to be a problem here -- we actually need the length in CHARACTERS
		// while the current implementation returns the length in bytes
		 
		template <class CharIterator>
		NonTokenizer(CharIterator inBegin, CharIterator inEnd, jerome::Locale const & inLocale = jerome::Locale()) 
		: mToken(String(inBegin, inEnd), 0, inEnd-inBegin)
		, mLocale(inLocale)
		, mHasToken(true)
		{
//		std::cout<< locale().name() << std::endl;
		}
		
		NonTokenizer(const String* inString, jerome::Locale const & inLocale = jerome::Locale()) 
    : mToken(*inString, 0, (Token::size_type)inString->length())
		, mLocale(inLocale)
		, mHasToken(true)
		{
//		std::cout<< locale().name() << std::endl;
		}

		bool getNextToken(Token& ioToken) {
			if (!mHasToken) return false;
			mHasToken	= false;
			ioToken = std::move(mToken);
			return true;
		}

		const jerome::Locale& locale() const { return mLocale; }
	};
	
	namespace filter {

		/**
		 * filter out tokens that do not contain any alphanumeric characters, i.e., punctuation
		 */
		class Alphanumeric : public TokenFilter {
		public:
			Alphanumeric(TokenStream inSource) : TokenFilter(inSource) {}
			bool getNextToken(Token& ioToken);
		};
		
		/**
		 * filter out tokens that do not contain any alpha characters, i.e., numbers & punctuation
		 */
		class Alpha : public TokenFilter {
		public:
			Alpha(TokenStream inSource) : TokenFilter(inSource) {}
			bool getNextToken(Token& ioToken);
		};


	}


}}

#endif
#endif // defined __jerome_ir_parsing_parsing_icu_hpp__
