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

namespace jerome { namespace stream {
  namespace stream_detail {
    struct ngram_holder : public stream_filter {
      const unsigned count;
      explicit ngram_holder(unsigned inCount = 2)
      : count(inCount)
      , mShouldSendEOS(false)
      {}
      ngram_holder operator() (unsigned inCount) const {
        return ngram_holder(inCount);
      }

      typedef ir::BasicToken<String> value_type;

      template <class Stream, ASSERT_STREAM(Stream)>
      auto operator() (Stream& inStream) -> value_type
      {
        while (true) {
          if (mShouldSendEOS) {
            mShouldSendEOS = false;
            mTokens.clear();
            return value_type::eos();
          }

          auto token = inStream.next();

          if (token.isBOS()) {
            mTokens.clear();
            mShouldSendEOS = false;
            mTokens.push_back(value_type(value_type::ngramSeparator(), 0, 0));
            return value_type::bos();
          }

          if (token.isEOS()) {
            if (mTokens.back().text() == value_type::ngramSeparator()) {
              mTokens.clear();
              return value_type::eos();
            }
            mTokens.push_back(value_type(value_type::ngramSeparator(),
                                         mTokens.back().end(), 0));
          } else {
            mTokens.push_back(token);
          }

          if (mTokens.size() >= count) {
            break;
          }
        }

        auto newToken = mTokens.front();
        mTokens.pop_front();
        for(const auto& t : mTokens) {
          newToken += t;
        }
        return newToken;
      }
    private:
      typedef ::std::list<value_type>  token_queue_type;
      token_queue_type mTokens;
      bool mShouldSendEOS;
    };

  }
  const auto ngram = stream_detail::ngram_holder();
}}
#endif // defined __jerome_ir_parsing_filter_ngram_hpp
