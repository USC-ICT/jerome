//
//  HTMLReporter.cpp
//
//  Created by Anton Leuski on 8/13/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <strstream>

#include <jerome/xml/writer.hpp>
#include <jerome/xml/document.hpp>
#include <jerome/xml/stylesheet.hpp>

#include "HTMLReporter.hpp"

namespace jerome {
  namespace ir {
    namespace evaluation {

      namespace detail {
// this uses C++11 string literal
        const char * kDefaultStylesheet =
#include "HTMLReporter.xslt.inc"
        ;

        static xml::stylesheet init_stylesheet()
        {
          std::strstreambuf buffer(kDefaultStylesheet, 0);
          std::istream is(&buffer);
          return xml::stylesheet(is);
        }

        static xml::stylesheet& defaultStylesheet()
        {
          static xml::stylesheet sStyleSheet = init_stylesheet();
          return sStyleSheet;
        }

        HTMLReporterBase::HTMLReporterBase()
        {
        }

        HTMLReporterBase::~HTMLReporterBase()
        {
        }

        xml::stylesheet& HTMLReporterBase::stylesheet()
        {
          return defaultStylesheet();
        }
      }
    }
  }
}
