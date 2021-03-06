//
//  dictionary_Tests.m
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
    try {
      {
        Dictionary dictionary(shared::Access::write_shared, fileName);

        XCTAssert(dictionary.string2index("hello") == Dictionary::unknownIndex,
                  @"the dictionary must be empty for strings");
        XCTAssert(!dictionary.index2string(0),
                  @"the dictionary must be empty for indices");
        XCTAssert(dictionary.emplace(string1) != Dictionary::unknownIndex,
                  @"should be able to insert const char*");
        try {
          XCTAssert(dictionary.emplace(string1) != Dictionary::unknownIndex,
                    @"should be able to insert const char*");
        } catch (const Dictionary::illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a write-shared: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string1) != Dictionary::unknownIndex,
                  @"the inserted string is missing");
        XCTAssert(dictionary.index2string(0),
                  @"the inserted string is missing for indices");

        XCTAssert(dictionary.emplace(std::string(string2)) != Dictionary::unknownIndex,
                  @"should be able to insert std::string");
        XCTAssert(dictionary.string2index(std::string(string1)) != Dictionary::unknownIndex,
                  @"should be able to fin std::string");

        dictionary.optimize();
      }
      {
        Dictionary dictionary(shared::Access::read_only, fileName);

        XCTAssert(dictionary.string2index(string1) != Dictionary::unknownIndex,
                  @"the existing string is missing");
        XCTAssert(dictionary.string2index(string2) != Dictionary::unknownIndex,
                  @"the existing string is missing");
        try {
          dictionary.emplace(string1);
        } catch (const Dictionary::illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a read-only: %s", error.what());
        }
        try {
          dictionary.emplace(string3);
          XCTFail(@"should not be able to emplace an string into a read-only");
        } catch (const Dictionary::illegal_access_exception& error) {
        }
        XCTAssert(dictionary.string2index(string3) == Dictionary::unknownIndex,
                  @"should not be able to emplace into read-only");
      }
      {
        Dictionary dictionary(shared::Access::write_private, fileName);

        auto existingIndex = dictionary.string2index(string1);
        XCTAssert(existingIndex != Dictionary::unknownIndex, @"the existing string is missing");
        XCTAssert(dictionary.string2index(string2) != Dictionary::unknownIndex,
                  @"the existing string is missing");
        try {
          auto newIndex = dictionary.emplace(string1);
          XCTAssert(existingIndex == newIndex,
                    @"the indices must be the same");
          XCTAssert(existingIndex == dictionary.string2index(string1),
                    @"the indices must be the same");
        } catch (const Dictionary::illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a write-private: %s", error.what());
        }
        try {
          dictionary.emplace(string3);
        } catch (const Dictionary::illegal_access_exception& error) {
          XCTFail(@"should be able to emplace a new string into a write-private: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string3) != Dictionary::unknownIndex,
                  @"should be able to emplace into write-private");
      }
      {
        Dictionary dictionary(shared::Access::write_shared, fileName);
        XCTAssert(dictionary.string2index(string1) != Dictionary::unknownIndex,
                  @"should be able to remember string placed into write-shared");
        XCTAssert(dictionary.string2index(string3) == Dictionary::unknownIndex,
                  @"should not be able to remember string placed into write-private");
        // this tests if we can expand a small file
        try {
          dictionary.emplace(string3);
        } catch (const Dictionary::illegal_access_exception& error) {
          XCTFail(@"should be able to emplace a new string into a write-shared: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string3) != Dictionary::unknownIndex,
                  @"should be able to emplace into write-private");
      }

      {
        Dictionary dictionary;

        XCTAssert(dictionary.string2index("hello") == Dictionary::unknownIndex,
                  @"the dictionary must be empty for strings");
        XCTAssert(!dictionary.index2string(0),
                  @"the dictionary must be empty for indices");
        XCTAssert(dictionary.emplace(string1) != Dictionary::unknownIndex,
                  @"should be able to insert const char*");
        try {
          XCTAssert(dictionary.emplace(string1) != Dictionary::unknownIndex,
                    @"should be able to insert const char*");
        } catch (const Dictionary::illegal_access_exception& error) {
          XCTFail(@"should be able to emplace an existing string into a write-shared: %s", error.what());
        }
        XCTAssert(dictionary.string2index(string1) != Dictionary::unknownIndex,
                  @"the inserted string is missing");
        XCTAssert(dictionary.index2string(0),
                  @"the inserted string is missing for indices");

        XCTAssert(dictionary.emplace(std::string(string2)) != Dictionary::unknownIndex,
                  @"should be able to insert std::string");
        XCTAssert(dictionary.string2index(std::string(string1)) != Dictionary::unknownIndex,
                  @"should be able to fin std::string");

        dictionary.optimize();
      }

    } catch (const std::exception& error) {
      XCTFail(@"Unexpected C++ exception is caught %s", error.what());
    }
  }

  @end

