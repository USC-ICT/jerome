//
//  ngram.hpp
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

#ifndef __jerome_ir_parsing_filter_ngram_hpp
#define __jerome_ir_parsing_filter_ngram_hpp

#include <jerome/types.hpp>
#include <jerome/ir/parsing/token.hpp>

namespace jerome {
  namespace stream {
    namespace stream_detail {
      struct ngram_holder {
        const unsigned count;
        explicit ngram_holder(unsigned inCount = 2)
        : count(inCount)
        {}
        ngram_holder operator() (unsigned inCount) const {
          return ngram_holder(inCount);
        }
      };

      //bool expand_contractions(String& root, String& suffix);

      template <class Stream>
      struct ngram_stream : public stream<
        ngram_stream<Stream>,
        ir::BasicToken<String>
      >
      {
        typedef stream<
          ngram_stream<Stream>,
          ir::BasicToken<String>
        > parent_type;
        typedef typename parent_type::value_type value_type;

        ngram_stream(Stream s, unsigned inCount)
        : mStream(s)
        , mCount(inCount)
        , mIndex(0)
        {
          mTokens.push_back(value_type(value_type::ngramSeparator(), 0, 0));
        }
      private:
        friend parent_type;
        typedef ::std::list<value_type>  Tokens;
        unsigned mCount;
        Tokens mTokens;
        int32_t mIndex;
        Stream mStream;
        optional<value_type> get_next() {
          static unsigned kSize      = 2;

          while (mIndex <= 0) {
            auto result = mStream.next();
            if (!result) {
              if (mTokens.back().text() == value_type::ngramSeparator())
                return optional<value_type>();
              mTokens.push_back(value_type(value_type::ngramSeparator(),
                                           mTokens.back().end(), 0));
            } else {
              mTokens.push_back(*result);
            }
            if (mTokens.size() > mCount) {
              mTokens.pop_front();
            }
            if (mTokens.size() < kSize) {
              auto ioToken = value_type("", INT_MAX);
              for(const auto& t : mTokens) ioToken += t;
              return ioToken;
            }
            mIndex  = (int32_t)mTokens.size() - kSize + 1;
          }
          --mIndex;
          Tokens  tmp;
          auto  j = mTokens.begin();
          for(int32_t i = 0; i < mIndex; ++i, ++j);
          auto ioToken = *j;
          ioToken += mTokens.back();
          return ioToken;
        }
      };
    }
    const auto ngram = stream_detail::ngram_holder();
  }

  template <class Stream>
  inline auto
  operator|(Stream&& r,
            const stream::stream_detail::ngram_holder& f)
  {
    return stream::stream_detail::ngram_stream<
    Stream>(::std::forward<Stream>(r), f.count);
  }
}
#endif // defined __jerome_ir_parsing_filter_ngram_hpp
