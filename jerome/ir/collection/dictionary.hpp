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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>

#pragma clang diagnostic pop

#include <jerome/type/optional.hpp>
#include <jerome/type/error.hpp>
#include <jerome/type/shared.hpp>

namespace jerome { namespace dictionary {
  struct index {};
  struct string {};

  typedef uint64_t index_type;

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
    typedef dictionary::index_type index_type;

    string_type string;
    index_type index;

    value_type(const std::string& inString,
               index_type inIndex,
               const typename string_type::allocator_type& inAllocator)
    : string(inString.begin(), inString.end(), inAllocator)
    , index(inIndex)
    {}
    value_type(const char* inString,
               index_type inIndex,
               const typename string_type::allocator_type& inAllocator)
    : string(inString, inAllocator)
    , index(inIndex)
    {}
  private:
    value_type() = delete;
  };

  template <>
  struct value_type<std::string> {
    typedef std::string string_type;
    typedef dictionary::index_type index_type;

    string_type string;
    index_type index;

    value_type(const std::string& inString,
               index_type inIndex)
    : string(inString)
    , index(inIndex)
    {}
    value_type(const char* inString,
               index_type inIndex)
    : string(inString)
    , index(inIndex)
    {}
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

  struct Dictionary {

    typedef dictionary::index_type index_type;

    Dictionary(shared::Access inAccess, const char* inPath)
    : access(inAccess)
    , mPersistent(inAccess == shared::Access::write_private
                  ? shared::Access::read_only
                  : inAccess, inPath)
    , mTransient(inAccess == shared::Access::write_private
                 ? new transient_type::element_type
                 : nullptr)
    {
    }

    JEROME_EXCEPTION(illegal_access_exception)

    const shared::Access access;

    template <typename S>
    index_type
    emplace(const S& inString) {
      switch (access) {
        case shared::read_only: {
          auto index = string2index(inString);
          if (index != Dictionary::unknownIndex) {
            return index;
          }
          throw illegal_access_exception();
        }
        case shared::write_shared: {
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
              mPersistent.get()->emplace(inString, index, mPersistent.get()->get_allocator());
              break;
            } catch (const boost::interprocess::bad_alloc& error) {
              if (++numberOfTries > 3) throw error;
              mPersistent.grow(mPersistent.storageSize());
            }
          }
          return index;
        }
        case shared::write_private: {
          auto index = string2index(inString);
          if (index != Dictionary::unknownIndex) {
            return index;
          }
          assert(mPersistent.get());
          assert(mTransient.get());
          index = mPersistent.get()->size() + mTransient.get()->size();
          mTransient.get()->emplace(inString, index);
          return index;
        }
      }
    }

    template <typename S>
    index_type
    string2index(const S& inString) const {
      assert(mPersistent.get());
      {
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
      assert(mPersistent.get());
      {
        auto iter = mPersistent.get()->get<dictionary::index>().find(inIndex);
        if (iter != mPersistent.get()->get<dictionary::index>().end()) {
          return shared::to_string(iter->string);
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
    std::numeric_limits<index_type>::max();

  private:
    typedef std::unique_ptr<dictionary::std_storage> transient_type;

    shared::MappedPointer<dictionary::shared_storage> mPersistent;
    transient_type mTransient;
  };
}

#endif // defined __jerome_ir_collection_dictionary_hpp
