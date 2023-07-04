//
//  ModelReaderCSV.cpp
//
//  Created by Anton Leuski on 3/16/19.
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#include <boost/spirit/include/qi.hpp>
#pragma clang diagnostic pop

#include <jerome/npc/model.hpp>

#include "ModelReaderCSV.hpp"
#include "ModelReaderTable.hpp"

namespace qi = boost::spirit::qi;

namespace jerome {
  namespace npc {
    namespace detail {
      
      using Field  = std::string;
      using Fields = std::vector<Field>;
      using Record = Fields;
      
      template <typename It>
      struct CsvGrammar : qi::grammar<It, qi::blank_type>
      {
        CsvGrammar(const std::function<void(const Record&)>& processLine) 
        : CsvGrammar::base_type(start)
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
          
          start    = -record [processLine] % eol;
          record   = field % fieldsep ;
          field    = quoted | unquoted;
          unquoted = *(char_ - fieldsep - quote - eol);
          quoted   = quote 
          >> *(escaped_quote >> attr(quote) | ~char_(quote)) 
          >> quote;
          
          BOOST_SPIRIT_DEBUG_NODES((start)(record)(field)(unquoted)(quoted));
        }
      private:
        qi::rule<It, qi::blank_type> start;
        qi::rule<It, Record(), qi::blank_type> record;
        qi::rule<It, Field()> field;
        qi::rule<It, std::string()> quoted;
        qi::rule<It, std::string()> unquoted;
      };
      
      Result<Collection>  
      ModelReaderCSV::provide(const ObjectFactory& inObjectFactory,
                              std::istream& stream)
      {
        // it does not work without it. 
        stream >> std::noskipws;

        boost::spirit::istream_iterator first(stream), last;
        ModelReaderTable tableReader(inObjectFactory);
        CsvGrammar<boost::spirit::istream_iterator> 
        grammar([&](const Record& r){ tableReader.processLine(r); });
        
        bool ok = qi::phrase_parse(first, last, grammar, qi::blank);
        if (!ok) {
          return Error("Failed to parse csv file.");
        }
        
        if (first != last) {
          return Error("Remaining unparsed: '" 
                       + std::string(first, last) + "'\n");
        }
        
        return tableReader.collection();
      }
      
      std::vector<String> 
      ModelReaderCSV::acceptableFileNameExtensions() const
      {
        return {"csv"};
      }
     
    } // detail
  } // npc
} // jerome
