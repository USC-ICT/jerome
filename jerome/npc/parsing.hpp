//
//  parsing.hpp
//
//  Created by Anton Leuski on 3/15/12.
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
      
      UtteranceAnalyzer(const jerome::ir::Dictionary& inDictionary, const Record& inModel)
      : parent_type(inDictionary, inModel.at(NAME_KEY, "UtteranceAnalyzer"))
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

      MultiAnalyzer(const jerome::ir::Dictionary& inDictionary, const Record& inModel)
      : parent_type(inDictionary, inModel)
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
      Untokenized(const jerome::ir::Dictionary& inDictionary, const Record& inRecord)
      : parent_type(inDictionary, inRecord)
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
      Tokenized(const jerome::ir::Dictionary& inDictionary, const Record& inRecord)
      : parent_type(inDictionary, inRecord)
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
