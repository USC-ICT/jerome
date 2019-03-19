//
//  csv_tests.mm
//
//  Created by Anton Leuski on 3/19/19.
//  Copyright © 2019 Anton Leuski & ICT/USC. All rights reserved.
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

#import <Foundation/Foundation.h>

#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wshorten-64-to-32"
//#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"

#include <boost/spirit/include/qi.hpp>

#pragma clang diagnostic pop

#include <fstream>

namespace qi = boost::spirit::qi;

using Field  = std::string;
using Fields = std::vector<Field>;
using Record = Fields;
using CsvFile = std::vector<Record>;

static void my_print(const Record& record) {
  for(auto& col : record)
    std::cout << '[' << col << ']';
  std::cout << std::endl;  
}

template <typename It>
struct CsvGrammar : qi::grammar<It, CsvFile(), qi::blank_type>
{
  CsvGrammar() : CsvGrammar::base_type(start)
  {
    using namespace qi;
    
    static const char fieldsep = ',';
    static const char quote = '"';
    static const char* escaped_quote = "\"\"";
    
    // these rules will try and trim whitespaces from fields
    //    start    %= -record % eol;
    //    record   %= field % fieldsep;
    //    field    %= quoted | unquoted | *blank;
    //    word     %= +(char_ - fieldsep - space - quote);
    //    unquoted %= raw[word >> *(+blank >> word)];
    //    quoted   %= quote 
    //                >> *(lit(' '))
    //                >> *(escaped_quote >> attr(quote) | ~char_(quote)) 
    //                >> quote;
    //    BOOST_SPIRIT_DEBUG_NODES((start)(record)(field)(unquoted)(quoted)(word));
    //  private:
    //    qi::rule<It, CsvFile(), qi::blank_type> start;
    //    qi::rule<It, Record(), qi::blank_type> record;
    //    qi::rule<It, Field(), qi::blank_type> field;
    //    qi::rule<It, std::string()> quoted;
    //    qi::rule<It, std::string()> unquoted;
    //    qi::rule<It, std::string()> word;
    
    start    = -record [&my_print] % eol;
    record   = field % fieldsep ;
    field    = quoted | unquoted;
    unquoted = *(char_ - fieldsep - quote - eol);
    quoted   = quote 
    >> *(escaped_quote >> attr(quote) | ~char_(quote)) 
    >> quote;
    
    BOOST_SPIRIT_DEBUG_NODES((start)(record)(field)(unquoted)(quoted));
  }
private:
  qi::rule<It, CsvFile(), qi::blank_type> start;
  qi::rule<It, Record(), qi::blank_type> record;
  qi::rule<It, Field()> field;
  qi::rule<It, std::string()> quoted;
  qi::rule<It, std::string()> unquoted;
};

int main(int argc, char** argv)
{
  //  const std::string s = R"()";
  
  //  const std::string s = R"(      
  //  question.text,answer.text,answer.id,answer.type
  //  "hello
  //  how are you
  //  howdy
  //  good morning" ," Hi or, ""Hello!"" "  , some    , topic  here and 
  //  hi hello,5, , some space )";
  
  //  auto f(begin(s)), l(end(s));
  std::ifstream stream(argv[1]);
  stream >> std::noskipws;
  
  boost::spirit::istream_iterator f(stream), l;
  CsvGrammar<typeof f> p;
  
  bool ok = qi::phrase_parse(f, l, p, qi::blank);
  
  if (ok)
  {
    //    for(auto& line : parsed) {
    //      for(auto& col : line)
    //        std::cout << '[' << col << ']';
    //      std::cout << std::endl;
    //    }
  } else {
    std::cout << "Parse failed\n";
  }
  
  if (f != l)
    std::cout << "Remaining unparsed: '" << std::string(f, l) << "'\n";
}
