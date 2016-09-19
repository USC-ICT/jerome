//
//  RankerFactory.cpp
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

#include <jerome/npc/detail/Ranker_impl.hpp>
#include <jerome/ir/rm/AnyWeighting.hpp>

#include "WeightingFactory.hpp"
#include "AnalyzerFactory.hpp"

#include "RankerFactory.hpp"

namespace jerome {
  namespace npc {

    namespace detail {

      using UtteranceCLRankerBase = jerome::ir::rm::cl::Ranker<
        Desc<
          typename IndexedData::question_type,
					QuestionWeightingFactory::object_type,
          HeapIndex,
          DocumentModel<typename IndexedData::question_type>
          >,
        Desc<
          typename IndexedData::answer_type,
          AnswerWeightingFactory::object_type,
          HeapIndex,
          StoredDocumentModel<typename IndexedData::answer_type, HeapIndex>
          >
        >;

      class UtteranceCLRanker
        : public UtteranceRanker<UtteranceCLRankerBase>
        , public UtteranceCLRankerModel
      {
      public:
        UtteranceCLRanker() = default;
        UtteranceCLRanker(const State::ranker_model_type& inModel,
          const Data& data)
        {
          auto doc_weigh = AnswerWeightingFactory::sharedInstance()
          .make(inModel.at(ANSWER_WEIGHTING_KEY,
                           AnswerWeightingFactory::sharedInstance().predefinedModels().find("unigram+id")->second));
					mModel.emplace(ANSWER_WEIGHTING_KEY, doc_weigh.value().model());

          auto qry_weigh = QuestionWeightingFactory::sharedInstance()
            .make(inModel.at(QUESTION_WEIGHTING_KEY,
                             QuestionWeightingFactory::sharedInstance().defaultModel()));
					mModel.emplace(QUESTION_WEIGHTING_KEY, qry_weigh.value().model());

          auto doc_analyzer = AnalyzerFactory::sharedInstance()
            .make(inModel.at(ANSWER_ANALYZER_KEY,
                             AnalyzerFactory::sharedInstance().predefinedModels().find("text-unigram+id")->second));
					mModel.emplace(ANSWER_ANALYZER_KEY, doc_analyzer.value().model());

          auto qry_analyzer = AnalyzerFactory::sharedInstance()
            .make(inModel.at(QUESTION_ANALYZER_KEY,
                             AnalyzerFactory::sharedInstance().defaultModel()));
					mModel.emplace(QUESTION_ANALYZER_KEY, qry_analyzer.value().model());

          this->index(
                      qry_weigh.value()
                      , doc_weigh.value()
                      , data.questions()
                      , data.answers()
                      , data.links()
                      , qry_analyzer.value()
                      , doc_analyzer.value()
                      );
          this->analyzer() = qry_analyzer.value();
        }
				
				Record model() const
        { return mModel; }
				
			private:
				Record mModel;
      };

    }

    RankerFactory::RankerFactory()
    {
			registerProvider<detail::UtteranceCLRanker>();
    }

    Result<RankerFactory::object_type>
    RankerFactory::make(const Record& inModel,
                        const detail::Data& inData,
                        const math::parameters::value_vector& inParams)
    {
      return parent_type::make(inModel.at<String>(parent_type::PROVIDER_KEY),
                               inModel, inData, inParams);
    }


  }
}
