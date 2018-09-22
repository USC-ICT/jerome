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
    namespace stream_detail {
      struct one_token_locale_holder {
        const Locale locale;
        one_token_locale_holder(const Locale& inLocale = Locale())
        : locale(inLocale)
        {}
        one_token_locale_holder operator() (const Locale& inLocale) const {
          return one_token_locale_holder(inLocale);
        }
      };
    }

    const auto as_token = stream_detail::one_token_locale_holder();

    using Token = ir::BasicToken<String>;

    struct one_token_stream : public stream<one_token_stream, Token> {
      typedef stream<one_token_stream, Token> parent_type;
      one_token_stream(const jerome::String& inString, const Locale& inLocale)
      : mToken(inString, 0, (Token::size_type)inString.length())
      , mHasToken(true)
      {}
    private:
      friend parent_type;
      Token mToken;
      bool mHasToken;
      optional<value_type> get_next() {
        if (mHasToken) {
          mHasToken = false;
          return mToken;
        } else {
          return optional<value_type>();
        }
      }
    };
  }

  inline stream::one_token_stream
  operator|(const jerome::String& string,
            const stream::stream_detail::one_token_locale_holder& h)
  {
    return stream::one_token_stream(string, h.locale);
  }

  inline stream::one_token_stream
  operator|(jerome::String& string,
            const stream::stream_detail::one_token_locale_holder& h)
  {
    return stream::one_token_stream(string, h.locale);
  }
}
#endif // defined __jerome_ir_parsing_filter_as_token_hpp
