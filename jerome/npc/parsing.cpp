//
//  parsing.cpp
//  jerome
//
//  Created by Anton Leuski on 9/16/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#include "parsing.hpp"
#include "factories/AnalyzerFactory.hpp"

namespace jerome {
  namespace npc {
    
    struct MakeAnalyzersVisitor : public boost::static_visitor<> {
      List<Analyzer> analyzers;
      jerome::ir::AlphabetPtr dictionary;

      explicit MakeAnalyzersVisitor(const jerome::ir::AlphabetPtr& inDictionary)
      : dictionary(inDictionary) {}

      template <typename T>
      void operator () (const T& value)
      {}
      void operator () (const Record& value)
      {
        auto result = AnalyzerFactory::sharedInstance().make(dictionary, value);
        if (result) {
          analyzers.push_back(result.value());
        }
      }
    };
    
    const List<Analyzer>& MultiAnalyzer::analyzers() const
    {
      if (mAnalyzers) return *mAnalyzers;
      
      MakeAnalyzersVisitor visitor(dictionary());
      for(const auto& v : model()) {
        if (v.first == ANALYZER_KEY) {
          boost::apply_visitor(visitor, v.second);
        }
      }
      
      mAnalyzers = optional<List<Analyzer>>(visitor.analyzers);
      return *mAnalyzers;
    }

    void MultiAnalyzer::parse(argument_type inObject,
                              result_type& ioIndex) const
    {
      for(const auto& a : analyzers()) {
        a.parse(inObject, ioIndex);
      }
    }

    void Untokenized::parse(argument_type inObject, result_type& ioIndex) const
    {
      using namespace jerome::ir;
      
      String  empty;
      String  utteranceFieldName = model().at(UTTERANCE_FIELD_KEY,
                                              Utterance::kFieldText);
      String  indexFieldName = model().at(INDEX_FIELD_KEY, "untokenized");
      
      const String& textp     = inObject.get(utteranceFieldName, empty);

      //		std::cout << textp << std::endl;
      TokenStream ts(new NonTokenizingPipe<result_type>
                     (&textp,
                      keyword::_index = &ioIndex,
                      keyword::_field = &indexFieldName));
      ts.run();
    }

    void Tokenized::parse(argument_type inObject, result_type& ioIndex) const
    {
      using namespace jerome::ir;

      String  empty;
      String  utteranceFieldName = model().at(UTTERANCE_FIELD_KEY,
                                              Utterance::kFieldText);
      String  unigramFieldName = model().at(INDEX_FIELD_KEY, "unigram");
      auto  bigramFieldName = model().at<String>(BIGRAM_INDEX_FIELD_KEY);
      
      const String& textp     = inObject.get(utteranceFieldName, empty);
      
      //		std::cout << textp << std::endl;
      TokenStream ts(new UniversalTokenPipe<result_type>
                     (&textp,
                      keyword::_index = &ioIndex,
                      keyword::_unigram_field = &unigramFieldName,
                      keyword::_bigram_field = bigramFieldName.get_ptr()));
      ts.run();
    }

  }
}
