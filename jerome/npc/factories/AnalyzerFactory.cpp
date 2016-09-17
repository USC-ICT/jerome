//
//  AnalyzerFactory.cpp
//
//  Created by Anton Leuski on 9/1/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#include <jerome/npc/parsing.hpp>
#include "AnalyzerFactory.hpp"

namespace jerome {
  namespace npc {
		
		static StringMap<Record> makePredefinedAnalyzers(String inDefaultKey)
    {
      StringMap<Record> analyzers;
      
      // text unigram
      analyzers.emplace(std::make_pair<String, Record>(std::move(inDefaultKey), {
        AnalyzerFactory::PROVIDER_IDENTIFIER_KEY, Tokenized::IDENTIFIER
        , Tokenized::NAME, "Unigram"
        , Tokenized::INDEX_FIELD, "unigram"
        , Tokenized::UTTERANCE_FIELD, Utterance::kFieldText
      }));
      
      // text bigram
      analyzers.emplace(std::make_pair<String, Record>("text-bigram", {
        AnalyzerFactory::PROVIDER_IDENTIFIER_KEY, Tokenized::IDENTIFIER
        , Tokenized::NAME, "Unigram-Bigram"
        , Tokenized::INDEX_FIELD, "unigram"
        , Tokenized::BIGRAM_INDEX_FIELD, "bigram"
        , Tokenized::UTTERANCE_FIELD, Utterance::kFieldText
      }));
      
      // text unigram + id
      analyzers.emplace(std::make_pair<String, Record>("text-unigram+id", {
        AnalyzerFactory::PROVIDER_IDENTIFIER_KEY, MultiAnalyzer::IDENTIFIER
        , MultiAnalyzer::ANALYZERS, Record {
          "0", Record {
            AnalyzerFactory::PROVIDER_IDENTIFIER_KEY, Tokenized::IDENTIFIER
            , Tokenized::NAME, "Unigram"
            , Tokenized::INDEX_FIELD, "unigram"
            , Tokenized::UTTERANCE_FIELD, Utterance::kFieldText
          },
          "1", Record {
            AnalyzerFactory::PROVIDER_IDENTIFIER_KEY, Untokenized::IDENTIFIER
            , Untokenized::NAME, "Untokenized"
            , Untokenized::INDEX_FIELD, "externalID"
            , Untokenized::UTTERANCE_FIELD, Utterance::kFieldID
          },
        }
      }));
      
      return analyzers;
    }
    
    static const char* DEFAULT_KEY = "text-unigram";
    
		AnalyzerFactory::AnalyzerFactory()
    : mDefaultAnalyzerModelKey(DEFAULT_KEY)
    , predefinedAnalyzers(makePredefinedAnalyzers(DEFAULT_KEY))
		{
      registerProvider<Tokenized>();
      registerProvider<Untokenized>();
      registerProvider<MultiAnalyzer>();
		}
		
    Record AnalyzerFactory::defaultAnalyzerModel() const
    {
      return predefinedAnalyzers.find(mDefaultAnalyzerModelKey)->second;
    }
    
  }
}
