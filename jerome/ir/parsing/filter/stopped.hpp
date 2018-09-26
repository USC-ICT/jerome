//
//  stopped.hpp
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

#ifndef __jerome_ir_parsing_filter_stopped_hpp
#define __jerome_ir_parsing_filter_stopped_hpp

#include <jerome/types.hpp>
#include <jerome/ir/parsing/parsing_fwd.hpp>
#include <jerome/ir/parsing/token.hpp>
#include <jerome/ir/parsing/stream.hpp>

namespace jerome { namespace stream {
  namespace stream_detail {
    template <typename S>
    struct not_in_set_holder : public conditional_filter<not_in_set_holder<S>> {
      typedef S element_type;
      typedef shared_ptr<::std::unordered_set<element_type>> set_type;
      const set_type set;
      not_in_set_holder(const set_type& inSet)
      : set(inSet)
      {}
      not_in_set_holder operator() (const set_type& inSet) const {
        return not_in_set_holder(inSet);
      }

      using conditional_filter<not_in_set_holder<S>>::operator ();
      bool operator() (const ir::BasicToken<element_type>& inToken) const {
        if (inToken.isEOS() || inToken.isBOS()) return true;
        return set->find(S(inToken.text())) == set->end();
      }
    };

    shared_ptr<::std::unordered_set<String>> defaultStopwords();
  }
  const auto remove_stopwords =
  stream_detail::not_in_set_holder<String>(stream_detail::defaultStopwords());
}}

#endif // defined __jerome_ir_parsing_filter_stopped_hpp
