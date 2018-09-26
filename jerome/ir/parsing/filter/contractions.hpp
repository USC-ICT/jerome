//
//  contractions.hpp
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

#ifndef __jerome_ir_parsing_filter_contractions_hpp
#define __jerome_ir_parsing_filter_contractions_hpp

#include <jerome/types.hpp>
#include <jerome/ir/parsing.hpp>

namespace jerome { namespace stream {
  namespace stream_detail {
    bool expand_contractions(String& root, String& suffix);

    struct contractions_holder : public stream_filter {
      typedef ir::BasicToken<String> value_type;

      value_type mSavedToken;
      bool mHasSavedToken;

      template <class Stream>
      auto operator() (Stream& inStream) -> value_type
      {
        if (mHasSavedToken) {
          mHasSavedToken = false;
          return mSavedToken;
        }
        auto token = inStream.next();
        if (token.isBOS()) return value_type::bos();
        if (token.isEOS()) return value_type::eos();
        value_type stringToken(token);
        jerome::String suffix;
        if (expand_contractions(stringToken.text(), suffix)) {
          mHasSavedToken = true;
          mSavedToken = value_type(suffix, stringToken);
        }
        return stringToken;
      }
    };
  }
  const auto expand_contractions = stream_detail::contractions_holder();
}}

#endif // defined __jerome_ir_parsing_filter_contractions_hpp
