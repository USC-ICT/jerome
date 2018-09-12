//
//  dictionary.hpp
//
//  Created by Anton Leuski on 9/11/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_collection_dictionary_hpp
#define __jerome_ir_collection_dictionary_hpp

#include <jerome/types.hpp>
#include <jerome/type/shared.hpp>

namespace jerome { namespace ir {

  JEROME_EXCEPTION(illegal_access_exception)

  namespace detail {
    class Dictionary;
  }

  struct Dictionary : public ReferenceClassInterface<detail::Dictionary> {
    typedef ReferenceClassInterface<detail::Dictionary> parent_type;
    using parent_type::parent_type;

    typedef uint64_t index_type;
    typedef jerome::shared::Access access_type;
    typedef illegal_access_exception illegal_access_exception;

    static const index_type unknownIndex =
    std::numeric_limits<index_type>::max();

    Dictionary(access_type inAccess, const char* inPath);
    Dictionary();

    access_type access() const;

    index_type
    emplace(const std::string& inString);

    index_type
    emplace(const char* inString);

    index_type
    string2index(const std::string& inString) const;

    index_type
    string2index(const char* inString) const;

    jerome::optional<std::string>
    index2string(index_type inIndex) const;

    void optimize();
  };


}}
#endif // defined __jerome_ir_collection_dictionary_hpp
