//
//  icu_string.hpp
//
//  Created by Anton Leuski on 9/21/18.
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

#ifndef icu_string_hpp
#define icu_string_hpp

#include <jerome/ir/parsing/parsing_fwd.hpp>

#if JEROME_PARSING == JEROME_PARSING_ICU

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <boost/locale.hpp>
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/cxx11/one_of.hpp>
#pragma clang diagnostic pop

namespace jerome {
  inline jerome::String
  lowercased(const jerome::String& inString, const Locale& inLocale = Locale()) {
    return boost::locale::to_lower(inString, inLocale);
  }

  inline bool
  is_alpha(const jerome::String& inString, const Locale& inLocale = Locale()) {
    auto inp = boost::locale::conv::utf_to_utf<wchar_t>(inString);
    return boost::algorithm::one_of(inp, boost::algorithm::is_alpha(inLocale));
  }

  inline bool
  is_alphanumeric(const jerome::String& inString, const Locale& inLocale = Locale()) {
    auto inp = boost::locale::conv::utf_to_utf<wchar_t>(inString);
    return boost::algorithm::one_of(inp, boost::algorithm::is_alnum(inLocale));
  }
}

#endif

#endif // defined icu_string_hpp
