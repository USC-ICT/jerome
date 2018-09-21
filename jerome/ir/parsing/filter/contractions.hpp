//
//  contractions.hpp
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

#ifndef __jerome_ir_parsing_filter_contractions_hpp
#define __jerome_ir_parsing_filter_contractions_hpp

#include <jerome/types.hpp>
#include <jerome/ir/parsing/token.hpp>
#include <jerome/ir/parsing/filter/filter_range.hpp>

namespace jerome {
  namespace ir {
    namespace filter {
      namespace filter_detail {
        struct contractions_holder {
        };

        bool expand_contractions(String& root, String& suffix);

        template <class T>
        struct contractions_expander {
          typedef BasicToken<String> result_type;

          bool operator() (result_type& ioToken, T& iterator) {
            if (mSavedToken) {
              ioToken = *mSavedToken;
              mSavedToken = boost::none;
              return true;
            }

            ioToken = *iterator++;
            if (ioToken.isEOS()) return false;

            jerome::String suffix;
            if (expand_contractions(ioToken.text(), suffix)) {
              mSavedToken = result_type(suffix, ioToken);
            }

            return true;
          }
        private:
          optional<result_type> mSavedToken;
        };
      }
      const filter_detail::contractions_holder expanded_contractions =
        filter_detail::contractions_holder();
    }
  }

  template <class SinglePassRange>
  inline auto
  operator|(SinglePassRange& r,
            ir::filter::filter_detail::contractions_holder)
  {
    typedef typename boost::range_iterator<SinglePassRange>::type iter_t;
    typedef ir::filter::filter_detail::contractions_expander<iter_t> f_t;
    return filter_range<f_t, SinglePassRange>(f_t(), r);
  }

  template <class SinglePassRange>
  inline auto
  operator|(const SinglePassRange& r,
            ir::filter::filter_detail::contractions_holder)
  {
    typedef typename boost::range_iterator<const SinglePassRange>::type iter_t;
    typedef ir::filter::filter_detail::contractions_expander<iter_t> f_t;
    return filter_range<f_t, const SinglePassRange>(f_t(), r);
  }
}

#endif // defined __jerome_ir_parsing_filter_contractions_hpp
