//
//  WeightingFactory.cpp
//
//  Created by Anton Leuski on 9/1/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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
