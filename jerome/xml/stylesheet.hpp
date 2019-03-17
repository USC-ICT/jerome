//
//  stylesheet.hpp
//
//  Created by Anton Leuski on 8/16/15.
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
