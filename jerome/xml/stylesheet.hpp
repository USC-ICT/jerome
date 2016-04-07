//
//  stylesheet.hpp
//
//  Created by Anton Leuski on 8/16/15.
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

#ifndef __jerome_xml_stylesheet_hpp__
#define __jerome_xml_stylesheet_hpp__

#include <memory>
#include <iosfwd>
#include <jerome/types.hpp>
#include <jerome/xml/xml_fwd.hpp>

namespace jerome {
  namespace xml {
    namespace detail {
      class stylesheet_impl;
    }
    class stylesheet
    {
    public:
      explicit stylesheet(std::istream& inStyleSheetSource);
      stylesheet(stylesheet&&);
      ~stylesheet();

      explicit operator bool () const;

      Result<document> operator () (const document& inDocument,
                                    const StringStringMap& inParams =
                                      StringStringMap()) const;

    private:
      std::unique_ptr<detail::stylesheet_impl> mImpl;
      friend class detail::stylesheet_impl;
    };

  }
}

#endif // defined __jerome_xml_stylesheet_hpp__
