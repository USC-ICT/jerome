//
//  document.hpp
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

#ifndef __jerome_xml_document_hpp__
#define __jerome_xml_document_hpp__

#include <memory>
#include <jerome/types.hpp>

namespace jerome {
  namespace xml {
    namespace detail {
      class document_impl;
    }
    class document
    {
    public:
      document();
			document(const document&) = delete;
      document(std::istream&);
      document(document&&);
      ~document() AL_NOEXCEPT;

			explicit operator bool() const;
			OptionalError save(std::ostream&, bool formatted = false) const;
    private:
      document(std::unique_ptr<detail::document_impl>&& x);
      std::unique_ptr<detail::document_impl> mImpl;
      friend struct detail::document_impl;
    };

  }
}

#endif // defined __jerome_xml_document_hpp__
