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
#include <jerome/ir/collection.hpp>

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
    if (x.isEOS()) {
      output << "END" << std::endl;
      break;
    }
    if (x.isBOS()) {
      output << "BEGIN" << std::endl;
      continue;
    }
    output << x << std::endl;
  }
  return output.str();
}

- (void)doRun:(const std::string&)inString {
  NSLog(@"%s", inString.c_str());
}

#define MY_TEST(x, o) \
std::stringstream output; \
stream::echo(output) \
x << test; \
if (strcmp(o, output.str().c_str())) {\
NSLog(@"%s", output.str().c_str()); \
XCTFail("unexpected output"); \
}

static const char* desired_result_test02Tokenized =
u8R"V0G0N({BEGIN}
{"Hello", (word)[0,5]}
{" ", (word)[5,6]}
{"the", (word)[6,9]}
{" ", (word)[9,10]}
{"World", (word)[10,15]}
{"!", (word)[15,16]}
{" ", (word)[16,17]}
{"We", (word)[17,19]}
{" ", (word)[19,20]}
{"went", (word)[20,24]}
{",", (word)[24,25]}
{" ", (word)[25,26]}
{"we", (word)[26,28]}
{" ", (word)[28,29]}
{"saw", (word)[29,32]}
{",", (word)[32,33]}
{" ", (word)[33,34]}
{"we", (word)[34,36]}
{" ", (word)[36,37]}
{"conquered", (word)[37,46]}
{".", (word)[46,47]}
{" ", (word)[47,48]}
{"We'll", (word)[48,53]}
{" ", (word)[53,54]}
{"overcome", (word)[54,62]}
{",", (word)[62,63]}
{" ", (word)[63,64]}
{"willn't", (word)[64,71]}
{" ", (word)[71,72]}
{"we", (word)[72,74]}
{"?", (word)[74,75]}
{" ", (word)[75,76]}
{"We're", (word)[76,81]}
{" ", (word)[81,82]}
{"a", (word)[82,83]}
{" ", (word)[83,84]}
{"groot", (word)[84,89]}
{".", (word)[89,90]}
{" ", (word)[90,91]}
{"Привет", (word)[91,97]}
{" ", (word)[97,98]}
{"57", (word)[98,100]}
{"!", (word)[100,101]}
{END}
)V0G0N";

- (void)test02Tokenized
{ // XCTAssert(<#expression, ...#>)
  MY_TEST(
          << stream::tokenize,
          desired_result_test02Tokenized
       );
}

static const char* desired_result_test03Untokenized =
u8R"V0G0N({BEGIN}
{"hello the world! we went, we saw, we conquered. we'll overcome, willn't we? we're a groot. привет 57!", (word)[0,107]}
{END}
)V0G0N"
;

- (void)test03Untokenized
{
  MY_TEST(
          << stream::lowercase
          << stream::as_token,
          desired_result_test03Untokenized
          );
}

static const char* desired_result_test04Lowercased =
u8R"V0G0N({BEGIN}
{"hello", (word)[0,5]}
{" ", (word)[5,6]}
{"the", (word)[6,9]}
{" ", (word)[9,10]}
{"world", (word)[10,15]}
{"!", (word)[15,16]}
{" ", (word)[16,17]}
{"we", (word)[17,19]}
{" ", (word)[19,20]}
{"went", (word)[20,24]}
{",", (word)[24,25]}
{" ", (word)[25,26]}
{"we", (word)[26,28]}
{" ", (word)[28,29]}
{"saw", (word)[29,32]}
{",", (word)[32,33]}
{" ", (word)[33,34]}
{"we", (word)[34,36]}
{" ", (word)[36,37]}
{"conquered", (word)[37,46]}
{".", (word)[46,47]}
{" ", (word)[47,48]}
{"we'll", (word)[48,53]}
{" ", (word)[53,54]}
{"overcome", (word)[54,62]}
{",", (word)[62,63]}
{" ", (word)[63,64]}
{"willn't", (word)[64,71]}
{" ", (word)[71,72]}
{"we", (word)[72,74]}
{"?", (word)[74,75]}
{" ", (word)[75,76]}
{"we're", (word)[76,81]}
{" ", (word)[81,82]}
{"a", (word)[82,83]}
{" ", (word)[83,84]}
{"groot", (word)[84,89]}
{".", (word)[89,90]}
{" ", (word)[90,91]}
{"привет", (word)[91,97]}
{" ", (word)[97,98]}
{"57", (word)[98,100]}
{"!", (word)[100,101]}
{END}
)V0G0N";

