//
//  lowercased.hpp
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

#ifndef __jerome_ir_parsing_filter_lowercased_hpp
#define __jerome_ir_parsing_filter_lowercased_hpp

#include <jerome/ir/parsing/parsing_fwd.hpp>

namespace jerome { namespace stream {
  namespace stream_detail {
    struct lowercased_holder : public locale_based_filter<lowercased_holder> {

      template <class Stream, ASSERT_STREAM(Stream)>
      auto operator() (Stream& inStream)
      {
        typedef typename std::decay<Stream>::type Stream_t;
        typedef typename Stream_t::value_type value_type;
        auto token = inStream.next();
        if (token.isEOS() || token.isBOS()) return token;
        auto text = lowercased(token.text(), locale);
        if (text == token.text()) return token;
        return value_type(text, token);
      }
    };

  }

  const auto lowercase = stream_detail::lowercased_holder();
}}


#endif // defined __jerome_ir_parsing_filter_lowercased_hpp
