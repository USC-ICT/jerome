//
//  DataSetEntry.hpp
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

#ifndef __jerome_ir_evaluation_DataSetEntry_hpp__
#define __jerome_ir_evaluation_DataSetEntry_hpp__

#include <jerome/ir/evaluation/Relevance.hpp>

namespace jerome {
  namespace ir {
		namespace evaluation {
      template <typename Q, typename D>
      class DataSetEntry
      {
      public:
        typedef D document_type;
        typedef Set<D> RelevantSet;
        typedef std::size_t identifier_type;
        typedef Q query_type;

      private:
        identifier_type mID;
        RelevantSet mRelevantSet;
        query_type mQuery;

      public:
        DataSetEntry(identifier_type inID, const query_type& inQuery)
          : mID(inID), mQuery(inQuery)
        {
        }

        identifier_type identifier() const
        {
          return mID;
        }

        RelevantSet& relevantSet()
        {
          return mRelevantSet;
        }

        query_type& query()
        {
          return mQuery;
        }

        const RelevantSet& relevantSet() const
        {
          return mRelevantSet;
        }

        const query_type& query() const
        {
          return mQuery;
        }

        Relevance<D> relevance() const
        {
          return Relevance<D>(relevantSet());
        }

      };
		}
	}
}

#endif // defined __jerome_ir_evaluation_DataSetEntry_hpp__
