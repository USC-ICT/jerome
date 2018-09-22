//
//  contractions.cpp
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

#include "contractions.hpp"

namespace jerome { namespace stream { namespace stream_detail {

  typedef ::std::vector<::std::pair<String, String> >  suffix_list_type;
  static suffix_list_type  sSuffixes;
  static void init_suffixes() {
    const char*  data[] = {
      "'",  "",
      "'s",  "",
      "'ve",  "have",
      "'d",  "would",
      "'re",  "are",
      "n't",  "not",
      "'m",  "am",
      "'ll",  "will",
      "'ed",  "",
      NULL
    };
    for(const char** i = data; *i; i += 2) {
      String  pattern(*i);
      sSuffixes.push_back(suffix_list_type::value_type(pattern, *(i+1)));
      replaceAll(pattern, "'", "’");
      sSuffixes.push_back(suffix_list_type::value_type(pattern, *(i+1)));
    }
  }

  bool expand_contractions(String& root, String& suffix)
  {
    if (sSuffixes.size() == 0) init_suffixes();

    for(const auto& suffix_pair : sSuffixes) {
      if (!hasSuffix(root, suffix_pair.first)) continue;

      root = dropSuffix(root, suffix_pair.first);

      if (suffix_pair.second.length() > 0) {
        suffix = suffix_pair.second;
        return true;
      }
      break;
    }
    return false;
  }

}}}
