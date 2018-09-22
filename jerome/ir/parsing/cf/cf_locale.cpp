//
//  cf_locale.cpp
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

#include "cf_locale.hpp"

#if JEROME_PARSING == JEROME_PARSING_CF
#include "cf_string.hpp"

namespace jerome { namespace cf {
  static Locale  kDefaultLocale(CFLocaleCopyCurrent());

  Locale::Locale(const jerome::String& inLocale)
  : parent_type(CFLocaleCreate(kCFAllocatorDefault, cf::String(inLocale)))
  {
  }

  Locale::Locale()
  : parent_type(kDefaultLocale)
  {
  }

  void Locale::global(const jerome::String& inLocale) {
    kDefaultLocale = Locale(inLocale);
  }
}}

#endif
