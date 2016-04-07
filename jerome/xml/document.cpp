//
//  document.cpp
//
//  Created by Anton Leuski on 8/15/15.
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

#include <jerome/xml/impl/document_impl.hpp>

#include "document.hpp"

namespace jerome {
  namespace xml {

		document::document()
		: mImpl(new detail::document_impl)
		{}

		document::document(std::istream& is)
		: mImpl(new detail::document_impl(is))
		{}

		document::document(document&& inOther)
		: mImpl(std::move(inOther.mImpl))
		{}

		document::document(std::unique_ptr<detail::document_impl>&& x)
		: mImpl(std::move(x))
		{}

		// must have the definition even if it's empty.
		document::~document() AL_NOEXCEPT
		{
		}
		
		document::operator bool() const
		{
			return mImpl->isGood();
		}

		OptionalError document::save(std::ostream& os, bool formatted) const
		{
			return mImpl->save(os, formatted);
		}

  }
}