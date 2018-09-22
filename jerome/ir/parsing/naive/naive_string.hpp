//
//  std_string.hpp
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

#ifndef std_string_hpp
#define std_string_hpp

#include <jerome/ir/parsing/parsing_fwd.hpp>

#if JEROME_PARSING == JEROME_PARSING_STD

namespace jerome {
  template <>
  struct Lowercased<String> {
    using result_type = String;
    result_type operator () (const String& inString,
                             const Locale& inLocale = Locale()) const
    {
      String newTokenText  = inString;
      for (unsigned int i=0;i<newTokenText.size();i++)
      {
        newTokenText[i] = ::std::tolower(newTokenText[i]);
      }
      return newTokenText;
    }
  };

  template <>
  struct IsAlpha<String> {
    using result_type = bool;
    result_type operator () (const String& inString,
                             const Locale& inLocale = Locale()) const
    {
      for (unsigned int i=0;i<inString.size();i++)
      {
        if (isalpha(inString[i]))
          return true;
      }
      return false;
    }
  };

  template <>
  struct IsAlphanumeric<String> {
    using result_type = bool;
    result_type operator () (const String& inString,
                             const Locale& inLocale = Locale()) const
    {
      for (unsigned int i=0;i<inString.size();i++)
      {
        if (isalnum(inString[i]))
          return true;
      }
      return false;
    }
  };

}

#endif

#endif // defined std_string_hpp
