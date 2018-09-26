//
//  tokenized.hpp
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

#ifndef __jerome_ir_parsing_filter_tokenized_hpp
#define __jerome_ir_parsing_filter_tokenized_hpp

namespace jerome { namespace stream {
  namespace stream_detail {
    struct tokenized_locale_holder : public locale_based_filter<tokenized_locale_holder>  {
    };

    inline jerome::tokenized_stream
    operator|(const jerome::String& string,
              const tokenized_locale_holder& h)
    {
      return tokenized_stream(string, h.locale);
    }

    inline jerome::tokenized_stream
    operator|(jerome::String& string,
              const tokenized_locale_holder& h)
    {
      return tokenized_stream(string, h.locale);
    }
  }

  const auto tokenized = stream_detail::tokenized_locale_holder();
}}

#endif // defined __jerome_ir_parsing_filter_tokenized_hpp
