//
//  parsing.cpp
//  jerome
//
//  Created by Anton Leuski on 9/16/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#include "parsing.hpp"
#include "factories/AnalyzerFactory.hpp"

namespace jerome {
  namespace npc {
    
    struct MakeAnalyzersVisitor : public boost::static_visitor<> {
      List<Analyzer> analyzers;
      jerome::ir::Dictionary dictionary;

      explicit MakeAnalyzersVisitor(const jerome::ir::Dictionary& inDictionary)
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
