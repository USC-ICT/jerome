//
//  persistence_containers.hpp
//
//  Created by Anton Leuski on 9/13/18.
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

#ifndef __jerome_persistence_containers_hpp
#define __jerome_persistence_containers_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/interprocess/containers/pair.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/unordered_map.hpp>

#pragma clang diagnostic pop

namespace jerome { namespace persistence {

  template <
  typename First,
  typename Second,
  typename Allocator,
  typename FirstHash = boost::hash<First>,
  typename FirstEqualTo = std::equal_to<First>,
  typename SecondHash = boost::hash<Second>,
  typename SecondEqualTo = std::equal_to<Second>
  >
  struct bidirectional_map {
    struct first {};
    struct second {};

    typedef boost::interprocess::pair<First, Second> value_type;

    typedef boost::multi_index::multi_index_container<
    value_type,
    boost::multi_index::indexed_by<
    boost::multi_index::hashed_unique<
    boost::multi_index::tag<first>,
    boost::multi_index::member<
    value_type,
    typename value_type::first_type,
    &value_type::first
    >,
    FirstHash, FirstEqualTo
    >,
    boost::multi_index::hashed_unique<
    boost::multi_index::tag<second>,
    boost::multi_index::member<
    value_type,
    typename value_type::second_type,
    &value_type::second
    >,
    SecondHash, SecondEqualTo
    >
    >,
    typename Allocator::template rebind<value_type>::other
    > storage_type;
  };

  template <typename ...Args>
  struct MappedPointerInitializer<boost::multi_index::multi_index_container<Args...>> {
    typedef boost::multi_index::multi_index_container<Args...> element_type;
    static element_type* find_or_create(MappedFile& mappedFile, const char* objectName) {
      return mappedFile.file()
      .find_or_construct<element_type>(objectName)(typename element_type::ctor_args_list(),
                                                   mappedFile.allocator());
    }
  };

  template <
  typename First,
  typename Second,
  typename Allocator,
  typename FirstHash = boost::hash<First>,
  typename FirstEqualTo = std::equal_to<First>
  >
  struct map {
    typedef First key_type;
    typedef Second mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef boost::unordered_map<
    key_type,
    mapped_type,
    FirstHash,
    FirstEqualTo,
    typename Allocator::template rebind<value_type>::other
    > storage_type;
  };

  template <
  typename Element,
  typename Allocator
  >
  struct vector {
    typedef Element value_type;
    typedef boost::interprocess::vector<
    value_type,
    typename Allocator::template rebind<value_type>::other
    > storage_type;
  };

  template <typename Persistent, typename Transient>
  struct basic_storage {
    typedef Persistent persistent_storage_type;
    typedef Transient transient_storage_type;
    typedef std::unique_ptr<transient_storage_type> transient_type;
    typedef MappedPointer<persistent_storage_type> persistent_type;
    typedef Access access_type;

    basic_storage(access_type inAccess, const char* inPath)
    : mAccess(inAccess)
    , mPersistent(inAccess == access_type::write_private
                  ? access_type::read_only
                  : inAccess, inPath)
    , mTransient(inAccess == access_type::write_private
                 ? new typename transient_type::element_type
                 : nullptr)
    {
    }

    basic_storage()
    : mAccess(access_type::write_private)
    , mPersistent()
    , mTransient(new typename transient_type::element_type)
    {
    }

    basic_storage(basic_storage&& inOther)
    : mAccess(inOther.mAccess)
    , mPersistent(std::move(inOther.mPersistent))
    , mTransient(std::move(inOther.mTransient))
    {}

    access_type access() const { return mAccess; }

    protected:
    const persistent_type& persistent() const { return mPersistent; }
    const transient_type& transient() const { return mTransient; }
    persistent_type& persistent() { return mPersistent; }
    transient_type& transient() { return mTransient; }

    private:
    persistent_type mPersistent;
    transient_type mTransient;
    const access_type mAccess;

    private:
    basic_storage(const basic_storage&) = delete;
    basic_storage& operator=(const basic_storage&) = delete;
  };

}}

#endif // defined __jerome_persistence_containers_hpp
