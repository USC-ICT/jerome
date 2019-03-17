//
//  dictionary.cpp
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>

#pragma clang diagnostic pop

#include <jerome/type/optional.hpp>
#include <jerome/type/error.hpp>
#include <jerome/type/shared.hpp>
#include "dictionary.hpp"

namespace jerome { namespace ir {

  namespace dictionary {
    struct index {};
    struct string {};

    typedef jerome::ir::Dictionary::index_type index_type;

    struct string_hash_type {
      std::size_t operator()(const char* val) const
      {
        return boost::hash_range(val, val + strlen(val));
      }
      template <typename T>
      std::size_t operator()(T const& val) const
      {
        return boost::hash_range(val.begin(), val.end());
      }
    };

    template <class S>
    struct value_type {
      typedef S string_type;
      typedef index_type index_type;

      string_type string;
      index_type index;

      value_type(string_type&& inString,
                 index_type inIndex)
      : string(std::move(inString))
      , index(inIndex)
      {}

      //    value_type(const std::string& inString,
      //               index_type inIndex,
      //               const typename string_type::allocator_type& inAllocator)
      //    : string(inString.begin(), inString.end(), inAllocator)
      //    , index(inIndex)
      //    {}
      //    value_type(const char* inString,
      //               index_type inIndex,
      //               const typename string_type::allocator_type& inAllocator)
      //    : string(inString, inAllocator)
      //    , index(inIndex)
      //    {}
      private:
      value_type() = delete;
    };

    template <class Value, class Allocator>
    struct storage {
      typedef Value value_type;
      struct string_compare_type {
        bool operator()(const char* inX,
                        const typename value_type::string_type& inY) const
        {
          return 0 == strcmp(inX, inY.c_str());
        }
        bool operator()(const std::string& inX,
                        const typename value_type::string_type& inY) const
        {
          return 0 == strcmp(inX.c_str(), inY.c_str());
        }

        template <typename T>
        bool operator()(const T& inX, const T& inY) const
        {
          return inX == inY;
        }
      };
      typedef boost::multi_index::multi_index_container<
      value_type,
      boost::multi_index::indexed_by<
      boost::multi_index::hashed_unique<
      boost::multi_index::tag<string>,
      boost::multi_index::member<
      value_type,
      typename value_type::string_type,
      &value_type::string
      >,
      string_hash_type, string_compare_type
      >,
      boost::multi_index::hashed_unique<
      boost::multi_index::tag<index>,
      boost::multi_index::member<
      value_type,
      typename value_type::index_type,
      &value_type::index
      >
      >
      >,
      Allocator
      > type;
    };

    typedef value_type<shared::string> shared_value_type;
    typedef storage<shared_value_type, shared::void_allocator::rebind<shared_value_type>::other>::type shared_storage;

    typedef value_type<std::string> std_value_type;
    typedef storage<std_value_type, std::allocator<std_value_type>>::type std_storage;
  }

  namespace detail {

    struct Dictionary {

      typedef dictionary::index_type index_type;
      typedef jerome::shared::Access access_type;

      Dictionary(access_type inAccess, const char* inPath)
      : access(inAccess)
      , mPersistent(inAccess == access_type::write_private
                    ? access_type::read_only
                    : inAccess, inPath)
      , mTransient(inAccess == access_type::write_private
                   ? new transient_type::element_type
                   : nullptr)
      {
      }

      Dictionary()
      : access(access_type::write_private)
      , mPersistent()
      , mTransient(new transient_type::element_type)
      {
      }

      const access_type access;

      template <typename S>
      index_type
      emplace(S inString) {
        switch (access) {
          case access_type::read_only: {
            auto index = string2index(inString);
            if (index != Dictionary::unknownIndex) {
              return index;
            }
            throw illegal_access_exception();
          }
          case access_type::write_shared: {
            auto index = string2index(inString);
            if (index != Dictionary::unknownIndex) {
              return index;
            }
            index = mPersistent.get()->size();
            int numberOfTries = 0;
            while (true) {
              try {
                // emplace tries to allocate
                // a node even if one already exists.
                // That's why we check for it first.
                assert(mPersistent.get());
                mPersistent.get()->emplace(jerome::shared::to_string(inString, mPersistent.get()->get_allocator()), index);
                break;
              } catch (const boost::interprocess::bad_alloc& error) {
                if (++numberOfTries > 3) throw error;
                mPersistent.grow(mPersistent.storageSize());
              }
            }
            return index;
          }
          case access_type::write_private: {
            auto index = string2index(inString);
            if (index != Dictionary::unknownIndex) {
              return index;
            }
            assert(mTransient.get());
            index = (mPersistent ? mPersistent.get()->size() : 0)
            + mTransient.get()->size();
            mTransient.get()->emplace(std::string(inString), index);
            return index;
          }
        }
      }

      template <typename S>
      index_type
      string2index(const S& inString) const {
        if (mPersistent) {
          auto iter = mPersistent.get()->get<dictionary::string>().find(inString);
          if (iter != mPersistent.get()->get<dictionary::string>().end()) {
            return iter->index;
          }
        }
        if (mTransient) {
          auto iter = mTransient.get()->get<dictionary::string>().find(inString);
          if (iter != mTransient.get()->get<dictionary::string>().end()) {
            return iter->index;
          }
        }
        return Dictionary::unknownIndex;
      }

      jerome::optional<std::string>
      index2string(index_type inIndex) const {
        if (mPersistent) {
          auto iter = mPersistent.get()->get<dictionary::index>().find(inIndex);
          if (iter != mPersistent.get()->get<dictionary::index>().end()) {
            return std::to_string(iter->string);
          }
        }
        if (mTransient) {
          auto iter = mTransient.get()->get<dictionary::index>().find(inIndex);
          if (iter != mTransient.get()->get<dictionary::index>().end()) {
            return iter->string;
          }
        }
        return jerome::optional<std::string>();
      }

      void
      optimize() {
        mPersistent.shrinkToFit();
      }

      static const index_type unknownIndex =
      jerome::ir::Dictionary::unknownIndex;

      private:
      typedef std::unique_ptr<dictionary::std_storage> transient_type;
      typedef jerome::shared::MappedPointer<dictionary::shared_storage> persistent_type;

      persistent_type mPersistent;
      transient_type mTransient;
    };
  }

  Dictionary::Dictionary(access_type inAccess, const char* inPath)
  : parent_type(std::make_shared<detail::Dictionary>(inAccess, inPath))
  {}

  Dictionary::Dictionary()
  : parent_type(std::make_shared<detail::Dictionary>())
  {}

  Dictionary::access_type Dictionary::access() const {
    return implementation().access;
  }

  Dictionary::index_type
  Dictionary::emplace(const std::string& inString) {
    return implementation().emplace(inString);
  }

  Dictionary::index_type
  Dictionary::emplace(const char* inString) {
    return implementation().emplace(inString);
  }

  Dictionary::index_type
  Dictionary::string2index(const std::string& inString) const {
    return implementation().string2index(inString);
  }

  Dictionary::index_type
  Dictionary::string2index(const char* inString) const {
    return implementation().string2index(inString);
  }

  jerome::optional<std::string>
  Dictionary::index2string(index_type inIndex) const {
    return implementation().index2string(inIndex);
  }

  void Dictionary::optimize() {
    return implementation().optimize();
  }

}}
