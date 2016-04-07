//
//  Relevance.hpp
//
//  Created by Anton Leuski on 8/12/15.
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

#ifndef __jerome_ir_evaluation_Relevance_hpp__
#define __jerome_ir_evaluation_Relevance_hpp__

namespace jerome {
  namespace ir {
    namespace evaluation {
      template <typename D>
      struct Relevance
        : std::unary_function<D, bool>
      {
        typedef Set<D> RelevantSet;
        typedef Relevance<D> this_type;

        bool operator () (const D& inValue) const
        {
          return mRelevantSet.find(inValue) != mRelevantSet.end();
        }

        Relevance(const RelevantSet& inRelevantSet)
          : mRelevantSet(inRelevantSet)
        {
        }

        Relevance(const this_type& inSource)
          : mRelevantSet(inSource.mRelevantSet)
        {
        }

      private:
        const RelevantSet& mRelevantSet;
      };
    }
  }
}

#endif // defined __jerome_ir_evaluation_Relevance_hpp__
