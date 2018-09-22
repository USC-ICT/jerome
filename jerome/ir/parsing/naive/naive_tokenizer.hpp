//
//  std_tokenizer.hpp
//
//  Created by Anton Leuski on 9/21/18.
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

#ifndef std_tokenizer_hpp
#define std_tokenizer_hpp

#include <jerome/ir/parsing/parsing_fwd.hpp>
#include <jerome/ir/parsing/locale.hpp>

#if JEROME_PARSING == JEROME_PARSING_STD

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <boost/locale.hpp>
#pragma clang diagnostic pop

namespace jerome {
  namespace naive {
    using namespace stream;

    typedef ir::BasicToken<String>  Token;

    struct tokenized_stream : public stream<tokenized_stream, Token> {
      typedef stream<tokenized_stream, Token> parent_type;
      typedef typename parent_type::value_type value_type;
      tokenized_stream(const jerome::String& inString, const Locale& inLocale)
      : mIndex(inString,boost::char_separator<char>(", "))
      , mBegin(mIndex.begin())
      , mIterator(mIndex.begin())
      , mEndIterator(mIndex.end())
      , mLocale(inLocale)
      {}
    private:
      friend parent_type;

      typedef boost::tokenizer<boost::char_separator<char> >  index_type;

      index_type            mIndex;
      index_type::iterator  mBegin;
      index_type::iterator  mIterator;
      index_type::iterator  mEndIterator;
      const jerome::Locale  mLocale;

      optional<value_type> get_next() {
        if (mIterator == mEndIterator) return optional<value_type>();
        index_type::iterator  start  = mIterator;
        ++mIterator;
        return value_type(*start, 0, (Token::size_type)start->length());
      }
    };

  }

  using tokenized_stream = naive::tokenized_stream;
}

#endif

#endif // defined std_tokenizer_hpp
