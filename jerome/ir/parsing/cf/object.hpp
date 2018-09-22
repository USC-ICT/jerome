//
//  object.hpp
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

#ifndef __jerome_ir_parsing_cf_object_hpp
#define __jerome_ir_parsing_cf_object_hpp

#include <jerome/types.hpp>
#include <jerome/ir/parsing/parsing_fwd.hpp>

#if JEROME_PARSING == JEROME_PARSING_CF
#include <CoreFoundation/CoreFoundation.h>

namespace jerome { namespace cf {
  template <typename T>
  struct basic_object {
    basic_object()
    : mValue(NULL)
    {}
    explicit basic_object(T inValue)
    : mValue(inValue)
    { retain(); }
    basic_object(const basic_object& inOther)
    : mValue(inOther.mValue)
    { retain(); }
    basic_object(basic_object&& inOther)
    : mValue(inOther.mValue)
    { inOther.mValue = NULL; }
    basic_object& operator = (const basic_object& inOther)
    {
      if (mValue != inOther.mValue) {
        release();
        mValue = inOther.mValue;
        retain();
      }
      return *this;
    }
    basic_object& operator = (basic_object&& inOther)
    {
      release();
      if (mValue != inOther.mValue) {
        mValue = inOther.mValue;
      }
      inOther.mValue = NULL;
      return *this;
    }
    ~basic_object() { release(); }
    operator T() const { return mValue; }
    static basic_object move(T inRef) {
      // we accept ref with +1
      basic_object result(inRef); // increment it
      result.release(); // decrement it, to release the inRef.
      return result;
    }
  protected:
    T value() const { return mValue; }
    auto retain() { if (mValue) CFRetain(mValue); }
    auto release() { if (mValue) CFRelease(mValue); }
  private:
    T mValue;
  };
}}
#endif

#endif // defined __jerome_ir_parsing_cf_object_hpp