- (void)test04Lowercased
{
  MY_TEST(
          << stream::lowercase
          << stream::tokenize,
          desired_result_test04Lowercased
          );
}

static const char* desired_result_test05Alpha =
u8R"V0G0N({BEGIN}
{"hello", (word)[0,5]}
{"the", (word)[6,9]}
{"world", (word)[10,15]}
{"we", (word)[17,19]}
{"went", (word)[20,24]}
{"we", (word)[26,28]}
{"saw", (word)[29,32]}
{"we", (word)[34,36]}
{"conquered", (word)[37,46]}
{"we'll", (word)[48,53]}
{"overcome", (word)[54,62]}
{"willn't", (word)[64,71]}
{"we", (word)[72,74]}
{"we're", (word)[76,81]}
{"a", (word)[82,83]}
{"groot", (word)[84,89]}
{"привет", (word)[91,97]}
{END}
)V0G0N";

- (void)test05Alpha
{
  MY_TEST(
          << stream::filter_alpha
          << stream::lowercase
          << stream::tokenize,
          desired_result_test05Alpha
          );
}

static const char* desired_result_test06Contractions =
u8R"V0G0N({BEGIN}
{"hello", (word)[0,5]}
{"the", (word)[6,9]}
{"world", (word)[10,15]}
{"we", (word)[17,19]}
{"went", (word)[20,24]}
{"we", (word)[26,28]}
{"saw", (word)[29,32]}
{"we", (word)[34,36]}
{"conquered", (word)[37,46]}
{"we", (word)[48,53]}
{"will", (word)[48,53]}
{"overcome", (word)[54,62]}
{"will", (word)[64,71]}
{"not", (word)[64,71]}
{"we", (word)[72,74]}
{"we", (word)[76,81]}
{"are", (word)[76,81]}
{"a", (word)[82,83]}
{"groot", (word)[84,89]}
{"привет", (word)[91,97]}
{"57", (word)[98,100]}
{END}
)V0G0N";

- (void)test06Contractions
{
  MY_TEST(
          << stream::expand_contractions
          << stream::filter_alphanumeric
          << stream::lowercase
          << stream::tokenize,
          desired_result_test06Contractions
          );
}

static const char* desired_result_test07IrregularVerbs =
u8R"V0G0N({BEGIN}
{"hello", (word)[0,5]}
{"the", (word)[6,9]}
{"world", (word)[10,15]}
{"we", (word)[17,19]}
{"go", (word)[20,24]}
{"we", (word)[26,28]}
{"see", (word)[29,32]}
{"we", (word)[34,36]}
{"conquered", (word)[37,46]}
{"we", (word)[48,53]}
{"will", (word)[48,53]}
{"overcome", (word)[54,62]}
{"will", (word)[64,71]}
{"not", (word)[64,71]}
{"we", (word)[72,74]}
{"we", (word)[76,81]}
{"be", (word)[76,81]}
{"a", (word)[82,83]}
{"groot", (word)[84,89]}
{"привет", (word)[91,97]}
{"57", (word)[98,100]}
{END}
)V0G0N";


- (void)test07IrregularVerbs
{
  MY_TEST(
          << stream::stem_irregular_verbs
          << stream::expand_contractions
          << stream::filter_alphanumeric
          << stream::lowercase
          << stream::tokenize,
          desired_result_test07IrregularVerbs
          );
}

static const char* desired_result_test08Kstem =
u8R"V0G0N({BEGIN}
{"hello", (word)[0,5]}
{"the", (word)[6,9]}
{"world", (word)[10,15]}
{"we", (word)[17,19]}
{"go", (word)[20,24]}
{"we", (word)[26,28]}
{"see", (word)[29,32]}
{"we", (word)[34,36]}
{"conquer", (word)[37,46]}
{"we", (word)[48,53]}
{"will", (word)[48,53]}
{"overcome", (word)[54,62]}
{"will", (word)[64,71]}
{"not", (word)[64,71]}
{"we", (word)[72,74]}
{"we", (word)[76,81]}
{"be", (word)[76,81]}
{"a", (word)[82,83]}
{"groot", (word)[84,89]}
{"привет", (word)[91,97]}
{"57", (word)[98,100]}
{END}
)V0G0N";

- (void)test08Kstem
{
  MY_TEST(
          << stream::kstem
          << stream::stem_irregular_verbs
          << stream::expand_contractions
          << stream::filter_alphanumeric
          << stream::lowercase
          << stream::tokenize,
          desired_result_test08Kstem
          );
}

