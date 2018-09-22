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
      typedef ir::BasicToken<String> result_type;
      result_type operator() (const result_type& inToken) const {
        if (inToken.isEOS() || inToken.isBOS()) return inToken;
        auto length = inToken.text().length();
        ::std::unique_ptr<char[]>  thestem(new char[2*length]);
        stem(inToken.text().c_str(), thestem.get());
        if (!strcmp(inToken.text().c_str(), thestem.get())) return inToken;
        return result_type(String(thestem.get()), inToken);
      }
    private:
      static void stem(const char* inString, char* outStem);
    };
  }
  const auto kstem = stream_detail::kstem_holder();
}}

#endif // defined __jerome_ir_parsing_filter_kstem_hpp
