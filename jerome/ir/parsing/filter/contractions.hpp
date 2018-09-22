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

namespace jerome {
  namespace stream {
    namespace stream_detail {
      struct contractions_holder {
      };

      bool expand_contractions(String& root, String& suffix);

      template <class Stream>
      struct expand_contractions_stream : public stream<
        expand_contractions_stream<Stream>,
        ir::BasicToken<String>
      >
      {
        typedef stream<
        expand_contractions_stream<Stream>,
        ir::BasicToken<String>
        > parent_type;
        typedef typename parent_type::value_type value_type;

        expand_contractions_stream(Stream s)
        : mStream(s)
        {}
      private:
        friend parent_type;
        value_type mSavedToken;
        bool mHasSavedToken;
        Stream mStream;
        optional<value_type> get_next() {
          if (mHasSavedToken) {
            mHasSavedToken = false;
            return mSavedToken;
          }
          auto x = mStream.next();
          if (!x) return optional<value_type>();
          value_type token(*x);
          jerome::String suffix;
          if (expand_contractions(token.text(), suffix)) {
            mHasSavedToken = true;
            mSavedToken = value_type(suffix, token);
          }
          return token;
        }
      };
    }
    const auto expanded_contractions = stream_detail::contractions_holder();
  }

  template <class Stream>
  inline auto
  operator|(Stream&& r,
            stream::stream_detail::contractions_holder)
  {
    return stream::stream_detail::expand_contractions_stream<
      Stream>(std::forward<Stream>(r));
  }
}

#endif // defined __jerome_ir_parsing_filter_contractions_hpp
