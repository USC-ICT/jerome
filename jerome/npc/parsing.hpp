//
//  parsing.hpp
//
//  Created by Anton Leuski on 3/15/12.
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

#ifndef __jerome_npc_parsing_hpp__
#define __jerome_npc_parsing_hpp__

#include <jerome/ir/analysis.hpp>
#include <jerome/ir/parsing.hpp>
#include <jerome/npc/model.hpp>
#include <jerome/npc/factories/AnalyzerFactory.hpp>

namespace jerome {
  namespace npc {

    class UtteranceAnalyzer
      : public jerome::ir::detail::AnalyzerImplementation<Utterance,
        jerome::ir::HeapIndex>
    {
      typedef jerome::ir::detail::AnalyzerImplementation<Utterance,
        jerome::ir::HeapIndex> parent_type;

      Record mModel;

    public:
      typedef typename parent_type::result_type result_type;
      typedef typename parent_type::argument_type argument_type;

      static constexpr char const* NAME_KEY = "name";

      UtteranceAnalyzer() = delete;
      
      UtteranceAnalyzer(const Record& inModel)
      : parent_type(inModel.at(NAME_KEY, "UtteranceAnalyzer"))
      , mModel(inModel)
      {
      }

      Record model() const override
      {
        return mModel;
      }
    };

    class MultiAnalyzer
      : public UtteranceAnalyzer
    {
      typedef UtteranceAnalyzer parent_type;
    public:
      static constexpr char const* IDENTIFIER = "jerome.analyzer.multi";
      static constexpr char const* ANALYZER_KEY = "analyzer";

      MultiAnalyzer(const Record& inModel)
      : parent_type(inModel)
      {
      }

      void parse(argument_type inObject, result_type& ioIndex) const override;
      
    private:
      mutable optional<List<Analyzer>>  mAnalyzers;
      const List<Analyzer>& analyzers() const;
    };
    
    class Untokenized
    : public UtteranceAnalyzer
    {
      typedef UtteranceAnalyzer parent_type;
    public:
      Untokenized(const Record& inRecord)
      : parent_type(inRecord)
      {
      }
      
      static constexpr char const* IDENTIFIER = "jerome.analyzer.untokenized";
      static constexpr char const* INDEX_FIELD_KEY = "index.field";
      static constexpr char const* UTTERANCE_FIELD_KEY = "utterance.field";
      
      void parse(argument_type inObject, result_type& ioIndex) const override;
    };
    

    class Tokenized
      : public UtteranceAnalyzer
    {
      typedef UtteranceAnalyzer parent_type;
    public:
      Tokenized(const Record& inRecord)
      : parent_type(inRecord)
      {
      }

      static constexpr char const* IDENTIFIER = "jerome.analyzer.tokenized";
      static constexpr char const* INDEX_FIELD_KEY = "unigram.index.field";
      static constexpr char const* BIGRAM_INDEX_FIELD_KEY = "bigram.index.field";
      static constexpr char const* UTTERANCE_FIELD_KEY = "utterance.field";

      void parse(argument_type inObject, result_type& ioIndex) const override;
    };
  }
}

#endif // defined __jerome_npc_parsing_hpp__
