//
//  parsing.cpp
//  jerome
//
//  Created by Anton Leuski on 9/16/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include "parsing.hpp"
#include "factories/AnalyzerFactory.hpp"

namespace jerome {
  namespace npc {
    
    struct MakeAnalyzersVisitor : public boost::static_visitor<> {
      List<Analyzer> analyzers;
      template <typename T>
      void operator () (const T& value)
      {}
      void operator () (const Record& value)
      {
        auto result = AnalyzerFactory::sharedInstance().make(value);
        if (result) {
          analyzers.push_back(result.value());
        }
      }
    };
    
    const List<Analyzer>& MultiAnalyzer::analyzers() const
    {
      if (mAnalyzers) return *mAnalyzers;
      
      MakeAnalyzersVisitor visitor;
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
      typename result_type::Field* indexFieldPtr =
        &ioIndex.findField(indexFieldName, true);
      
      //		std::cout << textp << std::endl;
      TokenStream ts(new NonTokenizingPipe<result_type>
                     (&textp, keyword::_field = indexFieldPtr));
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
      typename result_type::Field* unigramFieldPtr =
        &ioIndex.findField(unigramFieldName, true);
      typename result_type::Field* bigramFieldPtr  =
      bigramFieldName ? &ioIndex.findField(*bigramFieldName, true) : nullptr;
      
      //		std::cout << textp << std::endl;
      TokenStream ts(new UniversalTokenPipe<result_type>
                     (&textp,
                      keyword::_unigram_field = unigramFieldPtr,
                      keyword::_bigram_field = bigramFieldPtr));
      ts.run();
    }

  }
}
