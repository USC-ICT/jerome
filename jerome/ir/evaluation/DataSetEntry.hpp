//
//  DataSetEntry.hpp
//
//  Created by Anton Leuski on 8/12/15.
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
