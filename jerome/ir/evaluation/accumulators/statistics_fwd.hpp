//
//  statistics_fwd.hpp
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_fwd_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_fwd_hpp__

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace tag {
          struct accuracy;
          struct average_precision;
          template <typename Feature>
          struct average_of;
          struct average;
          struct fmeasure;
          template <typename F>
          struct function;
          struct is_relevant;
          template <typename Feature>
          struct macro_average_of;
          template <typename Feature>
          struct micro_average_of;
          struct non_relevant_count;
          struct pfalsealarm;
          struct pmiss;
          struct precision;
          template <typename Q>
          struct query;
          struct recall;
          struct relevant_count;
          struct relevant_set;
          struct relevant_size;
          struct roc_area;
          struct sample_count;
          struct retrieved_count;
          struct total_size;
          template <class Feature>
          struct total_of;
          struct total;
          struct vector;
          template <typename Feature>
          struct vector_of;
          template <typename Feature>
          struct map_of;
        }

        namespace impl {
          struct accuracy_impl;
          struct average_precision_impl;
          template <typename Sample, typename Feature>
          struct average_impl;
          template <typename Sample>
          struct average_impl1;
          struct fmeasure_impl;
          template <typename Sample, typename F>
          struct function_impl;
          struct is_relevant_impl;
          struct non_relevant_count_impl;
          struct pfalsealarm_impl;
          struct pmiss_impl;
          struct precision_impl;
          template <typename Q>
          struct query_impl;
          struct recall_impl;
          struct relevant_count_impl;
          template <typename Sample>
          struct relevant_set_impl;
          struct relevant_size_impl;
          struct roc_area_impl;
          struct sample_count_impl;
          struct retrieved_count_impl;
          struct total_size_impl;
          template <typename Sample, typename Feature>
          struct total_impl;
          template <typename Sample>
          struct total_impl2;
          template <
            typename Sample,
            typename Func,
            typename Element
            >
          struct vector_impl;
          template <typename Sample, typename Feature>
          struct map_impl;
        }

      }
    }
  }
}
#endif // defined __jerome_ir_evaluation_accumulators_statistics_fwd_hpp__
