//
//  cf_tokenizer.cpp
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

#include "cf_tokenizer.hpp"

#if JEROME_PARSING == JEROME_PARSING_CF

namespace jerome { namespace cf {
  void Tokenizer::init() {
    if (!mString) return; // locale can be NULL
    auto tokenzer = CFStringTokenizerCreate(kCFAllocatorDefault,
                                            mString,
                                            CFRangeMake(0, CFStringGetLength(mString)),
                                            kCFStringTokenizerUnitWordBoundary,
                                            mLocale);
    mTokenizer = tokenizer_type(tokenzer);
  }

  Tokenizer::Tokenizer(Locale const & inLocale)
  : mLocale(inLocale)
  , mString()
  , mTokenizer()
  {
  }

  void Tokenizer::setString(const String& inString)
  {
    mString = inString;
    mTokenizer = basic_object<CFStringTokenizerRef>();
  }

}}

#endif
