//
//  parsing_Tests.m
//
//  Created by Anton Leuski on 9/19/18.
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

#import <XCTest/XCTest.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#pragma clang diagnostic pop

#include <jerome/ir/parsing.hpp>

@interface ParsingTests : XCTestCase
@end

using namespace jerome;
using namespace jerome::ir;

@implementation ParsingTests

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

static const char* text = "Hello World!";
static jerome::String test { text };

- (void)test
{
  boost::for_each(test,
                  [&] (const auto& x) { std::cout << x << std::endl; });
}

- (void)testTokenized
{
  boost::for_each(test | adaptors::tokenized(), [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

- (void)testUntokenized
{
  boost::for_each(test | adaptors::untokenized, [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

@end
