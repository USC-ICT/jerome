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

    //	using namespace jerome::ir;

    class UtteranceAnalyzer
      : public jerome::ir::detail::AnalyzerImplementation<Utterance,
        jerome::ir::HeapIndex>
    {
      typedef jerome::ir::detail::AnalyzerImplementation<Utterance,
        jerome::ir::HeapIndex> parent_type;
      String  mIDFieldName;

    public:
      typedef typename parent_type::result_type result_type;
      typedef typename parent_type::argument_type argument_type;

      UtteranceAnalyzer(const String& inName) : parent_type(inName),
        mIDFieldName("ID")
      {
      }

      const String& idFieldName() const
      {
        return mIDFieldName;
      }

    };

    class Unigram
      : public UtteranceAnalyzer
    {
      static constexpr char const* DEFAULT_NAME = "Unigram";
      static constexpr char const* DEFAULT_FIELD = "unigram";

      String  mUnigramFieldName;
      String  mTextFieldName;

    public:
      Unigram(const String& inName = DEFAULT_NAME,
        const String& inFieldName = DEFAULT_FIELD,
        const String& inTextName = Utterance::kFieldText)
        : UtteranceAnalyzer(inName)
        , mUnigramFieldName(inFieldName)
        , mTextFieldName(inTextName)
      {
      }

    public:
      Unigram(const Record& inRecord)
        : UtteranceAnalyzer(inRecord.at(NAME, String(DEFAULT_NAME)))
        , mUnigramFieldName(inRecord.at(INDEX_FIELD_NAME,
            String(DEFAULT_FIELD)))
        , mTextFieldName(inRecord.at(UTTERANCE_FIELD_NAME,
            String(Utterance::kFieldText)))
      {

      }

      static constexpr char const* IDENTIFIER =
        "edu.usc.ict.jerome.analyzer.unigram";
      static constexpr char const* NAME = "name";
      static constexpr char const* INDEX_FIELD_NAME = "index.field.name";
      static constexpr char const* UTTERANCE_FIELD_NAME =
        "utterance.field.name";

      typedef UtteranceAnalyzer parent_type;
      typedef typename parent_type::result_type result_type;
      typedef typename parent_type::argument_type argument_type;

      const String& unigramFieldName() const
      {
        return mUnigramFieldName;
      }

      Record model() const override
      {
        return {
                 NAME, name()
                 , INDEX_FIELD_NAME, unigramFieldName()
                 , UTTERANCE_FIELD_NAME, mTextFieldName
        };
      }

      void parse(argument_type inObject, result_type& ioIndex) const override
      {
        String      empty;

        const String& textp     = inObject.get(mTextFieldName, empty);
        typename result_type::Field* unigramFieldPtr = &ioIndex.findField(
          unigramFieldName(),
          true);

        //		std::cout << textp << std::endl;
        jerome::ir::TokenStream ts(new jerome::ir::UniversalTokenPipe<result_type>(
						&textp
					, jerome::ir::keyword::_unigram_field = unigramFieldPtr
				));
        ts.run();
      }

    };

    class UnigramBigram
      : public UtteranceAnalyzer
    {
      String  mUnigramFieldName;
      String  mBigramFieldName;
      String  mTextFieldName;

      static constexpr char const* DEFAULT_NAME = "Unigram-Bigram";
      static constexpr char const* DEFAULT_UNIGRAM_FIELD = "unigram";
      static constexpr char const* DEFAULT_BIGRAM_FIELD = "bigram";

    public:
      UnigramBigram(const String& inName = DEFAULT_NAME,
        const String& inUnigramFieldName = DEFAULT_UNIGRAM_FIELD,
        const String& inBigramFieldName = DEFAULT_BIGRAM_FIELD,
        const String& inTextName = Utterance::kFieldText)
        : UtteranceAnalyzer(inName)
        , mUnigramFieldName(inUnigramFieldName)
        , mBigramFieldName(inBigramFieldName)
        , mTextFieldName(inTextName)
      {
      }

      UnigramBigram(const Record& inRecord)
        : UtteranceAnalyzer(inRecord.at(NAME, String(DEFAULT_NAME)))
        , mUnigramFieldName(inRecord.at(INDEX_UNIGRAM_FIELD_NAME,
            String(DEFAULT_UNIGRAM_FIELD)))
        , mBigramFieldName(inRecord.at(INDEX_BIGRAM_FIELD_NAME,
            String(DEFAULT_BIGRAM_FIELD)))
        , mTextFieldName(inRecord.at(UTTERANCE_FIELD_NAME,
            String(Utterance::kFieldText)))
      {
      }

      static constexpr char const* IDENTIFIER =
        "edu.usc.ict.jerome.analyzer.bigram";

      static constexpr char const* NAME = "name";
      static constexpr char const* INDEX_UNIGRAM_FIELD_NAME =
        "index.unigram.field.name";
      static constexpr char const* INDEX_BIGRAM_FIELD_NAME =
        "index.bigram.field.name";
      static constexpr char const* UTTERANCE_FIELD_NAME =
        "utterance.field.name";

      typedef UtteranceAnalyzer parent_type;
      typedef typename parent_type::result_type result_type;
      typedef typename parent_type::argument_type argument_type;

      const String& unigramFieldName() const
      {
        return mUnigramFieldName;
      }

      const String& bigramFieldName() const
      {
        return mBigramFieldName;
      }

      Record model() const override
      {
        return {
                 NAME, name()
                 , INDEX_UNIGRAM_FIELD_NAME, unigramFieldName()
                 , INDEX_BIGRAM_FIELD_NAME, bigramFieldName()
                 , UTTERANCE_FIELD_NAME, mTextFieldName
        };
      }

      void parse(argument_type inObject, result_type& ioIndex) const override
      {
        String      empty;

        const String& textp     = inObject.get(mTextFieldName, empty);
        typename result_type::Field *      unigramFieldPtr = &ioIndex.findField(
          unigramFieldName(),
          true);
        typename result_type::Field *      bigramFieldPtr  = &ioIndex.findField(
          bigramFieldName(),
          true);

        //		std::cout << textp << std::endl;
        jerome::ir::TokenStream ts(
					new jerome::ir::UniversalTokenPipe<result_type>(
							&textp
            , jerome::ir::keyword::_unigram_field = unigramFieldPtr
            , jerome::ir::keyword::_bigram_field = bigramFieldPtr
            ));
        ts.run();
      }

    };

  }
}

#endif // defined __jerome_npc_parsing_hpp__
