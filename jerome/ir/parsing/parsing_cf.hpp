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

#include <jerome/ir/parsing/parsing_fwd.hpp>

#if JEROME_PARSING == JEROME_PARSING_CF

#include <jerome/ir/parsing/cf/object.hpp>
#include <jerome/ir/parsing/cf/cf_string.hpp>
#include <jerome/ir/parsing/cf/cf_locale.hpp>
#include <jerome/ir/parsing/cf/cf_tokenizer.hpp>

namespace jerome { 
	namespace ir {

    class Tokenizer : public i::TokenStreamImpl {
      cf::Tokenizer mTokenizer;
    public:
      Tokenizer(const String* inString,
                jerome::Locale const & inLocale = jerome::Locale())
      : mTokenizer(cf::String(*inString), inLocale)
      {}
//      Tokenizer(CFStringRef inString,
//                jerome::Locale const & inLocale = jerome::Locale())
//      : mTokenizer(inString, inLocale)
//      {}
      bool getNextToken(Token& ioToken);
      const jerome::Locale& locale() const { return mTokenizer.locale(); }
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
        CharSet(TokenStream inSource,
                CFCharacterSetRef inCharSet)
        : TokenFilter(inSource)
        , mCharSet(inCharSet)
        {}
        bool getNextToken(Token& ioToken);
      };

      /**
       * filter out tokens that do not contain any alphanumeric characters, i.e., punctuation
       */
      class Alphanumeric : public TokenFilter {
      public:
        Alphanumeric(TokenStream inSource)
        : TokenFilter(inSource)
        {}
        bool getNextToken(Token& ioToken) {
          while(TokenFilter::getNextToken(ioToken)) {
            if (is_alphanumeric(ioToken.text(), locale())) return true;
          }
          return false;
        }

      };

      /**
       * filter out tokens that do not contain any alpha characters, i.e., numbers & punctuation
       */
      class Alpha : public TokenFilter {
      public:
        Alpha(TokenStream inSource)
        : TokenFilter(inSource)
        {}
        bool getNextToken(Token& ioToken) {
          while(TokenFilter::getNextToken(ioToken)) {
            if (is_alpha(ioToken.text(), locale())) return true;
          }
          return false;
        }

      };
    }
  }
}

#endif
#endif // defined __jerome_ir_parsing_parsing_cf_hpp__