static const char* desired_result_test09Stopwords =
u8R"V0G0N({BEGIN}
{"hello", (word)[0,5]}
{"world", (word)[10,15]}
{"we", (word)[17,19]}
{"go", (word)[20,24]}
{"we", (word)[26,28]}
{"see", (word)[29,32]}
{"we", (word)[34,36]}
{"conquer", (word)[37,46]}
{"we", (word)[48,53]}
{"will", (word)[48,53]}
{"overcome", (word)[54,62]}
{"will", (word)[64,71]}
{"not", (word)[64,71]}
{"we", (word)[72,74]}
{"we", (word)[76,81]}
{"be", (word)[76,81]}
{"groot", (word)[84,89]}
{"привет", (word)[91,97]}
{"57", (word)[98,100]}
{END}
)V0G0N";

- (void)test09Stopwords
{
  MY_TEST(
          << stream::remove_stopwords
          << stream::kstem
          << stream::stem_irregular_verbs
          << stream::expand_contractions
          << stream::filter_alphanumeric
          << stream::lowercase
          << stream::tokenize,
          desired_result_test09Stopwords
          );
}

static const char* desired_result_test10NGram =
u8R"V0G0N({BEGIN}
{"__hello", (word)[0,5]}
{"hello_world", (word)[0,15]}
{"world_we", (word)[10,19]}
{"we_go", (word)[17,24]}
{"go_we", (word)[20,28]}
{"we_see", (word)[26,32]}
{"see_we", (word)[29,36]}
{"we_conquer", (word)[34,46]}
{"conquer_we", (word)[37,53]}
{"we_will", (word)[48,53]}
{"will_overcome", (word)[48,62]}
{"overcome_will", (word)[54,71]}
{"will_not", (word)[64,71]}
{"not_we", (word)[64,74]}
{"we_we", (word)[72,81]}
{"we_be", (word)[76,81]}
{"be_groot", (word)[76,89]}
{"groot_привет", (word)[84,97]}
{"привет_57", (word)[91,100]}
{"57__", (word)[98,100]}
{END}
)V0G0N";

- (void)test10NGram
{
  MY_TEST(
          << stream::ngram
          << stream::remove_stopwords
          << stream::kstem
          << stream::stem_irregular_verbs
          << stream::expand_contractions
          << stream::filter_alphanumeric
          << stream::lowercase
          << stream::tokenize,
          desired_result_test10NGram
          );
}

static const char* desired_result_test11Expression1 =
u8R"V0G0N({BEGIN}
{"hello", (word)[0,5]}
{"the", (word)[6,9]}
{"world", (word)[10,15]}
{"we", (word)[17,19]}
{"went", (word)[20,24]}
{"we", (word)[26,28]}
{"saw", (word)[29,32]}
{"we", (word)[34,36]}
{"conquered", (word)[37,46]}
{"we'll", (word)[48,53]}
{"overcome", (word)[54,62]}
{"willn't", (word)[64,71]}
{"we", (word)[72,74]}
{"we're", (word)[76,81]}
{"a", (word)[82,83]}
{"groot", (word)[84,89]}
{"привет", (word)[91,97]}
{"57", (word)[98,100]}
{END}
)V0G0N";

- (void)test11Expression1
{
  auto var = stream::filter_alphanumeric
  << stream::lowercase
  << stream::tokenize
  ;

  MY_TEST(
          << var,
          desired_result_test11Expression1
          );
}

static const char* desired_result_test11Expression2 = desired_result_test10NGram;

- (void)test11Expression2
{
  auto var = stream::ngram
  << stream::remove_stopwords
  << stream::kstem
  << stream::stem_irregular_verbs
  << stream::expand_contractions
  << stream::filter_alphanumeric
  << stream::lowercase
  << stream::tokenize
  ;

  MY_TEST(
          << var,
          desired_result_test11Expression2
          );
}

static NSURL* indexURL = [[NSURL fileURLWithPath:NSTemporaryDirectory()]
                          URLByAppendingPathComponent:[[NSUUID new] UUIDString]];


- (void)test12Index
{
  try {
    NSLog(@"%@", indexURL.path);
    ir::FileIndex index(persistence::write_shared, indexURL.fileSystemRepresentation);

    auto var = stream::write_to_index(index, "bigrams")
    << stream::ngram
    << stream::write_to_index(index, "unigrams")
    << stream::remove_stopwords
    << stream::kstem
    << stream::stem_irregular_verbs
    << stream::expand_contractions
    << stream::filter_alphanumeric
    << stream::lowercase
    << stream::tokenize
    ;

    var << test;

    index.optimize();
  } catch (const std::exception& ex) {
    XCTFail("%s", ex.what());
  }
}
@end
