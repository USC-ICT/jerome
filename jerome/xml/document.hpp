//
//  document.hpp
//
//  Created by Anton Leuski on 8/15/15.
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
