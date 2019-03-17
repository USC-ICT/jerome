//
//  Trainer.cpp
//
//  Created by Anton Leuski on 9/18/15.
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

#include "Trainer.hpp"

namespace jerome {
  namespace ir {
    namespace training {

      Optimizer::Optimizer(nlopt::algorithm inAlgorithm,
        const range_vector& inRanges)
        : parent_type(inAlgorithm, (unsigned)inRanges.size())
        , mObjectiveFunction([] (const std::vector<double>&) -> double {
						return 0;
					})
        , mCallback([] (const Optimizer&) {
					})
      {
        std::vector<double> lb;
        lb.reserve(inRanges.size());
        std::vector<double> ub;
        ub.reserve(inRanges.size());

        for (const RangeDomain& r : inRanges) {
          lb.push_back(r.minimum());
          ub.push_back(r.maximum());
        }

        set_lower_bounds(lb);
        set_upper_bounds(ub);
      }

      double Optimizer::lastValue() const
      {
        return mLastValue;
      }
      std::vector<double> Optimizer::lastArgument() const
      {
        return mLastArgument;
      }
      double Optimizer::elapsedTimeInSeconds() const
      {
        time_point_type end = time_point_type::clock::now();
        std::chrono::duration<double> elapsed_seconds = end - mStartTime;
        return elapsed_seconds.count();
      }
      void Optimizer::stop()
      {
        force_stop();
      }

      std::vector<double> Optimizer::optimize(const std::vector<double>& x0)
      {
				std::vector<double> x(x0);
				mStartTime = time_point_type::clock::now();
				try {
					double f = 0;
					parent_type::optimize(x, f);
					return x;
				} catch (const nlopt::forced_stop& ex) {
					return x;
        } catch (const nlopt::roundoff_limited& ex) {
          std::cerr
            << std::endl
            << "Optimization halted because roundoff errors limited progress."
            << std::endl;
          // NLOPT reference says that "the optimization still typically
          // returns a useful result", so we return the value out.
          return x;
				}
      }

      double Optimizer::applyObjective(const std::vector<double>& x)
      {
        mLastArgument = x;
        mLastValue  = mObjectiveFunction(x);
        mCallback(*this);
        return mLastValue;
      }

      double Optimizer::nlopt_objective(const std::vector<double>& x,
                                        std::vector<double>& grad,
                                        void * my_func_data)
      {
        return (*static_cast<Optimizer*>(my_func_data)).applyObjective(x);
      }

    }
  }
}
