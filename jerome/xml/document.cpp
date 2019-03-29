//
//  document.cpp
//
//  Created by Anton Leuski on 8/15/15.
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
