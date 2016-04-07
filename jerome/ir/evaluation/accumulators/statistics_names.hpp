//
//  statistics_names.hpp
//
//  Created by Anton Leuski on 8/18/15.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_names_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_names_hpp__

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        template <typename T>
        struct accumulator_name
        {
          static String name()
          {
            return "unnamed";
          }

        };

#define JEROME_ACC_NAME(TAG, NAME) \
  template <> \
  struct accumulator_name<tag::TAG> \
  { \
    static String name() \
    { \
      return NAME; \
    } \
  };

#define JEROME_ACC_NAME_W_FEATURE(TAG, NAME) \
  template <typename F> \
  struct accumulator_name<tag::TAG<F>> \
  { \
    static String name() \
    { \
      return String(NAME) + accumulator_name<F>::name(); \
    } \
  };

        JEROME_ACC_NAME(accuracy, "accuracy")
        JEROME_ACC_NAME(average_precision, "averagePrecision")
        JEROME_ACC_NAME(average, "mean")
        JEROME_ACC_NAME_W_FEATURE(average_of, "mean_")
        JEROME_ACC_NAME(fmeasure, "fScore")
        JEROME_ACC_NAME(is_relevant, "is_relevant")
        JEROME_ACC_NAME_W_FEATURE(macro_average_of, "macro_average_")
        JEROME_ACC_NAME_W_FEATURE(micro_average_of, "micro_average_")
        JEROME_ACC_NAME(non_relevant_count, "nonRelevantCount")
        JEROME_ACC_NAME(pfalsealarm, "pFalseAlarm")
        JEROME_ACC_NAME(pmiss, "pMiss")
        JEROME_ACC_NAME(precision, "precision")
        JEROME_ACC_NAME(recall, "recall")
        JEROME_ACC_NAME(relevant_count, "relevantCount")
        JEROME_ACC_NAME(relevant_size, "relevantSetSize")
        JEROME_ACC_NAME(roc_area, "rocArea")
        JEROME_ACC_NAME(sample_count, "count")
        JEROME_ACC_NAME(retrieved_count, "retrievedCount")
        JEROME_ACC_NAME(total_size, "collectionSize")
        JEROME_ACC_NAME_W_FEATURE(total_of, "sum_")
        JEROME_ACC_NAME(total, "sum")

      }
    }
  }
}

#endif // defined __jerome_ir_evaluation_accumulators_statistics_names_hpp__
