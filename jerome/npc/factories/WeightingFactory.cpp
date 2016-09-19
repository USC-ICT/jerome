//
//  WeightingFactory.cpp
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

#include <jerome/type/Record.hpp>
#include <jerome/ir/rm/weighting.hpp>
#include <jerome/ir/rm/AnyWeighting.hpp>

#include <jerome/ir/rm/weighting/query/jelinek_mercer.hpp>
#include <jerome/ir/rm/weighting/document/cross_entropy_jelinek_mercer.hpp>

#include "WeightingFactory.hpp"

namespace jerome {
  namespace npc {

    namespace detail {

      struct SubWeightingParsingVisitor
        : public Record::Visitor
      {

        String weightingName;

        SubWeightingParsingVisitor(const String& inDefaultWeightingName)
          : weightingName(inDefaultWeightingName)
        {
        }

        using Record::Visitor::operator ();

        void operator () (const String& inKey, const String& inValue)
        {
          if (inKey == ir::rm::weighting::Weighting::NAME_KEY) {
            weightingName = inValue;
          }
        }

      };

      struct WeightingParsingVisitor
        : public Record::Visitor
      {
        std::vector<String> fieldName;
        String weightingName;
        String defaultFieldName;

        using Record::Visitor::operator ();

        WeightingParsingVisitor(const String& inDefaultWeightingName,
          const String& inDefaultFieldName)
          : weightingName(inDefaultWeightingName)
          , defaultFieldName(inDefaultFieldName)
        {
        }

        void operator () (const String& inKey, const String& inValue)
        {
          if (inKey == ir::rm::weighting::Weighting::NAME_KEY) {
            weightingName = inValue;
          }
        }

        void operator () (const String& inKey, const Record& inValue)
        {
          if (inKey == ir::rm::weighting::Weighting::SUB_WEIGHTING_KEY) {
            SubWeightingParsingVisitor visitor(defaultFieldName);
            inValue.visit(visitor);
            fieldName.push_back(visitor.weightingName);
          }
        }

        template <typename Object, typename DW, template <typename ...> class W>
        inline Result<Object> makeWeighting(const String& inProviderID)
        {
          switch (fieldName.size()) {
            case 1: return Object::template make<W>(inProviderID, weightingName,
              DW(fieldName[0]));
            case 2: return Object::template make<W>(inProviderID, weightingName,
              DW(fieldName[0]), DW(fieldName[1]));
            case 3: return Object::template make<W>(inProviderID, weightingName,
              DW(fieldName[0]), DW(fieldName[1]), DW(fieldName[2]));
            case 4: return Object::template make<W>(inProviderID, weightingName,
              DW(fieldName[0]), DW(fieldName[1]), DW(fieldName[2]),
              DW(fieldName[3]));
            case 5: return Object::template make<W>(inProviderID, weightingName,
              DW(fieldName[0]), DW(fieldName[1]), DW(fieldName[2]),
              DW(fieldName[3]), DW(fieldName[4]));
            default:
              return Error("too many fields when creating a weighting: "
              + std::to_string(fieldName.size()));
          }
        }

        template <typename Object, typename DW, template <typename ...> class W>
        inline Result<Object> parseRecord(const String& inProviderID)
        {
          if (fieldName.size() == 0) {
            fieldName.push_back("unigram");
          }

          return makeWeighting<Object, DW, W>(inProviderID);
        }

      };

      struct DefaultDocumentWeightingProvider
        : public AnswerWeightingFactory::provider_type
      {

        static constexpr const char* IDENTIFIER =
          "jerome.weighting.document.addCrossEntropyJelinekMercer";

        Result<AnswerWeightingFactory::object_type> provide(
          const Record& inRecord) override
        {
          WeightingParsingVisitor visitor("answer", "unigram");
          inRecord.visit(visitor);

          return visitor.parseRecord<AnswerWeightingFactory::object_type,
            ir::rm::weighting::document::CrossEntropyJelinekMercer,
            ir::rm::weighting::document::AddDocumentProbabilities>(IDENTIFIER);
        }

      };

      struct DefaultQueryWeightingProvider
        : public QuestionWeightingFactory::provider_type
      {
        static constexpr const char* IDENTIFIER =
          "jerome.weighting.query.multiplyJelinekMercer";

        Result<QuestionWeightingFactory::object_type> provide(
          const Record& inRecord) override
        {
          WeightingParsingVisitor visitor("question", "unigram");
          inRecord.visit(visitor);

          return visitor.parseRecord<QuestionWeightingFactory::object_type,
            jerome::ir::rm::weighting::query::JelinekMercer,
            ir::rm::weighting::query::MultiplyQueryProbabilities>(IDENTIFIER);
        }

      };

    }

    AnswerWeightingFactory::AnswerWeightingFactory()
    {
      registerProviderClass<detail::DefaultDocumentWeightingProvider>();
      
      using namespace jerome::ir::rm::weighting;
      
      // text unigram
      registerModel("unigram", {
        AnswerWeightingFactory::PROVIDER_KEY, detail::DefaultDocumentWeightingProvider::IDENTIFIER
        , Weighting::IDENTIFIER_KEY, document::AddDocumentProbabilitiesConst::IDENTIFIER
        , Weighting::NAME_KEY, "answer"
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, document::CrossEntropyJelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "unigram"
        }
      });
      
      registerModel("bigram", {
        AnswerWeightingFactory::PROVIDER_KEY, detail::DefaultDocumentWeightingProvider::IDENTIFIER
        , Weighting::IDENTIFIER_KEY, document::AddDocumentProbabilitiesConst::IDENTIFIER
        , Weighting::NAME_KEY, "answer"
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, document::CrossEntropyJelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "unigram"
        }
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, document::CrossEntropyJelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "bigram"
        }
      });
      
      registerModel("unigram+id", {
        AnswerWeightingFactory::PROVIDER_KEY, detail::DefaultDocumentWeightingProvider::IDENTIFIER
        , Weighting::IDENTIFIER_KEY, document::AddDocumentProbabilitiesConst::IDENTIFIER
        , Weighting::NAME_KEY, "answer"
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, document::CrossEntropyJelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "unigram"
        }
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, document::CrossEntropyJelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "id"
        }
      });
     
      setDefaultModelKey("unigram");
    }

    QuestionWeightingFactory::QuestionWeightingFactory()
    {
			registerProviderClass<detail::DefaultQueryWeightingProvider>();

      using namespace jerome::ir::rm::weighting;
      
      // text unigram
      registerModel("unigram", {
        AnswerWeightingFactory::PROVIDER_KEY, detail::DefaultQueryWeightingProvider::IDENTIFIER
        , Weighting::IDENTIFIER_KEY, query::MultiplyQueryProbabilitiesConst::IDENTIFIER
        , Weighting::NAME_KEY, "question"
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, query::JelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "unigram"
        }
      });
      
      // text unigram
      registerModel("bigram", {
        AnswerWeightingFactory::PROVIDER_KEY, detail::DefaultQueryWeightingProvider::IDENTIFIER
        , Weighting::IDENTIFIER_KEY, query::MultiplyQueryProbabilitiesConst::IDENTIFIER
        , Weighting::NAME_KEY, "question"
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, query::JelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "unigram"
        }
        , Weighting::SUB_WEIGHTING_KEY, Record {
          Weighting::IDENTIFIER_KEY, query::JelinekMercer::IDENTIFIER
          , Weighting::NAME_KEY, "bigram"
        }
      });
      
      setDefaultModelKey("unigram");

    }

  }
}
