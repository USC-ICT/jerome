//
//  statistics_names.hpp
//
//  Created by Anton Leuski on 8/18/15.
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
