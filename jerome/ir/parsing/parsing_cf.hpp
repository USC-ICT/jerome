//
//  parsing_cf.hpp
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

#ifndef __jerome_ir_parsing_parsing_cf_hpp__
#define __jerome_ir_parsing_parsing_cf_hpp__
#ifdef JEROME_IOS

#include <CoreFoundation/CoreFoundation.h>
#include <jerome/types.hpp>

namespace jerome { 

  namespace cf {
    template <typename T>
    struct basic_object {
      explicit basic_object(T inValue)
      : mValue(inValue)
      { retain(); }
      basic_object(const basic_object& inOther)
      : mValue(inOther.mValue)
      { retain(); }
      basic_object(basic_object&& inOther)
      : mValue(inOther.mValue)
      { inOther.mValue = NULL; }
      basic_object& operator = (const basic_object& inOther)
      {
        if (mValue != inOther.mValue) {
          release();
          mValue = inOther.mValue;
          retain();
        }
        return *this;
      }
      basic_object& operator = (basic_object&& inOther)
      {
        release();
        if (mValue != inOther.mValue) {
          mValue = inOther.mValue;
        }
        inOther.mValue = NULL;
        return *this;
      }
      ~basic_object() { release(); }
      operator T() const { return mValue; }
      static basic_object move(T inRef) {
        // we accept ref with +1
        basic_object result(inRef); // increment it
        result.release(); // decrement it, to release the inRef.
        return result;
      }
    protected:
      T value() const { return mValue; }
      auto retain() { if (mValue) CFRetain(mValue); }
      auto release() { if (mValue) CFRelease(mValue); }
    private:
      T mValue;
    };

    struct String : public basic_object<CFStringRef> {
      typedef basic_object<CFStringRef> parent_type;
      using parent_type::parent_type;
      String(const jerome::String& inString);
      explicit operator jerome::String () const;
    };
  }

  struct Locale : public cf::basic_object<CFLocaleRef> {
    typedef cf::basic_object<CFLocaleRef> parent_type;
    using parent_type::parent_type;
    static void global(const String& inLocaleString);
    Locale();
    Locale(const jerome::String& inString);
  };

	namespace ir {

    class Tokenizer : public i::TokenStreamImpl {
    private:
      const jerome::Locale	mLocale;
      cf::String					  mString;
      cf::basic_object<CFStringTokenizerRef>	mTokenizer;

      static cf::basic_object<CFStringTokenizerRef>
      init(CFStringRef inString, CFLocaleRef inLocale);

    public:

      Tokenizer(const String* inString, jerome::Locale const & inLocale = jerome::Locale());
      Tokenizer(CFStringRef inString, jerome::Locale const & inLocale = jerome::Locale());
      bool getNextToken(Token& ioToken);
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
      class Alphanumeric : public CharSet {
      public:
        Alphanumeric(TokenStream inSource)
        : CharSet(inSource, CFCharacterSetGetPredefined(kCFCharacterSetAlphaNumeric))
        {}
      };

      /**
       * filter out tokens that do not contain any alpha characters, i.e., numbers & punctuation
       */
      class Alpha : public CharSet {
      public:
        Alpha(TokenStream inSource)
        : CharSet(inSource, CFCharacterSetGetPredefined(kCFCharacterSetLetter))
        {}
      };
    }
  }
}

#endif
#endif // defined __jerome_ir_parsing_parsing_cf_hpp__
