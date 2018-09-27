//
//  cf_tokenizer.hpp
//
//  Created by Anton Leuski on 9/19/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_parsing_cf_tokenizer_hpp
#define __jerome_ir_parsing_cf_tokenizer_hpp

#include <jerome/ir/parsing/parsing_fwd.hpp>

#if JEROME_PARSING == JEROME_PARSING_CF

#include <jerome/ir/parsing/cf/object.hpp>
#include <jerome/ir/parsing/cf/cf_string.hpp>
#include <jerome/ir/parsing/cf/cf_locale.hpp>
#include <jerome/ir/parsing/stream.hpp>
#include <jerome/ir/parsing/token.hpp>

namespace jerome { namespace cf {
  struct Tokenizer {
    explicit Tokenizer(Locale const & inLocale = Locale());

    template <typename S>
    Tokenizer(S&& inString, Locale const & inLocale = Locale())
    : mLocale(inLocale)
    , mString(::std::forward<S>(inString))
    , mTokenizer()
    {}

    const Locale& locale() const { return mLocale; }

    template <class token_type>
    token_type nextToken() {
      if (!mTokenizer) {
        init();
        return mTokenizer ? token_type::bos() : token_type::eos();
      }
      auto  tokenType  = CFStringTokenizerAdvanceToNextToken(mTokenizer);
      if (tokenType == kCFStringTokenizerTokenNone) return token_type::eos();

      CFRange    range    = CFStringTokenizerGetCurrentTokenRange(mTokenizer);
      cf::String  substring(mString.substr(range.location, range.length));
      return token_type((typename token_type::value_type)substring,
                        (typename token_type::size_type)range.location,
                        (typename token_type::size_type)range.length,
                        token_type::Type::word);
    }

    void setString(const String& inString);

  private:
    typedef basic_object<CFStringTokenizerRef> tokenizer_type;
    Locale mLocale;
    String mString;
    tokenizer_type  mTokenizer;

    void init();
  };

  typedef ir::BasicToken<String> Token;

  using namespace stream;

  struct tokenized_stream : public stream<tokenized_stream, Token> {
    typedef stream<tokenized_stream, Token> parent_type;

    tokenized_stream(const Locale& inLocale)
    : mTokenizer(inLocale)
    {}

    template <typename S>
    tokenized_stream(S&& inString, const Locale& inLocale)
    : mTokenizer(::std::forward<S>(inString), inLocale)
    {}

    template <typename S>
    void setInput(S&& inString) {
      mTokenizer.setString(::std::forward<S>(inString));
    }

  private:
    friend parent_type;
    Tokenizer mTokenizer;
    value_type get_next() {
      return mTokenizer.nextToken<Token>();
    }
  };
}}

namespace jerome {
  using tokenized_stream = cf::tokenized_stream;
}

#endif

#endif /* __jerome_ir_parsing_cf_tokenizer_hpp */
