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
#include <jerome/ir/parsing/irregular_verbs.hpp>

namespace jerome { namespace ir { namespace filter {
  namespace filter_detail {
    struct dictionary_holder {
      typedef shared_ptr<std::unordered_map<String, String>> dictionary_type;
      const dictionary_type dictionary;
      dictionary_holder(const dictionary_type& inDictionary)
      : dictionary(inDictionary)
      {}

      typedef ir::BasicToken<String> result_type;
      result_type operator() (const result_type& inToken) const {
        if (inToken.isEOS() || inToken.isBOS()) return inToken;
        auto iter = dictionary->find(inToken.text());
        if (iter == dictionary->end()) return inToken;
        return result_type(iter->second, inToken);
      }
    };
  }
  const filter_detail::dictionary_holder stem_irregular_verbs =
  filter_detail::dictionary_holder(shared_irregular_verbs());
}}}

#endif // defined __jerome_ir_parsing_filter_dictionary_hpp
