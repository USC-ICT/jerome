//
//  as_token.hpp
//
//  Created by Anton Leuski on 9/20/18.
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

#ifndef __jerome_ir_parsing_filter_as_token_hpp
#define __jerome_ir_parsing_filter_as_token_hpp

namespace jerome {
  namespace stream {

    using Token = ir::BasicToken<String>;

    struct one_token_stream : public stream<one_token_stream, Token> {
      typedef stream<one_token_stream, Token> parent_type;
      one_token_stream(const Locale& inLocale)
      : mToken(Token::unknown())
      , mCount(2)
      {}

      one_token_stream(const jerome::String& inString, const Locale& inLocale)
      : mToken(inString, 0, (Token::size_type)inString.length(),
               Token::Type::word)
      , mCount(0)
      {}

      void setInput(const jerome::String& inString) {
        mToken = Token(inString, 0, (Token::size_type)inString.length(),
                       Token::Type::word);
        mCount = 0;
      }

    private:
      friend parent_type;
      Token mToken;
      int mCount;
      value_type get_next() {
        ++mCount;
        if (mCount == 1) return value_type::bos();
        if (mCount == 2) return mToken;
        return value_type::eos();
      }
    };

    namespace stream_detail {
      struct one_token_locale_holder : public locale_based_filter<one_token_locale_holder> {
      };

      inline auto
      operator << (const one_token_locale_holder& h,
                   const jerome::String& string)
      {
        return jerome::stream::one_token_stream(string, h.locale);
      }

      inline auto
      operator << (const one_token_locale_holder& h,
                jerome::String& string)
      {
        return jerome::stream::one_token_stream(string, h.locale);
      }
    }
    const auto as_token = stream_detail::one_token_locale_holder();
  }
}
#endif // defined __jerome_ir_parsing_filter_as_token_hpp
