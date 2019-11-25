//
//  parsing_std.hpp
//  jerome
//
//  Created by Wei-Wen Feng on ??/??/??.
//  Copyright (c) 2016 USC/ICT. All rights reserved.
//

#ifndef __jerome_ir_parsing_parsing_cpp_hpp__
#define __jerome_ir_parsing_parsing_cpp_hpp__
#ifndef JEROME_IOS

#warning "Please implement locale and unicode support"

#include <locale>
#include <iostream>
#include <boost/tokenizer.hpp>

#if 1
#include <android/log.h>
#define  LOG_TAG    "sbm"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOG_FOOT   LOGI("%s %s %d", __FILE__, __FUNCTION__, __LINE__)
#endif

namespace jerome { 

	class Locale : public std::locale {
	public:
		static void global(const String&  inLocaleString);
	};


	namespace ir {

// -----------------------------------------------------------------------------	
// Note that this tokenizer is different from the Java one, as it tokenizes
// English on '-'.

	class Tokenizer : public i::TokenStreamImpl {
	private:
		typedef boost::tokenizer<boost::char_separator<char> >	index_type;
		
		index_type					mIndex;
		index_type::iterator		mBegin;
		index_type::iterator		mIterator;
		index_type::iterator		mEndIterator;
		const jerome::Locale			mLocale;
		
	public:	

		Tokenizer(const String* inString, jerome::Locale const & inLocale = jerome::Locale()) 	
		:mIndex(*inString,boost::char_separator<char>(", ")),
		 mLocale(inLocale)
		{			
			mBegin = mIndex.begin();
			mIterator = mIndex.begin();
			mEndIterator = mIndex.end();
			
		}
		
		bool getNextToken(Token& ioToken) {
			if (mIterator == mEndIterator) return false;
			index_type::iterator	start	= mIterator;
			++mIterator;
			String tok = *start;
			ioToken = Token( tok , 0, tok.size());
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
//		log::info() << locale().name();
		}
		
		NonTokenizer(const String* inString, jerome::Locale const & inLocale = jerome::Locale()) 
		: mToken(*inString, 0, inString->length())
		, mLocale(inLocale)
		, mHasToken(true)
		{
//		log::info() << locale().name();
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
		 * filter out tokens that do not contain alphanumeric characters, i.e., punctuation
		 */
		class Alphanumeric : public TokenFilter {
		public:
			Alphanumeric(TokenStream inSource) : TokenFilter(inSource) {}
			bool getNextToken(Token& ioToken);
		};
		
		/**
		 * filter out tokens that do not contain alpha characters, i.e., numbers & punctuation
		 */
		class Alpha : public TokenFilter {
		public:
			Alpha(TokenStream inSource) : TokenFilter(inSource) {}
			bool getNextToken(Token& ioToken);
		};


	}


}}

#endif
#endif // __jerome_ir_parsing_parsing_cpp_hpp__
