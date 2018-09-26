//
//  kstem.hpp
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

#ifndef __jerome_ir_parsing_filter_kstem_hpp
#define __jerome_ir_parsing_filter_kstem_hpp

#include <jerome/types.hpp>
#include <jerome/ir/parsing/parsing_fwd.hpp>
#include <jerome/ir/parsing/token.hpp>
#include <jerome/ir/parsing/stream.hpp>

namespace jerome { namespace stream {
  namespace stream_detail {
    struct kstem_holder : public stream_filter {
      typedef ir::BasicToken<String> value_type;

      template <class Stream, ASSERT_STREAM(Stream)>
      auto operator() (Stream& inStream) -> value_type
      {
        auto token = inStream.next();
        if (token.isBOS()) return value_type::bos();
        if (token.isEOS()) return value_type::eos();
        value_type stringToken(token);
        auto length = stringToken.text().length();
        ::std::unique_ptr<char[]>  thestem(new char[2*length]);
        stem(stringToken.text().c_str(), thestem.get());
        if (!strcmp(stringToken.text().c_str(), thestem.get())) return stringToken;
        return value_type(String(thestem.get()), stringToken);
      }
    private:
      static void stem(const char* inString, char* outStem);
    };
  }
  const auto kstem = stream_detail::kstem_holder();
}}

#endif // defined __jerome_ir_parsing_filter_kstem_hpp
