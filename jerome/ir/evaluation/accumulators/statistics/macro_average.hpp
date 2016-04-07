//
//  macro_average.hpp
//
//  Created by Anton Leuski on 5/22/13.
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

#ifndef __jerome_ir_evaluation_accumulators_statistics_macro_average_hpp__
#define __jerome_ir_evaluation_accumulators_statistics_macro_average_hpp__

#include <jerome/ir/evaluation/accumulators/statistics/average.hpp>

// macro average of F computes an average of F values. We alias
// it directly to average implementation with a different name

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

        namespace tag {
          template <typename Feature>
          struct macro_average_of
            : public average_of<Feature>
          {};
        }

        namespace extract {
          BOOST_ACCUMULATORS_DEFINE_EXTRACTOR(tag, macro_average_of, (typename))
        }

        using extract::macro_average_of;
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_statistics_macro_average_hpp__