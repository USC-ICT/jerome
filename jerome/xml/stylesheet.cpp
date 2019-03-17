//
//  stylesheet.cpp
//
//  Created by Anton Leuski on 8/16/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#include <jerome/xml/document.hpp>
#include <jerome/xml/impl/stylesheet_impl.hpp>
#include "stylesheet.hpp"

namespace jerome {
  namespace xml {

    stylesheet::stylesheet(std::istream& is)
      : mImpl(new detail::stylesheet_impl(is))
    {
    }

    stylesheet::stylesheet(stylesheet&& s)
      : mImpl(std::move(s.mImpl))
    {
    }

    stylesheet::~stylesheet()
    {
    }

    stylesheet::operator bool () const
    {
      return mImpl->isGood();
    }

    Result<document> stylesheet::operator () (const document& inDocument,
                                              const StringStringMap& inParams)
    const
    {
      return mImpl->apply(inDocument, inParams);
    }

  }
}
