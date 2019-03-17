//
//  parsing.cpp
//
//  Created by Anton Leuski on 10/4/11.
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

#include <exception>
//#include <unicode/uchar.h>
#include <jerome/ir/parsing.hpp>
#include <jerome/ir/collection.hpp>
#include "irregular_verbs.hpp"

extern "C" {
	void read_dict_info(void); 
	void stem(char *term, char *stem);
}

namespace jerome { namespace ir {

  // STATIC
	static const String 	kNGramSeparator	= String("_");

	namespace i {
		const jerome::Locale	TokenStreamImpl::kDefaultLocale	= jerome::Locale();

	}
	
	Token& Token::operator+= (const Token& inToken) {
		if (text().size() > 0) {
			text() += kNGramSeparator;
		} else {
			type() = inToken.type();
		}
		text()		+= inToken.text();
		Token::size_type	finish	= std::max(end(), inToken.end());
		offset()	= std::min(offset(), inToken.offset());
		length()	= finish - offset();

		return *this;
	}
	

namespace filter {	

// -----------------------------------------------------------------------------	
#pragma mark - KStem

	static bool kstemInited = false;
	
	void KStem::init() {
		if (kstemInited) return;
		read_dict_info();
		kstemInited = true;
	}
	
	bool KStem::getNextToken(Token& ioToken) {
		if (!TokenFilter::getNextToken(ioToken)) return false;
		
		init();
		
		String::size_type		len	= ioToken.text().length();
		shared_ptr<char>	thestem(new char[2*len]);
			
		stem((char*)ioToken.text().c_str(), thestem.get());
		
		if (strcmp(ioToken.text().c_str(), thestem.get())) {
			
//			std::cout << token->text() << " -> " << thestem.get() << std::endl;
		
			ioToken.text() = thestem.get();
		}
		return true;
	}
	
	// -----------------------------------------------------------------------------	
#pragma mark - Apostrophe
	
	typedef std::vector<std::pair<String, String> >	suffix_list_type;
	static suffix_list_type	sSuffixes;
	static void init_suffixes() {
		const char*	data[] = {
			"'",	"",
			"'s",	"",
			"'ve",	"have",
			"'d",	"would",  
			"'re",	"are",
			"n't",	"not",
			"'m",	"am",
			"'ll",	"will", 
			"'ed",	"",
			NULL
		};
		for(const char** i = data; *i; i += 2) {
			String	pattern(*i);
			sSuffixes.push_back(suffix_list_type::value_type(pattern, *(i+1)));
			replaceAll(pattern, "'", "’");
			sSuffixes.push_back(suffix_list_type::value_type(pattern, *(i+1)));
		}
	}
	
	bool Apostrophe::getNextToken(Token& ioToken) {
		if (mHasToken) {
			ioToken = std::move(mLastToken);
			mHasToken = false;
			return true;
		}
	
		if (!TokenFilter::getNextToken(ioToken)) return false;
		
		if (sSuffixes.size() == 0) init_suffixes();
		
		for(const suffix_list_type::value_type& suffix_pair : sSuffixes) {
			if (!hasSuffix(ioToken.text(), suffix_pair.first)) continue;
			
			ioToken.text() = dropSuffix(ioToken.text(), suffix_pair.first);

			if (suffix_pair.second.length() > 0) {
				mLastToken = ioToken;
				mLastToken.text() = suffix_pair.second;
				mHasToken = true;
			}
			break;
		}
		return true;
	}
	
	// -----------------------------------------------------------------------------	
#pragma mark - Dictionary

	// TODO locale support

	struct const_char_hash {
		typedef const char* argument_type;
		typedef std::size_t result_type;

		result_type operator () (const argument_type& arg) const
		{
			result_type hash = 5381;
			const unsigned char* str = (const unsigned char*)arg;
			int c;

			while ((c = *str++))
				hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

			return hash;
		}
		
	};

	struct const_char_equal {
		typedef bool result_type;
		typedef const char* first_argument_type;
		typedef const char* second_argument_type;
		
    result_type operator () (const first_argument_type& lhs,
                             const second_argument_type& rhs) const
    {
      return strcmp(lhs, rhs) == 0;
    }
	};
	
	static char* loadDataFromFile(const String& inFileName)
	{
		FILE *f = fopen(inFileName.c_str(), "rb");
		if (!f) return nullptr;
		
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		char *string = (char*)malloc(fsize + 1);
		fread(string, fsize, 1, f);
		fclose(f);
		return string;
	}

	class Dictionary::Storage {
	public:
		typedef std::unordered_map<
			const char*,
			const char*,
			const_char_hash,
			const_char_equal
			> map_type;

		Storage()
		: data(nullptr)
		{}

		// we assume the ownership of the pointer. We will modify data in place.
		Storage(char* inData)
		: data(inData)
		{
			throw std::logic_error("unimplemented");
		}

		Storage(const String& fileName)
		: Storage(loadDataFromFile(fileName))
		{
		}
		
		// assuming static storage that we do not own
		Storage(const char* const inWords[], std::size_t inWordCount)
		: data(nullptr)
		{
			for(unsigned i = 0; i < inWordCount-1; i += 2) {
				mMap.emplace(inWords[i], inWords[i+1]);
			}
		}
		
