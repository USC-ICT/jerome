//
//  persistence.hpp
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

#ifndef __jerome_type_persistence_hpp
#define __jerome_type_persistence_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/string.hpp>

#pragma clang diagnostic pop

namespace jerome {
  namespace persistence {
    typedef boost::interprocess::managed_mapped_file::segment_manager  segment_manager_t;
    typedef boost::interprocess::allocator<void, segment_manager_t> void_allocator;
    typedef void_allocator::rebind<char>::other char_allocator;
    typedef boost::interprocess::basic_string<char, std::char_traits<char>, char_allocator>   string;

    inline string to_string(const std::string& inString, const string::allocator_type inAllocator) {
      return string(inString.begin(), inString.end(), inAllocator);
    }

    inline string to_string(const char* inString, const string::allocator_type inAllocator) {
      return string(inString, inAllocator);
    }

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

    template <typename S>
    struct string_compare_type {
      bool operator()(const char* inX,
                      const S& inY) const
      {
        return 0 == strcmp(inX, inY.c_str());
      }
      bool operator()(const std::string& inX,
                      const S& inY) const
      {
        return 0 == strcmp(inX.c_str(), inY.c_str());
      }

      template <typename T>
      bool operator()(const T& inX, const T& inY) const
      {
        return inX == inY;
      }
    };

    enum Access {
      read_only, write_shared, write_private
    };

    struct MappedFile {
      MappedFile(Access inAccess, const char* inPath, std::size_t inInitialSize = 1024*4)
      : mFile(inAccess == write_shared
              ? boost::interprocess::managed_mapped_file(boost::interprocess::open_or_create, inPath, inInitialSize)
              : inAccess == read_only
              ? boost::interprocess::managed_mapped_file(boost::interprocess::open_read_only, inPath)
              : boost::interprocess::managed_mapped_file(boost::interprocess::open_copy_on_write, inPath))
      , mAllocator(mFile.get_segment_manager())
      {}

      ~MappedFile()
      {
        mFile.flush();
      }

      const void_allocator& allocator() const { return mAllocator; }
      const boost::interprocess::managed_mapped_file& file() const { return mFile; }
      boost::interprocess::managed_mapped_file& file() { return mFile; }

    private:
      boost::interprocess::managed_mapped_file mFile;
      void_allocator mAllocator;
    };

    template <typename T>
    struct MappedPointer {
      typedef T element_type;
      typedef element_type* pointer;

      const Access access;
      const std::string path;
      const std::size_t initialSize;

      MappedPointer()
      : access(Access::read_only)
      , path("")
      , initialSize(0)
      , mFile(nullptr)
      , mObject(nullptr)
      {}

      MappedPointer(Access inAccess,
                    const char* inPath,
                    std::size_t inInitialSize = 1024*4)
      : access(inAccess)
      , path(inPath)
      , initialSize(inInitialSize)
      {loadObject();}

      MappedPointer(Access inAccess,
                    const std::string& inPath,
                    std::size_t inInitialSize = 1024*4)
      : access(inAccess)
      , path(inPath)
      , initialSize(inInitialSize)
      {loadObject();}

      pointer get() { return mObject; }
      const pointer get() const { return mObject; }

      void grow(std::size_t inAdditionalBytes) {
        if (!mFile) return;
        mFile = nullptr;
        boost::interprocess::managed_mapped_file::grow(path.c_str(),
                                                       inAdditionalBytes);
        loadObject();
      }

      void shrinkToFit() {
        if (!mFile) return;
        mFile = nullptr;
        boost::interprocess::managed_mapped_file::shrink_to_fit(path.c_str());
        loadObject();
      }

      std::size_t storageSize() const {
        return mFile ? mFile->file().get_size() : 0;
      }

      operator bool () const { return (bool)mFile; }

    private:
      std::unique_ptr<MappedFile> mFile;
      pointer mObject;

      MappedPointer(const MappedPointer&) = delete;

      const void_allocator& allocator() const { return mFile->allocator(); }
      const boost::interprocess::managed_mapped_file& file() const { return mFile->file(); }
      boost::interprocess::managed_mapped_file& file() { return mFile->file(); }

      void loadObject() {
        mFile = std::unique_ptr<MappedFile>(new MappedFile(access,
                                                           path.c_str(),
                                                           initialSize));
        auto objectName = "Root";
        mObject = access == Access::read_only
        ? mFile->file().find<element_type>(objectName).first
        : mFile->file()
        .find_or_construct<element_type>(objectName)(typename element_type::ctor_args_list(),
                                                     mFile->allocator());
      }
    };
  }

}

namespace std {
  inline std::string to_string(const jerome::persistence::string& inString) {
    return std::string(inString.begin(), inString.end());
  }
}

#endif // defined __jerome_type_persistence_hpp
