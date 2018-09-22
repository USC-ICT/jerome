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
      String(const char* inBytes, size_type inCount);
      String substr(size_type pos, size_type count);
      explicit operator jerome::String () const;
    };

    inline std::ostream& operator << (std::ostream& outs, const String& o) {
      return outs << (jerome::String)o;
    }

    String lowercased(const String& string,
                      const Locale& inLocale = Locale());

    bool isAlpha(const String& string,
                 const Locale& inLocale = Locale());

    bool isAlphanumeric(const String& string,
                        const Locale& inLocale = Locale());
  }

  template <>
  struct Lowercased<cf::String> {
    using result_type = cf::String;
    result_type operator () (const cf::String& inString,
                             const Locale& inLocale = Locale()) const
    {
      return cf::lowercased(inString, inLocale);
    }
  };

  template <>
  struct IsAlpha<cf::String> {
    using result_type = bool;
    result_type operator () (const cf::String& inString,
                             const Locale& inLocale = Locale()) const
    {
      return cf::isAlpha(inString, inLocale);
    }
  };

  template <>
  struct IsAlphanumeric<cf::String> {
    using result_type = bool;
    result_type operator () (const cf::String& inString,
                             const Locale& inLocale = Locale()) const
    {
      return cf::isAlphanumeric(inString, inLocale);
    }
  };

  template <>
  struct Lowercased<String> {
    using result_type = String;
    result_type operator () (const String& inString,
                             const Locale& inLocale = Locale()) const
    {
      return (String)cf::lowercased(cf::String(inString), inLocale);
    }
  };

  template <>
  struct IsAlpha<String> {
    using result_type = bool;
    result_type operator () (const String& inString,
                             const Locale& inLocale = Locale()) const
    {
      return cf::isAlpha(cf::String(inString), inLocale);
    }
  };

  template <>
  struct IsAlphanumeric<String> {
    using result_type = bool;
    result_type operator () (const String& inString,
                             const Locale& inLocale = Locale()) const
    {
      return cf::isAlphanumeric(cf::String(inString), inLocale);
    }
  };
}

#endif

#endif // defined __jerome_ir_parsing_cf_string_hpp
