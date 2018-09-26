//
//  alpha.hpp
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

#ifndef __jerome_ir_parsing_filter_alpha_hpp
#define __jerome_ir_parsing_filter_alpha_hpp

#include <jerome/ir/parsing/parsing_fwd.hpp>

namespace jerome { namespace stream {
  namespace stream_detail {
    struct is_alpha_holder :
      public locale_based_filter<is_alpha_holder>,
      public conditional_filter<is_alpha_holder>
    {
      using conditional_filter<is_alpha_holder>::operator ();
      template <typename T>
      bool operator() (const ir::BasicToken<T>& inToken) const {
        if (inToken.isEOS() || inToken.isBOS()) return true;
        return is_alpha(inToken.text(), locale);
      }
    };
  }
  const auto filter_alpha = stream_detail::is_alpha_holder();
}}

namespace jerome { namespace stream {
  namespace stream_detail {
    struct is_alphanumeric_holder :
      public locale_based_filter<is_alphanumeric_holder>,
      public conditional_filter<is_alphanumeric_holder>
    {
      using conditional_filter<is_alphanumeric_holder>::operator ();
      template <typename T>
      bool operator() (const ir::BasicToken<T>& inToken) const {
        if (inToken.isEOS() || inToken.isBOS()) return true;
        return is_alphanumeric(inToken.text(), locale);
      }
    };
  }
  const auto filter_alphanumeric = stream_detail::is_alphanumeric_holder();
}}

#endif // defined __jerome_ir_parsing_filter_alpha_hpp
