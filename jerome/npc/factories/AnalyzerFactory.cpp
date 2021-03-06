//
//  AnalyzerFactory.cpp
//
//  Created by Anton Leuski on 9/1/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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
