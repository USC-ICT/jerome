//
//  stylesheet.cpp
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