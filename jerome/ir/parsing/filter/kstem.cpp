//
//  kstem.cpp
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

#include "kstem.hpp"

extern "C" {
  void read_dict_info(void);
  void stem(char *term, char *stem);
}

namespace jerome { namespace ir { namespace filter {
  namespace filter_detail {

    struct Kstemmer {
      Kstemmer() {
        read_dict_info();
      }
      inline void stem(const char* inString, char* outStem) {
        ::stem((char*)inString, outStem);
      }
    };

    void kstem_holder::stem(const char* inString, char* outStem) {
      static Kstemmer kstemmer;
      kstemmer.stem(inString, outStem);
    }
  }
}}}
