//
//  dictionary_Tests.m
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"

#include <boost/filesystem.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#pragma clang diagnostic pop

#include <jerome/ir/collection/dictionary.hpp>

#import <XCTest/XCTest.h>

using namespace jerome;
using namespace jerome::ir;

@interface CollectionDictionary : XCTestCase
  @end

static const char* fileName = "MyMappedFile";
struct Remover {
  Remover() {   boost::interprocess::file_mapping::remove(fileName); }
  ~Remover() {   boost::interprocess::file_mapping::remove(fileName); }
};

@implementation CollectionDictionary
- (void)setUp
  {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
  }

- (void)tearDown
  {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
  }

- (void)test
  {
    Remover remover;
    const char* string1 = "hello";
    const char* string2 = "hello again";
    const char* string3 = "hello there";

    typedef persistence::Access access_type;
    typedef Alphabet::illegal_access_exception illegal_access_exception;
    const auto unknownIndex = Alphabet::unknownIndex;

    try {
      {
        Alphabet dictionary(access_type::write_shared, fileName);

        XCTAssert(dictionary.string2index("hello") == unknownIndex,
                  @"the dictionary must be empty for strings");
        XCTAssert(!dictionary.index2string(0),
                  @"the dictionary must be empty for indices");
        XCTAssert(dictionary.emplace(string1) != unknownIndex,
                  @"should be able to insert const char*");
        try {
          XCTAssert(dictionary.emplace(string1) != unknownIndex,
                    @"should be able to insert const char*");
        } catch (const illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a write-shared: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string1) != unknownIndex,
                  @"the inserted string is missing");
        XCTAssert(dictionary.index2string(0),
                  @"the inserted string is missing for indices");

        XCTAssert(dictionary.emplace(std::string(string2)) != unknownIndex,
                  @"should be able to insert std::string");
        XCTAssert(dictionary.string2index(std::string(string1)) != unknownIndex,
                  @"should be able to fin std::string");

        dictionary.optimize();
      }
      {
        Alphabet dictionary(access_type::read_only, fileName);

        XCTAssert(dictionary.string2index(string1) != unknownIndex,
                  @"the existing string is missing");
        XCTAssert(dictionary.string2index(string2) != unknownIndex,
                  @"the existing string is missing");
        try {
          dictionary.emplace(string1);
        } catch (const illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a read-only: %s", error.what());
        }
        try {
          dictionary.emplace(string3);
          XCTFail(@"should not be able to emplace an string into a read-only");
        } catch (const illegal_access_exception& error) {
        }
        XCTAssert(dictionary.string2index(string3) == unknownIndex,
                  @"should not be able to emplace into read-only");
      }
      {
        Alphabet dictionary(access_type::write_private, fileName);

        auto existingIndex = dictionary.string2index(string1);
        XCTAssert(existingIndex != unknownIndex, @"the existing string is missing");
        XCTAssert(dictionary.string2index(string2) != unknownIndex,
                  @"the existing string is missing");
        try {
          auto newIndex = dictionary.emplace(string1);
          XCTAssert(existingIndex == newIndex,
                    @"the indices must be the same");
          XCTAssert(existingIndex == dictionary.string2index(string1),
                    @"the indices must be the same");
        } catch (const illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a write-private: %s", error.what());
        }
        try {
          dictionary.emplace(string3);
        } catch (const illegal_access_exception& error) {
          XCTFail(@"should be able to emplace a new string into a write-private: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string3) != unknownIndex,
                  @"should be able to emplace into write-private");
      }
      {
        Alphabet dictionary(access_type::write_shared, fileName);
        XCTAssert(dictionary.string2index(string1) != unknownIndex,
                  @"should be able to remember string placed into write-shared");
        XCTAssert(dictionary.string2index(string3) == unknownIndex,
                  @"should not be able to remember string placed into write-private");
        // this tests if we can expand a small file
        try {
          dictionary.emplace(string3);
        } catch (const illegal_access_exception& error) {
          XCTFail(@"should be able to emplace a new string into a write-shared: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string3) != unknownIndex,
                  @"should be able to emplace into write-private");
      }

      {
        Alphabet dictionary;

        XCTAssert(dictionary.string2index("hello") == unknownIndex,
                  @"the dictionary must be empty for strings");
        XCTAssert(!dictionary.index2string(0),
                  @"the dictionary must be empty for indices");
        XCTAssert(dictionary.emplace(string1) != unknownIndex,
                  @"should be able to insert const char*");
        try {
          XCTAssert(dictionary.emplace(string1) != unknownIndex,
                    @"should be able to insert const char*");
        } catch (const illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a write-shared: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string1) != unknownIndex,
                  @"the inserted string is missing");
        XCTAssert(dictionary.index2string(0),
                  @"the inserted string is missing for indices");

        XCTAssert(dictionary.emplace(std::string(string2)) != unknownIndex,
                  @"should be able to insert std::string");
        XCTAssert(dictionary.string2index(std::string(string1)) != unknownIndex,
                  @"should be able to fin std::string");

        dictionary.optimize();
      }

    } catch (const std::exception& error) {
      XCTFail(@"Unexpected C++ exception is caught %s", error.what());
    }
  }

  @end

