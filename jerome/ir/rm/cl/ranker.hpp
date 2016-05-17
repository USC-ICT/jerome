//
//  cl_ranker.hpp
//
//  Created by Anton Leuski on 2/2/12.
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

#ifndef __jerome_ir_rm_cl_ranker_hpp__
#define __jerome_ir_rm_cl_ranker_hpp__

#include <jerome/types.hpp>
#include <jerome/ir/evaluation.hpp>
#include <jerome/ir/ranker.hpp>
#include <jerome/ir/rm/rm.hpp>
#include <jerome/ir/rm/cl/link.hpp>

namespace jerome { namespace ir { namespace rm { namespace cl {
	
template <
	typename Q,
	typename A,
	typename L = BinaryLink
//	,
//	class QM = DocumentModel<QI>,
//	class DM = StoredDocumentModel<A,AI>
>
struct Ranker : public jerome::ir::rm::Ranker<Q, A, Ranker<Q,A,L>> {
	typedef Ranker<Q,A,L>									this_type;
	typedef	jerome::ir::rm::Ranker<Q, A, this_type>			parent_type;
	
	JEROME_INTERNAL_RANKER_TYPES(parent_type)
	
	typedef L												link_type;
	typedef List<link_type>					link_list_type;

	typedef typename A::weighting_type						document_weighting_type;
	typedef typename Q::weighting_type						query_weighting_type;
	
	template <class QueryRange, class DocumentRange, class LinkRange, class QueryAnalyzer, class DocumentAnalyzer>
	void index(
			   const query_weighting_type& inQueryWeighting,
			   const document_weighting_type& inDocumentWeighting,
			   const QueryRange& inQueries,
			   const DocumentRange& inDocuments,
			   const LinkRange& inLinks,
			   const QueryAnalyzer& inQueryAnalyzer,
			   const DocumentAnalyzer& inDocumentAnalyzer)
	{
		parent_type::index(inQueryWeighting, inDocumentWeighting);
		this->query().model().index(inQueries, inQueryAnalyzer);
		this->document().model().index(inDocuments, inDocumentAnalyzer);
		mLinks = makeIndexedLinkList(inQueries, inDocuments, inLinks);
		computeIndexesOfQueriesLinkedToDocumentWithIndex();
	}
	
  template <typename DM, typename QM>
  WeightMatrix prod(const DM& dm, const QM& qm) const
  {
    MatrixSize dm_size(dm);
    MatrixSize qm_size(qm);
    MatrixSize p_size;
    p_size.rowCount = dm_size.rowCount;
    p_size.columnCount = qm_size.columnCount;
    
    WeightMatrix p = WeightMatrixZero(p_size);
    
    for(std::size_t i = 0, n = p_size.columnCount; i < n; ++i) {
      double sum = 0;
      for(const auto link : links()) {
        sum += qm(link.queryIndex, i);
      }
      for(const auto link : links()) {
        jerome::column(p, i) += jerome::column(dm, link.documentIndex)
          * (qm(link.queryIndex, i) / sum);
      }
    }
    
    return p;
  }
		
	link_list_type& links()					{ return mLinks; }
	const link_list_type& links() const		{ return mLinks; }
	
	const typename A::context_type&	documentWeightingContext() 	const { return this->document().context(); }
	const typename Q::context_type&	queryWeightingContext() 	const { return this->query().context(); }
	
	std::size_t countOfDocuments() const { return this->document().index().documentCount(); }
	std::size_t countOfQueries()   const { return this->query().index().documentCount(); }
	
private:
	void computeIndexesOfQueriesLinkedToDocumentWithIndex() {
		mIndexesOfQueriesLinkedToDocumentWithIndex.clear();
		for(std::size_t i = 0, n = this->document().index().documentCount(); i < n; ++i) {
			mIndexesOfQueriesLinkedToDocumentWithIndex.push_back(std::vector<std::size_t>());
		}
		for(std::size_t i = 0, n = links().size(); i < n; ++i) {
			mIndexesOfQueriesLinkedToDocumentWithIndex[links()[i].documentIndex].push_back(i);
		}
	}
	
	link_list_type		mLinks;
	std::vector<std::vector<std::size_t>>		mIndexesOfQueriesLinkedToDocumentWithIndex;
};

}}}}

#endif // defined __jerome_ir_rm_cl_ranker_hpp__
