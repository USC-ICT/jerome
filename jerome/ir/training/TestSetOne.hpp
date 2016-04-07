//
//  TestSetOne.hpp
//
//  Created by Anton Leuski on 7/31/15.
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

#ifndef __jerome_ir_training_TestSetOne_hpp__
#define __jerome_ir_training_TestSetOne_hpp__

#include <jerome/ir/training/TestSet.hpp>

namespace jerome {
  namespace ir {
    namespace training {

      template <class Q, class D, class Index>
      struct TestSetOne : public TestSet<Q, D> {
        typedef TestSet<Q, D> parent_type;
        typedef typename parent_type::entry_list_type entry_list_type;

        template <class QueryRange, class DocumentRange, class LinkRange>
        TestSetOne(const QueryRange& inQueries,
                   const DocumentRange& inDocuments, const LinkRange& inLinks)
        : parent_type(inQueries, inDocuments, inLinks) {
          init();
        }

        template <class Data>
        TestSetOne(const Data& inData)
        : parent_type(inData) {
          init();
        }

        template <class Data, class A>
        TestSetOne(const Data& inData, const A& inAnalyzer)
        : parent_type(inData, inAnalyzer) {
          init();
        }

        const Index& index() const { return mIndex; }

       private:
        void init() {
          for (auto& dse : this->entries()) {
            mIndex.addIndex(dse.query());
          }
        }

        Index mIndex;
      };
    }
  }
}

#endif // defined __jerome_ir_training_TestSetOne_hpp__
