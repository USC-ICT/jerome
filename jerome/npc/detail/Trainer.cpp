//
//  Trainer.cpp
//
//  Created by Anton Leuski on 8/1/15.
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

#include <set>

#include <jerome/npc/detail/types_fwd.hpp>

#include <jerome/ir/evaluation/accumulators/statistics/accuracy.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/average_precision.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/f_measure.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/average.hpp>

#include <jerome/ir/evaluation.hpp>

#include <jerome/ir/training/TestSet.hpp>
#include <jerome/npc/detail/Ranker_impl.hpp>
#include <jerome/npc/detail/Ranker.hpp>

#include <jerome/ir/training/Trainer.hpp>

#include "Trainer.hpp"

namespace acc = jerome::ir::evaluation::accumulators;

namespace jerome {
  namespace npc {
    namespace detail {

      Trainer::progress_callback_type Trainer::default_progress_callback =
        [] (const Trainer::state_type&) {
        };

      Trainer::state_type::~state_type()
      {
      }

      class TrainerImplementation
      {
      public:
        TrainerImplementation() = default;
        TrainerImplementation(const Record& inModel)
        : mModel(inModel)
        {}

        virtual ~TrainerImplementation()
        {
        }

        virtual Result<Trainer::result_type>
        train(IndexedRanker& inRanker,
              const Trainer::progress_callback_type& callback) = 0;

        void setData(const Data& trainData, const Data& devData)
        {
          mTrainData = trainData;
          mDevData = devData;
        }

        Data trainData() const
        {
          return mTrainData;
        }

        Data devData() const
        {
          return mDevData;
        }

        const Record& model() const { return mModel; }
        void setModel(const Record& inModel) { mModel = inModel; }

      private:
        Data mTrainData;
        Data mDevData;
        Record mModel;
      };

#define COPY_VAL(KEY, OPT, FUN) \
{ auto optional_double = this->model().template at<double>(Trainer::KEY); \
if (optional_double) OPT.set_##FUN(*optional_double); \
}
      
