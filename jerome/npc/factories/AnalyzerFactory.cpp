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
		
		AnalyzerFactory::AnalyzerFactory()
		{
      registerProvider<Tokenized>();
      registerProvider<Untokenized>();
      registerProvider<MultiAnalyzer>();

      // text unigram
      registerModel("text-unigram", {
        AnalyzerFactory::PROVIDER_KEY, Tokenized::IDENTIFIER
        , Tokenized::NAME_KEY, "Unigram"
        , Tokenized::INDEX_FIELD_KEY, "unigram"
        , Tokenized::UTTERANCE_FIELD_KEY, Utterance::kFieldText
      });
      
      // text bigram
     registerModel("text-bigram", {
        AnalyzerFactory::PROVIDER_KEY, Tokenized::IDENTIFIER
        , Tokenized::NAME_KEY, "Unigram-Bigram"
        , Tokenized::INDEX_FIELD_KEY, "unigram"
        , Tokenized::BIGRAM_INDEX_FIELD_KEY, "bigram"
        , Tokenized::UTTERANCE_FIELD_KEY, Utterance::kFieldText
      });
      
      // text unigram + id
      registerModel("text-unigram+id", {
        AnalyzerFactory::PROVIDER_KEY, MultiAnalyzer::IDENTIFIER
        , MultiAnalyzer::ANALYZER_KEY, Record {
          AnalyzerFactory::PROVIDER_KEY, Tokenized::IDENTIFIER
          , Tokenized::NAME_KEY, "Unigram"
          , Tokenized::INDEX_FIELD_KEY, "unigram"
          , Tokenized::UTTERANCE_FIELD_KEY, Utterance::kFieldText
        },
        MultiAnalyzer::ANALYZER_KEY, Record {
          AnalyzerFactory::PROVIDER_KEY, Untokenized::IDENTIFIER
          , Untokenized::NAME_KEY, "Untokenized"
          , Untokenized::INDEX_FIELD_KEY, "id"
          , Untokenized::UTTERANCE_FIELD_KEY, Utterance::kFieldID
        }
      });

      setDefaultModelKey("text-unigram");
    }
		
  }
}
