//
//  filters.hpp
//
//  Created by Anton Leuski on 11/1/14.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_parsing_filters_hpp__
#define __jerome_ir_parsing_filters_hpp__

namespace jerome {
	
	namespace ir {
		
		
		namespace filter {
			
			/**
			 * convert toke to lowercase
			 */
			class Lowercase : public TokenFilter {
			public:
				explicit Lowercase(TokenStream inSource) : TokenFilter(inSource) {};
				bool getNextToken(Token& ioToken);
			};
			
			/**
			 * stem token using kstem.
			 */
			
			class KStem : public TokenFilter {
			public:
				static void init();
				explicit KStem(TokenStream inSource) : TokenFilter(inSource) {};
				bool getNextToken(Token& ioToken);
			};
			
			/**
			 * expand apostrophes
			 */
			class Apostrophe : public TokenFilter {
				Token	mLastToken;
				bool	mHasToken;
			public:
				Apostrophe(TokenStream inSource) : TokenFilter(inSource), mHasToken(false) {}
				bool getNextToken(Token& ioToken);
			};
			
			/**
			 * explicit dictionary
			 */
			class Dictionary : public TokenFilter {
			public:
				Dictionary(TokenStream inSource, const String& inDictionaryName);
				bool getNextToken(Token& ioToken);
				
				static void init();
				
				static void init(const String& dictionaryName, const char* const words[], const std::size_t wordCount);

				// implementation left as an exercise to the reader ;) ...
				// static void init(const String& dictionaryName, const String& dictionaryFileName);

				static String defaultDictionaryName();

			protected:
				class Storage;
			public:
				typedef shared_ptr<Storage>		dictionary_ptr;
			
			protected:
				Dictionary(TokenStream inSource, const dictionary_ptr& inDictionaryPtr);

			private:
				dictionary_ptr	mDictionary;
			};
			
			/**
			 * n-gram filter
			 */
			class NGram : public TokenFilter {
			private:
				typedef std::list<Token>	Tokens;
				unsigned			mCount;
				Tokens				mTokens;
				int32_t				mIndex;
			public:
				NGram(TokenStream inSource, unsigned inCount = 2);
				bool getNextToken(Token& ioToken);
			};
			
			/**
			 * write tokens to the field index
			 */
			template <class Index>
			class IndexWriter : public TokenFilter {
        Index&      mIndex;
				typename Index::Field&			mField;
				typename Index::Term::size_type mDocumentID;
			public:
				IndexWriter(TokenStream inSource, Index& ioIndex, const String& inFieldName)
				: TokenFilter(inSource)
        , mIndex(ioIndex)
        , mField(ioIndex.findField(inFieldName, true))
        , mDocumentID(0)
        {}
        
				bool getNextToken(Token& ioToken) {
					if (mDocumentID == 0) mDocumentID = mField.addDocument()+1;
					if (!TokenFilter::getNextToken(ioToken)) return false;
					mIndex.addTerm(ioToken, mDocumentID-1, mField);
					return true;
				}
			};
			
			/**
			 * skip all tokens from the given set. The set is not retained.
			 */
			class Stopper : public TokenFilter {
			public:
				typedef Set<String>	Stopwords;
			private:
				const Stopwords&	mStopwords;
			public:
				Stopper(TokenStream inSource, const Stopwords& inStopwords = defaultStopwords())
				: TokenFilter(inSource), mStopwords(inStopwords){}
				bool getNextToken(Token& ioToken);
				
				static const Stopwords& defaultStopwords();
			};
			
		}
		
	}} // namespace jerome::ir

#endif // defined __jerome_ir_parsing_filters_hpp__
