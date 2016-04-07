//
//  statistics_fwd.hpp
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
