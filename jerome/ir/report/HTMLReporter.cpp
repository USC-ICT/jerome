//
//  HTMLReporter.cpp
//
//  Created by Anton Leuski on 8/13/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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