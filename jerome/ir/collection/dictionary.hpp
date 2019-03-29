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
#include <jerome/type/persistence.hpp>
#include "persistence_containers.hpp"

namespace jerome { namespace ir {

  JEROME_EXCEPTION(illegal_access_exception)

  namespace detail {

    typedef uint64_t index_type;

    typedef jerome::persistence::bidirectional_map<
    persistence::string,
    index_type,
    jerome::persistence::void_allocator,
    jerome::persistence::string_hash_type,
    jerome::persistence::string_compare_type<persistence::string>
    > persistent_bidirectional_map;

    typedef jerome::persistence::bidirectional_map<
    ::std::string,
    index_type,
    ::std::allocator<void>,
    jerome::persistence::string_hash_type,
    jerome::persistence::string_compare_type<::std::string>
    > transient_bidirectional_map;

    struct AlphabetImpl : public jerome::persistence::basic_storage<
    persistent_bidirectional_map::storage_type,
    transient_bidirectional_map::storage_type>
    {
      typedef jerome::persistence::basic_storage<
      persistent_bidirectional_map::storage_type,
      transient_bidirectional_map::storage_type> parent_type;

      using parent_type::parent_type;

      typedef jerome::ir::illegal_access_exception illegal_access_exception;
      typedef persistent_bidirectional_map::value_type::second_type index_type;

      template <typename S>
      index_type
      emplace(S inString) {
        switch (access()) {
          case access_type::read_only: {
            auto index = string2index(inString);
            if (index != unknownIndex) {
              return index;
            }
            throw illegal_access_exception();
          }
          case access_type::write_shared: {
            auto index = string2index(inString);
            if (index != unknownIndex) {
              return index;
            }
            index = persistent().get()->size();
            persistent().perform([&]{
              assert(persistent().get());
              persistent().get()->emplace(
                jerome::persistence::to_string(
                  inString, persistent().get()->get_allocator()), index);
            });
            return index;
          }
          case access_type::write_private: {
            auto index = string2index(inString);
            if (index != unknownIndex) {
              return index;
            }
            assert(transient().get());
            index = (persistent() ? persistent().get()->size() : 0)
            + transient().get()->size();
            transient().get()->emplace(::std::string(inString), index);
            return index;
          }
        }
      }

      template <typename S>
      index_type
      string2index(const S& inString) const {
        if (persistent()) {
          auto iter = persistent().get()->get<persistent_bidirectional_map::first>().find(inString);
          if (iter != persistent().get()->get<persistent_bidirectional_map::first>().end()) {
            return iter->second;
          }
        }
        if (transient()) {
          auto iter = transient().get()->get<transient_bidirectional_map::first>().find(inString);
          if (iter != transient().get()->get<transient_bidirectional_map::first>().end()) {
            return iter->second;
          }
        }
        return unknownIndex;
      }

      jerome::optional<::std::string>
      index2string(index_type inIndex) const {
        if (persistent()) {
          auto iter = persistent().get()->get<persistent_bidirectional_map::second>().find(inIndex);
          if (iter != persistent().get()->get<persistent_bidirectional_map::second>().end()) {
            return ::std::to_string(iter->first);
          }
        }
        if (transient()) {
          auto iter = transient().get()->get<transient_bidirectional_map::second>().find(inIndex);
          if (iter != transient().get()->get<transient_bidirectional_map::second>().end()) {
            return iter->first;
          }
        }
        return jerome::optional<::std::string>();
      }

      void
      optimize() {
        persistent().shrinkToFit();
      }

      static const index_type unknownIndex =
      ::std::numeric_limits<index_type>::max();

    };
  }

  typedef detail::AlphabetImpl Alphabet;
  typedef ::std::shared_ptr<Alphabet> AlphabetPtr;

//  struct Dictionary : public ReferenceClassInterface<detail::Dictionary> {
//    typedef ReferenceClassInterface<detail::Dictionary> parent_type;
//    using parent_type::parent_type;
//
//    typedef uint64_t index_type;
//    typedef jerome::persistence::Access access_type;
//    typedef illegal_access_exception illegal_access_exception;
//
//    static const index_type unknownIndex =
//    std::numeric_limits<index_type>::max();
//
//    Dictionary(access_type inAccess, const char* inPath);
//    Dictionary();
//
//    access_type access() const;
//
//    index_type
//    emplace(const std::string& inString);
//
//    index_type
//    emplace(const char* inString);
//
//    index_type
//    string2index(const std::string& inString) const;
//
//    index_type
//    string2index(const char* inString) const;
//
//    jerome::optional<std::string>
//    index2string(index_type inIndex) const;
//
//    void optimize();
//  };

  namespace index {
    template <class Index>
    struct indexTraits {
    };
  }

}}
#endif // defined __jerome_ir_collection_dictionary_hpp
