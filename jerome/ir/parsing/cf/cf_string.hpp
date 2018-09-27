//
//  cf_string.hpp
//
//  Created by Anton Leuski on 9/19/18.
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

#ifndef __jerome_ir_parsing_cf_string_hpp
#define __jerome_ir_parsing_cf_string_hpp

#include <jerome/ir/parsing/locale.hpp>

#if JEROME_PARSING == JEROME_PARSING_CF

#include <jerome/ir/parsing/cf/object.hpp>

namespace jerome {
  namespace cf {
    struct String : public basic_object<CFStringRef> {
      typedef basic_object<CFStringRef> parent_type;
      using parent_type::parent_type;
      typedef CFIndex size_type;

      String(const jerome::String& inString);
      String(const char* inCString, bool inDoCopy = true);
      String(const uint8_t* inBytes, size_type inCount, bool inDoCopy = true);
      
      String substr(size_type pos, size_type count);
      explicit operator jerome::String () const;
    };

    inline std::ostream& operator << (std::ostream& outs, const String& o) {
      return outs << (jerome::String)o;
    }

    bool operator == (const String& lhs, const String& rhs);

    String lowercased(const String& string,
                      const Locale& inLocale = Locale());

    bool is_alpha(const String& string,
                  const Locale& inLocale = Locale());

    bool is_alphanumeric(const String& string,
                         const Locale& inLocale = Locale());
  }

  inline jerome::String
  lowercased(const jerome::String& inString, const Locale& inLocale = Locale()) {
    return (String)cf::lowercased(cf::String(inString), inLocale);
  }

  inline bool
  is_alpha(const jerome::String& inString, const Locale& inLocale = Locale()) {
    return is_alpha(cf::String(inString), inLocale);
  }

  inline bool
  is_alphanumeric(const jerome::String& inString, const Locale& inLocale = Locale()) {
    return is_alphanumeric(cf::String(inString), inLocale);
  }
}

#endif

#endif // defined __jerome_ir_parsing_cf_string_hpp
