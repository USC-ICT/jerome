//
//  parsing_fwd.hpp
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

#ifndef parsing_fwd_hpp
#define parsing_fwd_hpp

#ifndef JEROME_PARSING_CF
#define JEROME_PARSING_CF  1
#endif

#ifndef JEROME_PARSING_ICU
#define JEROME_PARSING_ICU 2
#endif

#ifndef JEROME_PARSING_STD
#define JEROME_PARSING_STD 3
#endif

#if !defined(JEROME_PARSING)
# if defined(JEROME_IOS)
#   define JEROME_PARSING JEROME_PARSING_CF
# elif defined(JEROME_ANDROID)
#   define JEROME_PARSING JEROME_PARSING_STD
# else
#   define JEROME_PARSING JEROME_PARSING_ICU
# endif
#endif

namespace jerome {
  template <typename T>
  struct Lowercased {};

  template <typename T>
  struct IsAlpha {};

  template <typename T>
  struct IsAlphanumeric {};
}

#if JEROME_PARSING == JEROME_PARSING_CF
# include <jerome/ir/parsing/cf/cf_locale.hpp>
#else
# include <locale>
namespace jerome {
  using Locale = std::locale;
}
#endif

#endif // defined parsing_fwd_hpp