      struct Algorithm {
        nlopt::algorithm algorithm;
        bool global;
        bool requiresLocal;
        Algorithm(nlopt::algorithm inAlgorithm
                  , bool inGlobal = true
                  , bool inRequiresLocal = false)
        : algorithm(inAlgorithm)
        , global(inGlobal)
        , requiresLocal(inRequiresLocal)
        {}
//        Algorithm(const Algorithm&) = default;
//        Algorithm(Algorithm&&) = default;
//        Algorithm& operator = (const Algorithm&) = default;
//        Algorithm& operator = (Algorithm&&) = default;
      };
      
#define MAP_ASSIGN(X) \
map.emplace(#X, Algorithm(nlopt::X))

#define MAP_ASSIGN_L(X) \
map.emplace(#X, Algorithm(nlopt::X, false))

#define MAP_ASSIGN_G_L(X) \
map.emplace(#X, Algorithm(nlopt::X, true, true))

      static StringMap<Algorithm> makeAlgorithms()
      {
        StringMap<Algorithm> map;
        MAP_ASSIGN(GN_DIRECT_L_RAND);
        MAP_ASSIGN(GN_DIRECT_L);
        MAP_ASSIGN(GN_DIRECT);
        MAP_ASSIGN(GN_DIRECT_NOSCAL);
        MAP_ASSIGN(GN_DIRECT_L_NOSCAL);
        MAP_ASSIGN(GN_DIRECT_L_RAND_NOSCAL);
        //      MAP_ASSIGN(GN_ORIG_DIRECT);
        //      MAP_ASSIGN(GN_ORIG_DIRECT_L);
        MAP_ASSIGN(GN_CRS2_LM);
        MAP_ASSIGN(GN_ISRES);
        MAP_ASSIGN_G_L(AUGLAG);
        MAP_ASSIGN_G_L(AUGLAG_EQ);
        MAP_ASSIGN_G_L(G_MLSL);
        MAP_ASSIGN_G_L(G_MLSL_LDS);

        MAP_ASSIGN_L(LN_BOBYQA);
        MAP_ASSIGN_L(LN_COBYLA);
        MAP_ASSIGN_L(LN_PRAXIS);
        MAP_ASSIGN_L(LN_NEWUOA);
        MAP_ASSIGN_L(LN_NEWUOA_BOUND);
        MAP_ASSIGN_L(LN_NELDERMEAD);
        MAP_ASSIGN_L(LN_SBPLX);
        MAP_ASSIGN_L(LN_AUGLAG);
        MAP_ASSIGN_L(LN_AUGLAG_EQ);

        return map;
      }
      
      static const StringMap<Algorithm>& algorithmsMap()
      {
        static StringMap<Algorithm> algorithmsMap = makeAlgorithms();
        return algorithmsMap;
      }
      
      List<String> Trainer::algorithms(AlgorithmKind kind)
      {
        List<String> list;
        for(const auto& i : algorithmsMap()) {
          switch (kind) {
            case ALL:
              list.push_back(i.first);
              break;
            case GLOBAL:
              if (i.second.global)
                list.push_back(i.first);
              break;
            case LOCAL:
              if (!i.second.global)
                list.push_back(i.first);
              break;
            case GLOBAL_REQUIRING_LOCAL:
              if (i.second.global && i.second.requiresLocal)
                list.push_back(i.first);
              break;
          }
        }
        std::sort(list.begin(), list.end());
        return list;
      }
      
      template <typename F>
      class TrainerImplementationTemplate
        : public TrainerImplementation
      {
        typedef TrainerImplementation parent_type;
        using parent_type::parent_type;
        typedef ir::training::Optimizer optimizer_type;

        struct objective_type
          : public jerome::ir::training::ObjectiveFunction<IndexedRanker,
            jerome::ir::evaluation::RankedListCollectorOne<
              Data::answer_type,
              F
              >>
        {
          typedef jerome::ir::training::ObjectiveFunction<IndexedRanker,
            jerome::ir::evaluation::RankedListCollectorOne<
              Data::answer_type,
              F
              >> parent_type;

          using parent_type::parent_type;
        };

				class state_type_impl : public Trainer::state_type {
				public:
					state_type_impl(optimizer_type& opt)
					: mSource(opt)
					{}
					
					double lastValue() const override
					{
						return mSource.lastValue();
					}
					
					jerome::math::parameters::value_vector lastArgument() const override
					{
						return mSource.lastArgument();
					}
					
					double elapsedTimeInSeconds() const override
					{
						return mSource.elapsedTimeInSeconds();
					}
					
					void stop() override
					{
						mSource.stop();
					}
					
				private:
					optimizer_type& mSource;
				};

        Result<Trainer::result_type>
        train(IndexedRanker& inRanker,
              const Trainer::progress_callback_type& callback) override
        {
          typedef IndexedRanker ranker_type;
          typedef typename ranker_type::test_set_type test_set_type;

          test_set_type devSet = inRanker.testSetWithData(this->devData());
          objective_type obj(inRanker, devSet);

          ir::range_vector  ranges = inRanker.ranges();

          nlopt::algorithm  algorithm = nlopt::GN_DIRECT_L_RAND;
          auto global_alg_name = this->model()
            .template at<String>(Trainer::G_ALGORITHM);
          
          bool requiresLocal = false;
          
          if (global_alg_name) {
            auto optional_algorithm = map_value_at(algorithmsMap(),
                                                   *global_alg_name);
            if (!optional_algorithm) {
              return Error("Unknown global optimization algorithm "
                           + *global_alg_name);
            }
            algorithm = optional_algorithm->algorithm;
            requiresLocal = optional_algorithm->requiresLocal;
          }
          
          optimizer_type gopt(algorithm, ranges);
          
          COPY_VAL(G_FTOL_REL, gopt, ftol_rel);
          COPY_VAL(G_FTOL_ABS, gopt, ftol_abs);
          COPY_VAL(G_XTOL_REL, gopt, xtol_rel);
          COPY_VAL(G_XTOL_ABS, gopt, xtol_abs);
					
          auto local_alg_name = this->model()
            .template at<String>(Trainer::L_ALGORITHM);
          if (local_alg_name) {
            auto optional_algorithm = map_value_at(algorithmsMap(),
                                                   *local_alg_name);
            if (!optional_algorithm || optional_algorithm->global) {
              return Error("Unknown local optimization algorithm "
                           + *local_alg_name);
            }
            
            optimizer_type lopt(optional_algorithm->algorithm, ranges);
            COPY_VAL(L_FTOL_REL, lopt, ftol_rel);
            COPY_VAL(L_FTOL_ABS, lopt, ftol_abs);
            COPY_VAL(L_XTOL_REL, lopt, xtol_rel);
            COPY_VAL(L_XTOL_ABS, gopt, xtol_abs);
            lopt.set_max_objective(obj);
            
            gopt.set_local_optimizer(lopt);
          } else if (requiresLocal) {
            return Error("Local optimization algorithm is required for "
                         + *global_alg_name);
          }
          
          gopt.set_max_objective(obj);
          gopt.set_callback([&callback] (optimizer_type& opt) {
						state_type_impl state(opt);
						callback(state);
          });

					std::vector<double> x = inRanker.values();
          std::vector<double> o = math::parameters::clumpToMean(x, ranges);

//          log::info() << x;
//          log::info() << obj.scoreValuePair(x);

					x = gopt.optimize(o);

//		log::info() << "switch";
//
//    optimizer_type lopt(nlopt::LN_BOBYQA, ranges);
//    lopt.set_ftol_rel(5.0e-4);
//    lopt.set_max_objective(obj);
//
//		x = lopt.optimize(x);

          Trainer::result_type  result;
          result.params = x;
          result.best_point = obj.scoreValuePair(x);

          if (this->model().at(Trainer::HACK_THRESHOLD, Bool()).value()) {
//            log::info() << "hacking score to "
//              << result.best_point.score() - 1;
           
            result.best_point = 
              ScoredObject<double>(result.best_point.score() - 1, 
                                   result.best_point.object());
          }
          
//          log::info() << x;
//          log::info() << result.best_point;

          return std::move(result);
        }

        // this function compares different optimization algorithms from the
        // NLopt
        // library. No optimization problem is the same, so they suggest running
        // one algorithm for a long time with high precision termination
        // conditions
        // or even time condition, and then run the other algorithms with the
        // discovered value as the "stopval" term ination criteria and count
        // the number of function evaluations needed to reach that goal.
        //
        // on the basic twins data, 50% dev set, GN_DIRECT_L_RAND is the
        // fastest.
        void compareOptimizationAlgorithms(IndexedRanker& inRanker)
        {
          ir::range_vector  ranges = inRanker.ranges();

          typename objective_type::test_set_type devSet =
            inRanker.testSetWithData(
              this->devData());

          std::vector<double> o = math::parameters::mean(ranges);

//          double stop_value = 0.381;

          for (auto galg : algorithmsMap()) {

            optimizer_type gopt(galg.second.algorithm, ranges);

//      gopt.set_xtol_abs(1e-2);
            gopt.set_ftol_abs(1e-3);

//			gopt.set_stopval(stop_value);

            if (!galg.second.requiresLocal) {

              objective_type obj(inRanker, devSet);

              gopt.set_max_objective(obj);

              log::info() << gopt.get_algorithm_name();

              try {
                gopt.optimize(o);
                log::info() << obj.evaluationCount();
              } catch (const std::exception& ex) {
                log::error() << "exception:" << ex.what();
              }

            } else {

              for (auto lalg : algorithmsMap()) {

                if (lalg.second.global) continue;
                
                objective_type obj(inRanker, devSet);

                optimizer_type lopt(lalg.second.algorithm, ranges);

//					lopt.set_stopval(stop_value);
//          lopt.set_xtol_abs(1e-4);
                lopt.set_ftol_rel(1e-4);
                lopt.set_max_objective(obj);

                gopt.set_local_optimizer(lopt);
                gopt.set_max_objective(obj);

                log::info() << gopt.get_algorithm_name()
                  << " w/ " << lopt.get_algorithm_name()
                ;

                try {
                  gopt.optimize(o);
                  log::info() << obj.evaluationCount();
                } catch (const std::exception& ex) {
                  log::error() << "exception:" << ex.what();
                }
              }

            }
          }
        }

      };

      Result<Trainer::result_type> Trainer::train(IndexedRanker& inRanker,
                                                  const progress_callback_type& callback)
      {
        return this->implementation().train(inRanker, callback);
      }

      Data Trainer::trainData() const
      {
        return this->implementation().trainData();
      }

      Data Trainer::devData() const
      {
        return this->implementation().devData();
      }

      void Trainer::setData(const Data& trainData, const Data& devData)
      {
        this->implementation().setData(trainData, devData);
      }

      Record Trainer::model() const
      {
        return this->implementation().model();
      }
      
      void Trainer::setModel(const Record& inModel)
      {
        this->implementation().setModel(inModel);
      }

    }
  }
}

#include <jerome/npc/factories/TrainerFactory.hpp>
#include <jerome/ir/evaluation/accumulators/statistics_fwd.hpp>
#include <jerome/ir/evaluation/accumulators/statistics_names.hpp>

namespace jerome {
  namespace npc {
    
