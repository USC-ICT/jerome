//
//  file_index.cpp
//
//  Created by Anton Leuski on 9/18/18.
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#pragma clang diagnostic pop

#include "file_index.hpp"

namespace jerome { namespace ir {
  namespace fs = boost::filesystem;

  static const char* termFileName = "term.bin";
  static const char* dlFileName = "dl.bin";
  static const char* fieldsDirectoryName = "fields";

  namespace index {
    FileField::FileField(persistence::Access inAccess,
                         const fs::path& inPath)
    : parent_type()
    , mTerms(inAccess, inPath / termFileName)
    , mDocumentLengths(inAccess, inPath / dlFileName)
    {}
  }

  static auto createAlphabet(persistence::Access inAccess,
                             const fs::path& path,
                             AlphabetPtr inAlphabet) -> AlphabetPtr
  {
    if (inAccess != persistence::Access::read_only) {
      fs::create_directory(path);
    }

    return inAlphabet
      ? inAlphabet
      : std::make_shared<Alphabet>(inAccess, path / "alphabet.bin");
  }

  static const int version = 1;

  auto FileIndex::makeField(const String& inName) -> Field {
    auto targetPath = mPath / fieldsDirectoryName / inName;
    if (access() == persistence::Access::write_shared) {
      fs::create_directory(targetPath);
    }
    return index::FileField(access(), targetPath);
  }

  namespace pt = boost::property_tree;

  auto FileIndex::infoPath() const -> fs::path {
    return mPath / "info.json";
  }

  auto FileIndex::read() {
    if (!fs::exists(infoPath())) return;
    pt::ptree root;
    pt::read_json(infoPath().string(), root);
    if (root.get<int>("version") != version) {
      throw unsupported_index_version();
    }
  }

  auto FileIndex::write() {
    if (access() != persistence::Access::write_shared) return;
    fs::create_directory(mPath);
    pt::ptree root;
    root.put("version", version);
    pt::write_json(infoPath().string(), root);
  }

  FileIndex::FileIndex(persistence::Access inAccess,
                       const fs::path& inPath,
                       AlphabetPtr inAlphabet)
  : parent_type(createAlphabet(inAccess, inPath, inAlphabet))
  , mPath(inPath)
  , mAccess(inAccess)
  {
    read();
    auto fieldsPath = mPath / fieldsDirectoryName;
    if (fs::exists(fieldsPath) && fs::is_directory(fieldsPath)) {
      for(auto entry : fs::directory_iterator(mPath / fieldsDirectoryName)) {
        if (!fs::is_directory(entry.path())) continue;
        auto name = entry.path().filename().string();
        fields().emplace(name, index::FileField(access(), entry.path()));
      }
    }
  }
}}
