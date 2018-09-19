//
//  filters.hpp
//
//  Created by Anton Leuski on 11/1/14.
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

#ifndef __jerome_ir_parsing_filters_hpp__
#define __jerome_ir_parsing_filters_hpp__

namespace jerome {

//  namespace stream {
//    template <class Derived, typename Result, Result DF>
//    struct Filter {
//      typedef Result result_type;
//    private:
//      struct HolderBase {
//        virtual ~HolderBase() {}
//        virtual result_type process(Filter* ioFilter) = 0;
//      };
//
//      template <class Source>
//      struct Holder : public HolderBase {
//        Holder(Source& inSource)
//        : mSource(inSource)
//        {}
//        result_type process(Filter* ioFilter) override {
//          return ioFilter->process(this->mSource);
//        }
//      private:
//        Source& mSource;
//      };
//
//      std::unique_ptr<HolderBase> mHolder;
//
//      template <class Source>
//      result_type process(Source& ioSource) {
//        return static_cast<Derived*>(this)->process(ioSource);
//      }
//    public:
//
//      result_type eos() const { return DF; }
//
//      template <class Source>
//      void setSource(Source& inSource) {
//        mHolder = std::unique_ptr<HolderBase>(new Holder<Source>(inSource));
//      }
//
//      result_type next() {
//        return mHolder ? mHolder->process(this) : eos();
//      }
//
//      void drain() {
//        while (eos() != next());
//      }
//    };
//
//    template <class First, class Second>
//    struct Combined {
//      typedef typename Second::result_type result_type;
//
//      Combined(First&& inFirst, Second&& inSecond)
//      : mFirst(std::move(inFirst))
//      , mSecond(std::move(inSecond))
//      {
//        mSecond.setSource(mFirst);
//      }
//      
//      result_type next() {
//        return mSecond.next();
//      }
//
//    private:
//      First mFirst;
//      Second mSecond;
//    };
//
//    struct Concrete : public Filter<Concrete, int, 0> {
//      typedef Filter<Concrete, int, 0> parent_type;
//      typedef parent_type::result_type result_type;
//      Concrete()
//      : count(5)
//      {}
//
//      template <class Source>
//      result_type process(Source& ioSource) {
//        return count > 0 ? --count : ioSource.next();
//      }
//
//    private:
//      int count;
//    };
//  }

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
				typename Index::field_type&			mField;
				typename Index::term_type::size_type mDocumentID;
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
