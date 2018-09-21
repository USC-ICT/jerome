//
//  dictionary.hpp
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

#ifndef __jerome_ir_parsing_filter_dictionary_hpp
#define __jerome_ir_parsing_filter_dictionary_hpp

#include <jerome/ir/parsing/parsing_fwd.hpp>

namespace jerome {

  struct DictionaryMapper
  {
    typedef shared_ptr<std::unordered_map<String, String>> dictionary_type;

    DictionaryMapper(const dictionary_type& inDictionary)
    : mDictionary(inDictionary)
    {}

    DictionaryMapper(const DictionaryMapper&) = default;
    DictionaryMapper(DictionaryMapper&&) = default;

    typedef ir::BasicToken<String> result_type;
    result_type operator() (const result_type& inToken) const {
      if (inToken.isEOS() || inToken.isBOS()) return inToken;
      auto iter = mDictionary->find(inToken.text());
      if (iter == mDictionary->end()) return inToken;
      return result_type(iter->second, inToken);
    }

  private:
    shared_ptr<std::unordered_map<String, String>> mDictionary;
  };

//  namespace ir {
//    namespace filter {
//      namespace filter_detail {
//        struct lowercased_holder {
//          const Locale locale;
//          lowercased_holder(const Locale& inLocale = Locale())
//          : locale(inLocale)
//          {}
//          lowercased_holder operator() (const Locale& inLocale) const {
//            return lowercased_holder(inLocale);
//          }
//        };
//      }
//      const filter_detail::lowercased_holder lowercased =
//      filter_detail::lowercased_holder();
//    }
//  }
//
//  template <class SinglePassRange>
//  inline auto
//  operator|(SinglePassRange& r,
//            const ir::filter::filter_detail::lowercased_holder& f)
//  {
//    typedef typename SinglePassRange::value_type value_t;
//    auto binded = std::bind(Lowercased<value_t>(),
//                            std::placeholders::_1, f.locale);
//    return boost::transformed_range<
//    decltype(binded),
//    SinglePassRange
//    >(binded, r);
//  }
//
//  template <class SinglePassRange>
//  inline auto
//  operator|(const SinglePassRange& r,
//            const ir::filter::filter_detail::lowercased_holder& f)
//  {
//    typedef typename SinglePassRange::value_type value_t;
//    auto binded = std::bind(Lowercased<value_t>(),
//                            std::placeholders::_1, f.locale);
//    return boost::transformed_range<
//    decltype(binded),
//    const SinglePassRange
//    >(binded, r);
//  }
}

#endif // defined __jerome_ir_parsing_filter_dictionary_hpp