		~Storage()
		{
			delete data;
		}
		
		const map_type& map() const { return mMap; };
		
	private:
		map_type	mMap;
		char* data;
	};

	typedef StringMap<Dictionary::dictionary_ptr> dictionary_cache_type;
  // STATIC
	static dictionary_cache_type	sDictionaryCache;

	static Dictionary::dictionary_ptr dictionaryWithName(const String& inName)
	{
		dictionary_cache_type::const_iterator	cached	= sDictionaryCache.find(inName);
		if (cached != sDictionaryCache.end()) {
			return cached->second;
		}
		return Dictionary::dictionary_ptr();
	}

	Dictionary::Dictionary(TokenStream inSource, const String& inDictionaryName)
	: TokenFilter(inSource)
	, mDictionary(dictionaryWithName(inDictionaryName))
	{}
	
	
//		dictionary_cache_type::const_iterator	cached	= sDictionaryCache.find(inDictionaryFileName);
//		if (cached != sDictionaryCache.end()) {
//			mDictionary	= cached->second;
//			return;
//		}
//		
//		mDictionary	= Dictionary::dictionary_ptr(new Dictionary::dictionary_type);
//		sDictionaryCache.emplace(inDictionaryFileName, mDictionary);
//		
//		FILE*	f	= fopen(inDictionaryFileName.c_str(), "r");
//		if (!f) {
//			std::cerr << "Failed to open " << inDictionaryFileName << std::endl;
//			return;
//		}
//
//		char variant[1024];
//		char root[1024];
//		while (!feof(f))  {
//			fscanf(f, "%s %s", variant, root);
//			mDictionary->emplace(variant, root);
//		}
//		
//		fclose(f);	
//	}
	
	Dictionary::Dictionary(TokenStream inSource, const dictionary_ptr& inDictionaryPtr) 
	: TokenFilter(inSource)
	, mDictionary(inDictionaryPtr)
	{
	
	}

	bool
	Dictionary::getNextToken(Token& ioToken) {
		if (!TokenFilter::getNextToken(ioToken)) return false;
	
		if (mDictionary.get() == nullptr) return true;

		Storage::map_type::const_iterator entry =
      mDictionary->map().find(ioToken.text().c_str());
		if (entry == mDictionary->map().end()) return true;
		
		ioToken.text() = entry->second;
		return true;
	}
	
	void Dictionary::init()
	{
		init(defaultDictionaryName(), irregular_verbs, irregular_verbs_size);
	}
	
  void Dictionary::init(const String& dictionaryName,
                        const char* const words[],
                        const std::size_t wordCount)
  {
		sDictionaryCache.emplace(dictionaryName, std::make_shared<Storage>(words, wordCount));
  }

	String
	Dictionary::defaultDictionaryName() {
		return "irregular_verbs";
	}
	
	// -----------------------------------------------------------------------------	
#pragma mark - NGram


	NGram::NGram(TokenStream inSource, unsigned inCount) 
		:	TokenFilter(inSource), 
			mCount(inCount), 
			mIndex(0) 
	{
		mTokens.push_back(Token(kNGramSeparator, 0, 0));
	}

	
	bool NGram::getNextToken(Token &ioToken) {
		static unsigned kSize			= 2;
		
		while (mIndex <= 0) {
			bool result = TokenFilter::getNextToken(ioToken);
			if (!result) {
				if (mTokens.back().text() == kNGramSeparator)
					return false;
				ioToken = Token(kNGramSeparator, mTokens.back().end(), 0);
			}
			mTokens.push_back(ioToken);
			if (mTokens.size() > mCount) {
				mTokens.pop_front();
			}
			if (mTokens.size() < kSize) {
				ioToken = Token("", INT_MAX);
				for(const Token& t : mTokens) ioToken += t;
				return true;
			}
			mIndex	= (int32_t)mTokens.size() - kSize + 1;
		}
		--mIndex;
		Tokens	tmp;
		Tokens::const_iterator	j = mTokens.begin();
		for(int32_t i = 0; i < mIndex; ++i, ++j);
		ioToken = *j;
		ioToken += mTokens.back();
		return true;
	}
	
	
	// -----------------------------------------------------------------------------	
#pragma mark - Stopper
	
	bool Stopper::getNextToken(Token& ioToken) {
		while (TokenFilter::getNextToken(ioToken)) {
			if (mStopwords.find(ioToken.text()) == mStopwords.end()) {
				return true;
			}
		}
		return false;
	}
  
  static Stopper::Stopwords* makeDefaultStopwords()
  {
    Stopper::Stopwords* stopwords = new Stopper::Stopwords;
    stopwords->emplace("a");
    stopwords->emplace("an");
    stopwords->emplace("the");
    return stopwords;
  }
	
	const Stopper::Stopwords&
	Stopper::defaultStopwords() {
    // Something strange here: if I simply allocate (on global region)
    // the app can crash on quit. The crash happens somewhere in the
    // map<> accesing its elements. So, it looks like the static object
    // is being destroyed before the processing ends. By putting the object
    // on heap, I'll prevent this but leak memory.
    // STATIC
		static Stopper::Stopwords*	kStopwords = makeDefaultStopwords();
		return *kStopwords;
	}
	
}
	
	
}}

