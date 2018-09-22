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

namespace jerome { namespace ir { namespace filter {
  namespace filter_detail {
    struct is_alpha_holder : public stream::stream_filter {
      const Locale locale;
      is_alpha_holder(const Locale& inLocale = Locale())
      : locale(inLocale)
      {}
      is_alpha_holder operator() (const Locale& inLocale) const {
        return is_alpha_holder(inLocale);
      }

      template <typename T>
      bool operator() (const ir::BasicToken<T>& inToken) const {
        if (inToken.isEOS() || inToken.isBOS()) return true;
        return functor<T>()(inToken.text(), locale);
      }

      template <typename T>
      static auto functor()
      {
        static IsAlpha<typename ir::BasicToken<T>::value_type> funct;
        return funct;
      }
    };
  }
  const filter_detail::is_alpha_holder is_alpha =
  filter_detail::is_alpha_holder();
}}}

namespace jerome { namespace stream {
  const auto is_alpha = jerome::ir::filter::is_alpha;
}}

namespace jerome { namespace ir { namespace filter {
  namespace filter_detail {
    struct is_alphanumeric_holder : public stream::stream_filter {
      const Locale locale;
      is_alphanumeric_holder(const Locale& inLocale = Locale())
      : locale(inLocale)
      {}
      is_alphanumeric_holder operator() (const Locale& inLocale) const {
        return is_alphanumeric_holder(inLocale);
      }

      template <typename T>
      bool operator() (const ir::BasicToken<T>& inToken) const {
        if (inToken.isEOS() || inToken.isBOS()) return true;
        return functor<T>()(inToken.text(), locale);
      }

      template <typename T>
      static auto functor()
      {
        static IsAlphanumeric<typename ir::BasicToken<T>::value_type> funct;
        return funct;
      }
    };
  }
  const filter_detail::is_alphanumeric_holder is_alphanumeric =
  filter_detail::is_alphanumeric_holder();
}}}

namespace jerome { namespace stream {
  const auto is_alphanumeric = jerome::ir::filter::is_alphanumeric;
}}

#endif // defined __jerome_ir_parsing_filter_alpha_hpp