    namespace acc = jerome::ir::evaluation::accumulators;
    
    namespace detail {
      
      template <typename F>
      struct DefaultTrainerProvider
      : public TrainerFactory::provider_type
      {
        static String identifier()
        {
          return String("jerome.trainer.") + acc::accumulator_name<F>::name();
        }
        
        Result<TrainerFactory::object_type>
        provide(const Record& inRecord) override
        {
          return Trainer(std::make_shared<TrainerImplementationTemplate<F>>(inRecord));
        }
      };
      
      template <typename F>
      static void registerProvider(TrainerFactory& factory)
      {
        factory.registerProviderClassForID<DefaultTrainerProvider<F>>
          (DefaultTrainerProvider<F>::identifier());
      }

      String Trainer::trainerFscoreID()
      {
        return DefaultTrainerProvider<acc::tag::fmeasure>::identifier();
      }
      
      String Trainer::trainerAveragePrecisionID()
      {
        return DefaultTrainerProvider<acc::tag::average_precision>::identifier();
      }
      
      String Trainer::trainerAccuracyID()
      {
        return DefaultTrainerProvider<acc::tag::accuracy>::identifier();
      }
      
    }
    
    TrainerFactory::TrainerFactory()
    {
      detail::registerProvider<acc::tag::average_precision>(*this);
      detail::registerProvider<acc::tag::accuracy>(*this);
      detail::registerProvider<acc::tag::fmeasure>(*this);
      
      registerModel("fscore", {
        TrainerFactory::PROVIDER_KEY, detail::Trainer::trainerFscoreID()
      });
      registerModel("average-precision", {
        TrainerFactory::PROVIDER_KEY, detail::Trainer::trainerAveragePrecisionID()
      });
      registerModel("accuracy", {
        TrainerFactory::PROVIDER_KEY, detail::Trainer::trainerAccuracyID()
      });

      setDefaultModelKey("fscore");
    }
    
  }
}
