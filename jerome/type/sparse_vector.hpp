//
//  sparse_vector.hpp
//
//  Created by Anton Leuski on 9/14/18.
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

#ifndef sparse_vector_hpp
#define sparse_vector_hpp

namespace jerome {
  template <
  typename V, typename I,
  template<class, class> class VC = std::vector,
  class Allocator = std::allocator<void>
  >
  struct sparse_vector {
    public:
    typedef V value_type;
    typedef I index_type;

    private:
    typedef VC<value_type, typename Allocator::template rebind<value_type>::other> values_type;
    typedef VC<index_type, typename Allocator::template rebind<index_type>::other> indices_type;

    public:
    typedef typename values_type::pointer pointer;
    typedef typename values_type::const_pointer const_pointer;
    typedef typename values_type::reference reference;
    typedef typename values_type::const_reference const_reference;
    typedef typename values_type::size_type size_type;

    typedef typename indices_type::pointer index_pointer;
    typedef typename indices_type::const_pointer index_const_pointer;

    const_pointer data() const { return mValues.data(); }
    index_const_pointer indicies() const { return mIndices.data(); }
    size_type size() const { return mSize; }
    size_type entry_count() const { return mValues.size(); }

    sparse_vector()
    : mValues()
    , mIndices()
    , mSize(0)
    {}

    sparse_vector(const Allocator& inAllocator)
    : mValues(inAllocator)
    , mIndices(inAllocator)
    , mSize(0)
    {}

    reference at(index_type pos) // can throw
    {
      if (pos < 0 || pos >= size()) {
        throw std::out_of_range(std::string("index ") + std::to_string(pos)
                                + " out of range " + " [0.."
                                + std::to_string(size()) + "[");
      }
      if (mIndices.size() > 0 && pos == mIndices.back()) {
        return mValues.back();
      }
      auto iter = std::lower_bound(mIndices.begin(), mIndices.end(), pos);
      if (iter == mIndices.end() || *iter != pos) {
        // this exceution order is important, we preallocate space first, so if
        // an allocation exception is thrown, we have the data in a
        // consistent state.
        if (entry_count() >= capacity()) {
          reserve(1+entry_count()); // can throw
          iter = std::lower_bound(mIndices.begin(), mIndices.end(), pos);
        }
        mIndices.insert(iter, pos); // can throw
        return *mValues.insert(mValues.begin()+std::distance(mIndices.begin(), iter),
                               value_type()); // can throw
      } else {
        return *(mValues.begin()+std::distance(mIndices.begin(), iter));
      }
    }

    const_reference at(index_type pos) const // can throw
    {
      if (pos < 0 || pos >= size()) {
        throw std::out_of_range(std::string("index ") + pos
                                + " out of range " + " [0.." + size() + "[");
      }
      if (mIndices.size() > 0 && pos == mIndices.back()) {
        return mValues.back();
      }
      auto iter = std::lower_bound(mIndices.begin(), mIndices.end(), pos);
      if (iter == mIndices.end() || *iter != pos) {
        return value_type();
      } else {
        return *(mValues.begin()+std::distance(mIndices.begin(), iter));
      }
    }

    // appends the other sparse vector, assuming it starts at inOffset
    // todo make it a template
    void append(const sparse_vector& inOther, index_type inOffset) // can throw
    {
      reserve(entry_count() + inOther.entry_count()); // can throw
      resize(std::max(size(), inOffset + inOther.size()));
      auto start = entry_count();
      for(size_type i = 0; i < inOther.mIndices.size(); ++i) {
        mValues[start+i] = inOther.mValues[i];
        mIndices[start+i] = inOther.mIndices[i]+inOffset;
      }
    }

    void shrink_to_fit() {
      mValues.shrink_to_fit();
      mIndices.shrink_to_fit();
    }

    void resize(size_type inNewSize) {
      if (inNewSize < mSize) {
        index_type i = 0;
        for(; i < mIndices.size() && mIndices[i] < inNewSize; ++i);
        mValues.resize(i);
        mIndices.resize(i);
      }
      mSize = inNewSize;
    }

    void reserve(size_type inNewCapacity) // can throw
    {
      mValues.reserve(inNewCapacity); // can throw
      mIndices.reserve(inNewCapacity); // can throw
    }

    size_type capacity() const {
      return std::min(mValues.capacity(), mIndices.capacity());
    }

    private:
    values_type mValues;
    indices_type mIndices;
    index_type mSize;
  };

}
#endif // defined sparse_vector_hpp
