//
//  Trainer.cpp
//
//  Created by Anton Leuski on 9/18/15.
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