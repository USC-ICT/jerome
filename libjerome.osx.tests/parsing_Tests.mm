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

//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wshorten-64-to-32"
//#pragma clang diagnostic ignored "-Wcomma"
//#pragma clang diagnostic ignored "-Wdocumentation"
//#include <boost/range/algorithm.hpp>
//#pragma clang diagnostic pop

#include <jerome/ir/parsing.hpp>
#include <jerome/ir/parsing/filter/tokenized.hpp>

@interface ParsingTests : XCTestCase
@end

using namespace jerome;

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

template <typename Stream>
void run(Stream s) {
  while (true) {
    auto x = s.next();
    if (!x) break;
    if (x->isEOS()) break;
    std::cout << *x << std::endl;
  }
}

- (void)test02Tokenized
{
  run(test
      | stream::tokenized
      );
}

//- (void)test01
//{
//  boost::for_each(test,
//                  [&] (const auto& x) { std::cout << x << std::endl; });
//}
//
//
- (void)test03Untokenized
{
  run(test
      | stream::as_token
      | stream::lowercased
      );
}

- (void)test04Lowercased
{
  run(test
      | stream::tokenized
      | stream::lowercased
      );
}

- (void)test05Alpha
{
  run(test
      | stream::tokenized
      | stream::lowercased
      | stream::filtered(stream::is_alpha)
      );
}

- (void)test06Contractions
{
  run(test
      | stream::tokenized
      | stream::lowercased
      | stream::filtered(stream::is_alphanumeric)
      | stream::expanded_contractions
      );
}


- (void)test07IrregularVerbs
{
  run(test
      | stream::tokenized
      | stream::lowercased
      | stream::filtered(stream::is_alphanumeric)
      | stream::expanded_contractions
      | stream::transformed(stream::stem_irregular_verbs)
      );
}

- (void)test08Kstem
{
  run(test
      | stream::tokenized
      | stream::lowercased
      | stream::filtered(stream::is_alphanumeric)
      | stream::expanded_contractions
      | stream::transformed(stream::stem_irregular_verbs)
      | stream::transformed(stream::kstem)
      );
}

- (void)test09Stopwords
{
  run(test
      | stream::tokenized
      | stream::lowercased
      | stream::filtered(stream::is_alphanumeric)
      | stream::expanded_contractions
      | stream::transformed(stream::stem_irregular_verbs)
      | stream::transformed(stream::kstem)
      | stream::filtered(stream::not_stopword)
      );
}

- (void)test10NGram
{
  run(test
      | stream::tokenized
      | stream::lowercased
      | stream::filtered(stream::is_alphanumeric)
      | stream::expanded_contractions
      | stream::transformed(stream::stem_irregular_verbs)
      | stream::transformed(stream::kstem)
      | stream::filtered(stream::not_stopword)
      | stream::ngram
      );
}

- (void)test11Expression1
{
  run(test
      | (stream::tokenized
         | stream::lowercased
         | stream::filtered(stream::is_alphanumeric)
         )
      );
}

- (void)test11Expression2
{
  run(test
      | (stream::tokenized
         | stream::lowercased
         | stream::filtered(stream::is_alphanumeric)
         | stream::expanded_contractions
         | stream::transformed(stream::stem_irregular_verbs)
         | stream::transformed(stream::kstem)
         | stream::filtered(stream::not_stopword)
         | stream::ngram
         )
      );
}

@end
