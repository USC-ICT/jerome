//
//  dictionary.hpp
//
//  Created by Anton Leuski on 9/11/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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
