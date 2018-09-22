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

namespace jerome {

  template <class T>
  struct Lowercased<ir::BasicToken<T>>
  {
    typedef ir::BasicToken<T> result_type;
    ir::BasicToken<T> operator() (const ir::BasicToken<T>& inToken,
                                  const Locale& inLocale) const {
      if (inToken.isEOS() || inToken.isBOS()) return inToken;
      auto text = functor()(inToken.text(), inLocale);
      if (text == inToken.text()) return inToken;
      return ir::BasicToken<T>(text, inToken);
    }
    static auto functor() -> Lowercased<typename ir::BasicToken<T>::value_type>
    {
      static Lowercased<typename ir::BasicToken<T>::value_type> funct;
      return funct;
    }
  };

  namespace filter_detail {
    struct lowercased_holder {
      const Locale locale;
      lowercased_holder(const Locale& inLocale = Locale())
      : locale(inLocale)
      {}
      lowercased_holder operator() (const Locale& inLocale) const {
        return lowercased_holder(inLocale);
      }
    };
  }

  namespace stream {
    const filter_detail::lowercased_holder lowercased =
    filter_detail::lowercased_holder();
  }

  template <class SinglePassRange>
  inline auto
  operator|(SinglePassRange&& r,
            const filter_detail::lowercased_holder& f)
  {
    typedef typename SinglePassRange::value_type value_t;
    auto binded = ::std::bind(Lowercased<value_t>(),
                              ::std::placeholders::_1, f.locale);
    return stream::transformed_stream<
      decltype(binded),
      SinglePassRange
    >(binded, ::std::forward<SinglePassRange>(r));
  }
}


#endif // defined __jerome_ir_parsing_filter_lowercased_hpp
