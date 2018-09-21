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

static jerome::String test =
  u8"Hello the World! We went, we saw, we conquered. We'll overcome, willn't we? We're a groot. Привет 57!";

- (void)test01
{
  boost::for_each(test,
                  [&] (const auto& x) { std::cout << x << std::endl; });
}

- (void)test02Tokenized
{
  boost::for_each(test | adaptors::tokenized(), [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

//- (void)test03Untokenized
//{
//  boost::for_each(test | adaptors::untokenized, [&]
//                  (const auto& x) { std::cout << x << std::endl; });
//}

- (void)test04Lowercased
{
  boost::for_each(test
                  | adaptors::tokenized()
                  | filter::lowercased
                  , [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

- (void)test05Alpha
{
  boost::for_each(test
                  | adaptors::tokenized()
                  | filter::lowercased
                  | filter::filtered(is_alpha)
                  , [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

- (void)test06Contractions
{
  boost::for_each(test
                  | adaptors::tokenized()
                  | filter::lowercased
                  | filter::filtered(is_alphanumeric)
                  | filter::expanded_contractions
                  , [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

- (void)test07IrregularVerbs
{
  boost::for_each(test
                  | adaptors::tokenized()
                  | filter::lowercased
                  | filter::filtered(is_alphanumeric)
                  | filter::expanded_contractions
                  | filter::transformed(stem_irregular_verbs)
                  , [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

- (void)test08Kstem
{
  boost::for_each(test
                  | adaptors::tokenized()
                  | filter::lowercased
                  | filter::filtered(is_alphanumeric)
                  | filter::expanded_contractions
                  | filter::transformed(stem_irregular_verbs)
                  | filter::transformed(kstem)
                  , [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

- (void)test09Stopwords
{
  boost::for_each(test
                  | adaptors::tokenized()
                  | filter::lowercased
                  | filter::filtered(is_alphanumeric)
                  | filter::expanded_contractions
                  | filter::transformed(stem_irregular_verbs)
                  | filter::transformed(kstem)
                  | filter::filtered(not_stopword)
                  , [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

- (void)test10NGram
{
  boost::for_each(test
                  | adaptors::tokenized()
                  | filter::lowercased
                  | filter::filtered(is_alphanumeric)
                  | filter::expanded_contractions
                  | filter::transformed(stem_irregular_verbs)
                  | filter::transformed(kstem)
                  | filter::transformed(bigram)
                  | filter::filtered(not_stopword)
                  , [&]
                  (const auto& x) { std::cout << x << std::endl; });
}

@end
