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

#include <sstream>
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
std::string run(Stream s) {
  std::stringstream output;
  while (true) {
    auto x = s.next();
    if (!x) {
      return "No EOS";
    }
    if (x->isEOS()) {
      output << "END" << std::endl;
      break;
    }
    if (x->isBOS()) {
      output << "BEGIN" << std::endl;
      continue;
    }
    output << *x << std::endl;
  }
  return output.str();
}

- (void)doRun:(const std::string&)inString {
  NSLog(@"%s", inString.c_str());
}

- (void)test02Tokenized
{
  [self doRun:
   run(test
      | stream::tokenized
      )
   ];
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
  [self doRun:
   run(test
      | stream::as_token
      | stream::lowercase
       )
   ];
}

- (void)test04Lowercased
{
  [self doRun:
   run(test
      | stream::tokenized
      | stream::lowercase
       )
   ];
}

- (void)test05Alpha
{
  [self doRun:
   run(test
      | stream::tokenized
      | stream::lowercase
      | stream::filter_alpha
       )
   ];
}

- (void)test06Contractions
{
  [self doRun:
   run(test
      | stream::tokenized
      | stream::lowercase
      | stream::filter_alphanumeric
      | stream::expand_contractions
       )
   ];
}


- (void)test07IrregularVerbs
{
  [self doRun:
   run(test
      | stream::tokenized
      | stream::lowercase
      | stream::filter_alphanumeric
      | stream::expand_contractions
      | stream::stem_irregular_verbs
       )
   ];
}

- (void)test08Kstem
{
  [self doRun:
   run(test
      | stream::tokenized
      | stream::lowercase
      | stream::filter_alphanumeric
      | stream::expand_contractions
      | stream::stem_irregular_verbs
      | stream::kstem
       )
   ];
}

- (void)test09Stopwords
{
  [self doRun:
   run(test
      | stream::tokenized
      | stream::lowercase
      | stream::filter_alphanumeric
      | stream::expand_contractions
      | stream::stem_irregular_verbs
      | stream::kstem
      | stream::remove_stopwords
       )
   ];
}

- (void)test10NGram
{
  [self doRun:
   run(test
      | stream::tokenized
      | stream::lowercase
      | stream::filter_alphanumeric
      | stream::expand_contractions
      | stream::stem_irregular_verbs
      | stream::kstem
      | stream::remove_stopwords
      | stream::ngram
       )
   ];
}

- (void)test11Expression1
{
  [self doRun:
   run(test
      | (stream::tokenized
         | stream::lowercase
         | stream::filter_alphanumeric
         )
       )
   ];
}

- (void)test11Expression2
{
  [self doRun:
   run(test
      | (stream::tokenized
         | stream::lowercase
         | stream::filter_alphanumeric
         | stream::expand_contractions
         | stream::stem_irregular_verbs
         | stream::kstem
         | stream::remove_stopwords
         | stream::ngram
         )
       )
   ];
}

@end
