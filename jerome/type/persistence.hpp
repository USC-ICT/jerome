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

#include <jerome/type/filesystem.hpp>

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
      MappedFile(Access inAccess, const fs::path& inPath, std::size_t inInitialSize = 1024*4)
      : mFile(inAccess == write_shared
              ? boost::interprocess::managed_mapped_file(boost::interprocess::open_or_create, inPath.c_str(), inInitialSize)
              : inAccess == read_only
              ? boost::interprocess::managed_mapped_file(boost::interprocess::open_read_only, inPath.c_str())
              : boost::interprocess::managed_mapped_file(boost::interprocess::open_copy_on_write, inPath.c_str()))
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

    struct MappedPointerBase {
      std::size_t storageSize() const {
        return mFile ? mFile->file().get_size() : 0;
      }

      operator bool () const { return (bool)mFile; }

      Access access() const { return mAccess; }

      const void_allocator& allocator() const {
        return mFile->allocator();
      }

      const boost::interprocess::managed_mapped_file& file() const {
        return mFile->file();
      }

    protected:
      MappedPointerBase(Access inAccess,
                        const fs::path& inPath,
                        std::size_t inInitialSize)
      : mAccess(inAccess)
      , mPath(inPath)
      , mInitialSize(inInitialSize)
      , mFile(nullptr)
      {}

      MappedPointerBase(MappedPointerBase&& inOther) = default;

      const Access mAccess;
      const fs::path mPath;
      const std::size_t mInitialSize;
      std::unique_ptr<MappedFile> mFile;

      MappedPointerBase(const MappedPointerBase&) = delete;

      boost::interprocess::managed_mapped_file& file() {
        return mFile->file();
      }

      void initializeFile() {
        mFile = std::unique_ptr<MappedFile>(new MappedFile(mAccess,
                                                           mPath,
                                                           mInitialSize));
      }
    };

    template <typename T>
    struct MappedPointerInitializer {
      typedef T element_type;
      static T* find_or_create(MappedFile& mappedFile, const char* objectName) {
        return mappedFile.file()
          .find_or_construct<T>(objectName)(mappedFile.allocator());
      }
    };

    template <typename T>
    struct MappedPointer : public MappedPointerBase {
      typedef T element_type;
      typedef element_type* pointer;
      typedef MappedPointerBase parent_type;

      MappedPointer()
      : parent_type(Access::read_only, "", 0)
      , mObject(nullptr)
      {}

      MappedPointer(MappedPointer&& inOther) = default;

      MappedPointer(Access inAccess,
                    const fs::path& inPath,
                    std::size_t inInitialSize = 1024*4)
      : parent_type(inAccess, inPath, inInitialSize)
      {loadObject();}

      /// avoid capturing this pointer at any cost!
      pointer get() { return mObject; }
      const pointer get() const { return mObject; }

      template <typename F>
      auto perform(F inFunction, int numberOfTries = 10)
      -> decltype(inFunction())
      {
        while (true) {
          try {
            return inFunction();
          } catch (const boost::interprocess::bad_alloc& error) {
            if (--numberOfTries <= 0) throw;
            grow(storageSize());
          }
        }
      }

      void grow(std::size_t inAdditionalBytes) {
        if (!mFile) return;
        mFile = nullptr;
        boost::interprocess::managed_mapped_file::grow(mPath.c_str(),
                                                       inAdditionalBytes);
        loadObject();
      }

      void shrinkToFit() {
        if (!mFile) return;
        mFile = nullptr;
        boost::interprocess::managed_mapped_file::shrink_to_fit(mPath.c_str());
        loadObject();
      }

    private:
      pointer mObject;

      friend parent_type;

      MappedPointer(const MappedPointer&) = delete;

      void loadObject() {
        initializeFile();
        auto objectName = "Root";
        mObject = access() == Access::read_only
        ? mFile->file().find<element_type>(objectName).first
        : MappedPointerInitializer<element_type>::find_or_create(*mFile, objectName);
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
