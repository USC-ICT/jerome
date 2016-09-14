//
//  Trainer.cpp
//
//  Created by Anton Leuski on 8/1/15.
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

        virtual ~TrainerImplementation()
        {
        }

        virtual Result<Trainer::result_type> train(IndexedRanker& inRanker,
                                                   const Trainer::progress_callback_type& callback)
          = 0;

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

      private:
        Data mTrainData;
        Data mDevData;
      };

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

        Result<Trainer::result_type> train(IndexedRanker& inRanker,
                                           const Trainer::progress_callback_type& callback)
        override
        {
          typedef IndexedRanker ranker_type;
          typedef typename ranker_type::test_set_type test_set_type;

          test_set_type devSet = inRanker.testSetWithData(this->devData());
          objective_type obj(inRanker, devSet);

          ir::range_vector  ranges = inRanker.ranges();

          optimizer_type gopt(nlopt::GN_DIRECT_L_RAND, ranges);
          gopt.set_ftol_rel(5.0e-4);
					
          gopt.set_max_objective(obj);
          gopt.set_callback([&callback] (optimizer_type& opt) {
						state_type_impl state(opt);
						callback(state);
          });

          std::vector<double> o = math::parameters::mean(ranges);
					std::vector<double> x;

					x = inRanker.values();

//          std::cout << x << std::endl;
//          std::cout << obj.scoreValuePair(x) << std::endl;

					x = gopt.optimize(o);

//		std::cout << "switch" << std::endl;
//
//    optimizer_type lopt(nlopt::LN_BOBYQA, ranges);
//    lopt.set_ftol_rel(5.0e-4);
//    lopt.set_max_objective(obj);
//
//		x = lopt.optimize(x);

          Trainer::result_type  result;
          result.params = x;
          result.best_point = obj.scoreValuePair(x);

//          std::cout << x << std::endl;
//          std::cout << result.best_point << std::endl;

          return std::move(result);
        }

        // this function compares different optimization algorithms from the
        // NLopt
        // library. No optimization problem is the same, so they suggest running
        // one algorithm for a long time with high precision termination
        // conditions
        // or even time condition, and then run the other algorithms with the
        // discovered value as the "stopval" termaination criteria and count
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

          std::vector<nlopt::algorithm> globalAlgoithms = {
            nlopt::GN_DIRECT_L,
            nlopt::GN_DIRECT,
            nlopt::GN_DIRECT_L_RAND,
            nlopt::GN_DIRECT_NOSCAL,
            nlopt::GN_DIRECT_L_NOSCAL,
            nlopt::GN_DIRECT_L_RAND_NOSCAL,
//      nlopt::GN_ORIG_DIRECT,
//      nlopt::GN_ORIG_DIRECT_L,
            nlopt::GN_CRS2_LM,
//      nlopt::GN_MLSL,
//      nlopt::GN_MLSL_LDS,
            nlopt::GN_ISRES,
            nlopt::AUGLAG,
            nlopt::AUGLAG_EQ,
            nlopt::G_MLSL,
            nlopt::G_MLSL_LDS
          };

          std::set<nlopt::algorithm> algorithmRequiresLocalOptimizer = {
            nlopt::AUGLAG,
            nlopt::AUGLAG_EQ,
            nlopt::G_MLSL,
            nlopt::G_MLSL_LDS
          };

          std::vector<nlopt::algorithm> localAlgoithms = {
            nlopt::LN_BOBYQA,
            nlopt::LN_COBYLA,
            nlopt::LN_PRAXIS,
            nlopt::LN_NEWUOA,
            nlopt::LN_NEWUOA_BOUND,
            nlopt::LN_NELDERMEAD,
            nlopt::LN_SBPLX,
            nlopt::LN_AUGLAG,
            nlopt::LN_AUGLAG_EQ
          };

          std::vector<double> o = math::parameters::mean(ranges);

//          double stop_value = 0.381;

          for (auto galg : globalAlgoithms) {

            optimizer_type gopt(galg, ranges);

//      gopt.set_xtol_abs(1e-2);
            gopt.set_ftol_abs(1e-3);

//			gopt.set_stopval(stop_value);

            if (algorithmRequiresLocalOptimizer.find(galg) ==
                algorithmRequiresLocalOptimizer.end()) {

              objective_type obj(inRanker, devSet);

              gopt.set_max_objective(obj);

              std::cout
                << std::endl << gopt.get_algorithm_name();
              ;

              try {
                gopt.optimize(o);
                std::cout << std::endl << obj.evaluationCount();
              } catch (const std::exception& ex) {
                std::cout << std::endl << "exception:" << ex.what();
              }

            } else {

              for (auto lalg : localAlgoithms) {

                objective_type obj(inRanker, devSet);

                optimizer_type lopt(lalg, ranges);

//					lopt.set_stopval(stop_value);
//          lopt.set_xtol_abs(1e-4);
                lopt.set_ftol_rel(1e-4);
                lopt.set_max_objective(obj);

                gopt.set_local_optimizer(lopt);
                gopt.set_max_objective(obj);

                std::cout
                  << std::endl << gopt.get_algorithm_name()
                  << " w/ " << lopt.get_algorithm_name()
                ;

                try {
                  gopt.optimize(o);
                  std::cout << std::endl << obj.evaluationCount() << std::endl;
                } catch (const std::exception& ex) {
                  std::cout << std::endl << "exception:" << ex.what() <<
                    std::endl;
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

      Trainer Trainer::trainerFscore()
      {
        return Trainer(
          std::make_shared<TrainerImplementationTemplate<acc::tag::fmeasure>>());
      }

      Trainer Trainer::trainerAveragePrecision()
      {
        return Trainer(std::make_shared<TrainerImplementationTemplate<acc::tag::
               average_precision>>());
      }

      Trainer Trainer::trainerAccuracy()
      {
        return Trainer(
          std::make_shared<TrainerImplementationTemplate<acc::tag::accuracy>>());
      }

      void Trainer::setData(const Data& trainData, const Data& devData)
      {
        this->implementation().setData(trainData, devData);
      }

    }
  }
}