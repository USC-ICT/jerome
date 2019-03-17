//
//  TestSet.hpp
//
//  Created by Anton Leuski on 7/31/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_training_TestSet_hpp__
#define __jerome_ir_training_TestSet_hpp__

#include <boost/range/adaptor/transformed.hpp>

#include <jerome/ir/evaluation.hpp>
#include <jerome/ir/rm/cl/link.hpp>

namespace jerome {
  namespace ir {
    namespace training {

			namespace detail {
				template <class Q, class D>
				using TestSetStorage = jerome::List<
					jerome::ir::evaluation::DataSetEntry<Q, D>
				>;
			}
			
      template <class Q, class D>
			struct TestSet : public ReferenceClassInterface<detail::TestSetStorage<Q,D>>{
        typedef detail::TestSetStorage<Q,D> entry_list_type;
				typedef ReferenceClassInterface<detail::TestSetStorage<Q,D>> parent_type;
				typedef typename parent_type::implementation_type implementation_type;
        typedef typename entry_list_type::value_type value_type;

        template <class QueryRange, class DocumentRange, class LinkRange>
        TestSet(const QueryRange& inQueries, const DocumentRange& inDocuments,
          const LinkRange& inLinks)
				: parent_type(std::make_shared<implementation_type>())
        {
          init(inQueries, inDocuments, inLinks);
        }

        template <class Data>
        TestSet(const Data& inData)
				: parent_type(std::make_shared<implementation_type>())
        {
          init(inData.questions(), inData.answers(),
            jerome::ir::rm::cl::makeIndexedLinkList(
              inData.questions(), inData.answers(), inData.links()));
        }

        template <class Data, class A>
        TestSet(const Data& inData, const A& inAnalyzer)
				: parent_type(std::make_shared<implementation_type>())
        {
          init(inData.questions() | boost::adaptors::transformed(inAnalyzer),
            inData.answers(),
            jerome::ir::rm::cl::makeIndexedLinkList(
              inData.questions(), inData.answers(), inData.links()));
        }

        const entry_list_type& entries() const
        {
          return this->implementation();
        }

			private:

				entry_list_type& entries()
				{
					return this->implementation();
				}

       template <class QueryRange, class DocumentRange, class LinkRange>
        void init(const QueryRange& inQueries, const DocumentRange& inDocuments,
                  const LinkRange& inLinks)
        {
          typedef typename jerome::ir::evaluation::DataSetEntry<
              typename QueryRange::value_type,
              typename DocumentRange::value_type> dse_type;
          typedef uint32_t index_type;

          entries().reserve(boost::size(inQueries));
          index_type i = 0;
          for (const auto& x : inQueries) {
            entries().push_back(dse_type(i++, x));
          }

          for (const auto& l : inLinks) {
            entries()[l.queryIndex].relevantSet().insert(
              inDocuments[l.documentIndex]);
          }
        }

      };
    }
  }
}

#endif // defined __jerome_ir_training_TestSet_hpp__
