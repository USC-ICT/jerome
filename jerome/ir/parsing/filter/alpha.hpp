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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <boost/range.hpp>
#include <boost/range/adaptor/filtered.hpp>
#pragma clang diagnostic pop

namespace jerome { namespace ir { namespace filter {
  const auto filtered = boost::adaptors::filtered;
}}}

namespace jerome {

  template <class T>
  struct IsAlpha<ir::BasicToken<T>>
  {
    typedef bool result_type;
    bool operator() (const ir::BasicToken<T>& inToken,
                     const Locale& inLocale) const
    {
      if (inToken.isEOS() || inToken.isBOS()) return true;
      return functor()(inToken.text(), inLocale);
    }
    static auto functor() -> IsAlpha<typename ir::BasicToken<T>::value_type>
    {
      static IsAlpha<typename ir::BasicToken<T>::value_type> funct;
      return funct;
    }
  };

  namespace ir {
    namespace filter {
      namespace filter_detail {
        struct is_alpha_holder {
          const Locale locale;
          is_alpha_holder(const Locale& inLocale = Locale())
          : locale(inLocale)
          {}
          is_alpha_holder operator() (const Locale& inLocale) const {
            return is_alpha_holder(inLocale);
          }
        };
      }
      const filter_detail::is_alpha_holder filter_alpha =
      filter_detail::is_alpha_holder();
    }
  }

  template <class SinglePassRange>
  inline auto
  operator|(SinglePassRange& r,
            const ir::filter::filter_detail::is_alpha_holder& f)
  {
    typedef typename SinglePassRange::value_type value_t;
    auto binded = std::bind(IsAlpha<value_t>(),
                            std::placeholders::_1, f.locale);
    return r | boost::adaptors::filtered(binded);
  }

  template <class SinglePassRange>
  inline auto
  operator|(const SinglePassRange& r,
            const ir::filter::filter_detail::is_alpha_holder& f)
  {
    typedef typename SinglePassRange::value_type value_t;
    auto binded = std::bind(IsAlpha<value_t>(),
                            std::placeholders::_1, f.locale);
    return r | boost::adaptors::filtered(binded);
  }
}

namespace jerome {

  template <class T>
  struct IsAlphanumeric<ir::BasicToken<T>>
  {
    typedef bool result_type;
    bool operator() (const ir::BasicToken<T>& inToken,
                     const Locale& inLocale) const
    {
      if (inToken.isEOS() || inToken.isBOS()) return true;
      return functor()(inToken.text(), inLocale);
    }
    static auto functor()
      -> IsAlphanumeric<typename ir::BasicToken<T>::value_type>
    {
      static IsAlphanumeric<typename ir::BasicToken<T>::value_type> funct;
      return funct;
    }
  };

  namespace ir {
    namespace filter {


      namespace filter_detail {
        struct is_alphanumeric_holder {
          const Locale locale;
          is_alphanumeric_holder(const Locale& inLocale = Locale())
          : locale(inLocale)
          {}
          is_alphanumeric_holder operator() (const Locale& inLocale) const {
            return is_alphanumeric_holder(inLocale);
          }
        };
      }
      const filter_detail::is_alphanumeric_holder filter_alphanumeric =
      filter_detail::is_alphanumeric_holder();
    }
  }

  template <class SinglePassRange>
  inline auto
  operator|(SinglePassRange& r,
            const ir::filter::filter_detail::is_alphanumeric_holder& f)
  {
    typedef typename SinglePassRange::value_type value_t;
    auto binded = std::bind(IsAlphanumeric<value_t>(),
                            std::placeholders::_1, f.locale);
    return r | boost::adaptors::filtered(binded);
  }

  template <class SinglePassRange>
  inline auto
  operator|(const SinglePassRange& r,
            const ir::filter::filter_detail::is_alphanumeric_holder& f)
  {
    typedef typename SinglePassRange::value_type value_t;
    auto binded = std::bind(IsAlphanumeric<value_t>(),
                            std::placeholders::_1, f.locale);
    return r | boost::adaptors::filtered(binded);
  }
}

#endif // defined __jerome_ir_parsing_filter_alpha_hpp
