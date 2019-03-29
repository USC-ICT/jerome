//
//  Ranker_impl.hpp
//
//  Created by Anton Leuski on 8/25/15.
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

#ifndef __jerome_npc_detail_Ranker_impl_hpp__
#define __jerome_npc_detail_Ranker_impl_hpp__

#include <jerome/npc/model.hpp>
#include <jerome/ir/rm/cl/ranker.hpp>
#include <jerome/ir/training/Data.hpp>
#include <jerome/npc/detail/Ranker.hpp>

namespace jerome {
  namespace npc {
    namespace detail {

      using namespace jerome::ir;
      using namespace jerome::ir::rm;

      template <class Q, class D>
      class RankerImplementation
        : public jerome::ir::i::RankerImplementation<Q, D>
      {
        typedef jerome::ir::i::RankerImplementation<Q, D> parent_type;

      public:
        JEROME_INTERNAL_RANKER_TYPES(parent_type)
        typedef typename RankerInterface<
          query_type,
          document_type
          >::test_set_type test_set_type;

        using parent_type::parent_type;

//        RankerImplementation(const State& inState)
//          : mState(inState)
//        {
//        }

        virtual void train(Trainer& inTrainer,
                           const Trainer::progress_callback_type& callback) = 0;
        virtual test_set_type testSetWithData(const Data& data) const = 0;

        virtual Record model() const = 0;

      };

      template <typename R, typename QT = typename R::query_type>
      class RankerImplementationTemplate
        : public RankerImplementation<QT, typename R::document_type>
      {
        typedef RankerImplementation<
          QT,
          typename R::document_type
          > parent_type;

      public:
        JEROME_INTERNAL_RANKER_TYPES(parent_type)
        typedef typename parent_type::test_set_type test_set_type;

        typedef R ranker_type;

        RankerImplementationTemplate(const Record& inModel,
          const Data& inData)
          : mRanker(inModel, inData)
        {
          // note that we do not want to pass parameters to the base ranker constructor,
          // because if the actual ranker is classifier, the classifier template does not
          // know how to handle the param list in the constructor.
        }

        JEROME_INTERNAL_RANKER_REDIRECT_METHODS(base_ranker(), override)

      private:
        ranker_type mRanker;

      protected:
        ranker_type& base_ranker()
        {
          return mRanker;
        }

        const ranker_type& base_ranker() const
        {
          return mRanker;
        }

				Record parametersRecord() const
				{
					Record params;

					auto  titles = this->base_ranker().titles();
					auto  values = this->base_ranker().values();

					std::size_t index = 0;
					for (const auto& t : titles) {
						params.emplace(t, values[index++]);
					}
					
					return params;
				}
				
        void setParameters(const value_vector& inValues)
        {
          this->base_ranker().setValues(inValues);
        }
				
				Record model() const override
				{
					State::ranker_model_type model = base_ranker().model();
					model.remove(State::PARAMETERS_KEY);
					model.emplace(State::PARAMETERS_KEY, parametersRecord());
					return model;
				}
      };

      template <typename R>
      class IndexedRankerImplementationTemplate
        : public RankerImplementationTemplate<R, IndexedData::question_type>
      {
        typedef RankerImplementationTemplate<R,
          IndexedData::question_type> parent_type;

      public:
        JEROME_INTERNAL_RANKER_TYPES(parent_type)
        typedef typename parent_type::test_set_type test_set_type;
        using parent_type::parent_type;

        IndexedRankerImplementationTemplate(const Record& inModel,
          const Data& inData)
          : parent_type(inModel, inData)
        {
        }

        void train(Trainer& inTrainer,
                   const Trainer::progress_callback_type& callback) override
        {
          throw Exception("why are you calling this function?");
        }

        test_set_type testSetWithData(const Data& data) const override
        {
          return test_set_type(data, this->base_ranker().analyzer());
        }

      };

      template <typename R>
      class ClassifierImplementationTemplate
        : public RankerImplementationTemplate<jerome::ir::i::Classifier<R>>
      {
        typedef jerome::ir::i::Classifier<R> ranker_model_type;
        typedef RankerImplementationTemplate<ranker_model_type> parent_type;

      public:
        typedef typename parent_type::ranker_type ranker_type;
        typedef IndexedRankerImplementationTemplate<R>
          indexed_base_ranker_impl_type;

        JEROME_INTERNAL_RANKER_TYPES(parent_type)
        typedef typename parent_type::test_set_type test_set_type;

        using parent_type::parent_type;

        test_set_type testSetWithData(const Data& data) const override
        {
          return test_set_type(data);
        }

        void train(Trainer& inTrainer,
                   const Trainer::progress_callback_type& callback) override
        {
          value_vector values = this->base_ranker().values();
          int index = indexOfClassifierParam();
          values.erase(values.begin() + index);

          IndexedRanker rankerToTrain = IndexedRanker(
            std::make_shared<indexed_base_ranker_impl_type>(
              this->model(),
              inTrainer.trainData()));
					rankerToTrain.setValues(values);

          Result<Trainer::result_type> result = inTrainer.train(rankerToTrain,
            callback);

          if (result) {
            values = result.value().params;
            values.emplace(values.begin() + index,
              result.value().best_point.score());

            this->setParameters(values);
          }
        }

      private:
        int indexOfClassifierParam()
        {
          value_vector values = this->base_ranker().values();
          value_vector empty(values.size(), 0);
					this->base_ranker().setValues(empty);
					this->base_ranker().setThreshold(1);
					
					int index = ((int)values.size()) - 1;
          for (int i = 0; i < values.size(); ++i) {
            if (values[i] == 1) { index = i; break; }
					}
					this->base_ranker().setValues(values);
          return index;
        }

      };

      template <typename Base>
      using UtteranceRanker = RankerWithAnalyzer<
        Data::question_type,
        Base,
        HeapIndex
        >;

    }
  }
}

#endif // defined __jerome_npc_detail_Ranker_impl_hpp__
