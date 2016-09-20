//
//  Trainer.hpp
//
//  Created by Anton Leuski on 7/31/15.
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

#ifndef __jerome_npc_detail_Trainer_hpp__
#define __jerome_npc_detail_Trainer_hpp__

#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/math/parameters/types.hpp>
#include <jerome/math/scored.hpp>

namespace jerome {
  namespace npc {
    
    namespace detail {
      
      template <typename F>
      struct DefaultTrainerProvider;
      class TrainerImplementation;

      class Trainer
        : public ReferenceClassInterface<TrainerImplementation>
      {
      public:
        typedef ReferenceClassInterface<TrainerImplementation> parent_type;
        using parent_type::parent_type;

        class state_type
        {
				public:
          virtual ~state_type();
          virtual double lastValue() const = 0;
          virtual jerome::math::parameters::value_vector lastArgument() const = 0;
          virtual double elapsedTimeInSeconds() const = 0;
          virtual void stop() = 0;
        };

        // returns true if you want to continue training
        typedef std::function<void (state_type&)> progress_callback_type;
        static progress_callback_type default_progress_callback;

        struct result_type
        {
          jerome::math::parameters::value_vector params;
          jerome::math::ScoredObject<double>  best_point;
        };

        Result<result_type> train(IndexedRanker& inRanker,
                                  const progress_callback_type& callback
                                    = default_progress_callback);
        Data trainData() const;
        Data devData() const;

        void setData(const Data& trainData, const Data& devData);
        
        Record model() const;
        void setModel(const Record& inModel);

        static String trainerFscoreID();
        static String trainerAveragePrecisionID();
        static String trainerAccuracyID();
        
        template <typename F>
        friend struct DefaultTrainerProvider;
        
        static constexpr const char* G_FTOL_REL = "global-function-relative-tolerance";
        static constexpr const char* G_FTOL_ABS = "global-function-absolute-tolerance";
        static constexpr const char* G_XTOL_REL = "global-parameter-relative-tolerance";
        static constexpr const char* G_XTOL_ABS = "global-parameter-absolute-tolerance";
        static constexpr const char* G_ALGORITHM = "global-algorithm";
        
        static constexpr const char* L_FTOL_REL = "local-function-relative-tolerance";
        static constexpr const char* L_FTOL_ABS = "local-function-absolute-tolerance";
        static constexpr const char* L_XTOL_REL = "local-parameter-relative-tolerance";
        static constexpr const char* L_XTOL_ABS = "local-parameter-absolute-tolerance";
        static constexpr const char* L_ALGORITHM = "local-algorithm";
        
        static List<String> globalAlgorithms();
        static List<String> localAlgorithms();
        static List<String> globalAlgorithmsRequiringLocalOptimizer();
        
      };

    }
  }
}

#endif // defined __jerome_npc_detail_Trainer_hpp__
