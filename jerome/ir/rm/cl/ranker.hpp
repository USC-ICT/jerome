//
//  cl_ranker.hpp
//
//  Created by Anton Leuski on 2/2/12.
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

#ifndef __jerome_ir_rm_cl_ranker_hpp__
#define __jerome_ir_rm_cl_ranker_hpp__

#include <jerome/types.hpp>
#include <jerome/ir/evaluation.hpp>
#include <jerome/ir/lookup.hpp>
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
	
	template <
    class QueryRange
  , class DocumentRange
  , class LinkRange
  , class QueryAnalyzer
  , class DocumentAnalyzer>
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
      for(const auto& link : links()) {
        sum += qm(link.queryIndex, i);
      }
      for(const auto& link : links()) {
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
	
  result_type lookup(const query_type& inQuery) const
  {
    result_type ranked_list(inQuery.documentCount());
    auto doc_lists = jerome::ir::lookup(inQuery, this->query().index());
    
    for(std::size_t i = 0, n = doc_lists.size(); i < n; ++i) {
      if (doc_lists[i].empty()) {
        continue;
      }
      
      // if multiple queires match and same documents are linked to those
      // queries, we only want a single instance of each document to appear
      // in the ranked list.
      Set<std::size_t>  docs;
      for(std::size_t j = 0, m = doc_lists[i].size(); j < m; ++j) {
        for(const auto& x : mIndexesOfDocumentsLinkedToQueryWithIndex[doc_lists[i][j]]) {
          docs.insert(x);
        }
      }
      List<std::size_t> sortedDocs(std::begin(docs), std::end(docs));
      std::sort(std::begin(sortedDocs), std::end(sortedDocs));

      for(const auto& x : sortedDocs) {
        // use 0 to indicate that this is a perfact match
        ranked_list[i].push_back(typename ranked_list_type::value_type(0,
                  *(this->document().model().objectIterator()+x)));
      }
    }
    
    return ranked_list;
  }
  
private:
	void computeIndexesOfQueriesLinkedToDocumentWithIndex() {
		mIndexesOfQueriesLinkedToDocumentWithIndex.clear();
		for(std::size_t i = 0, n = this->document().index().documentCount(); i < n; ++i) {
			mIndexesOfQueriesLinkedToDocumentWithIndex.push_back(std::vector<std::size_t>());
		}
    
    mIndexesOfDocumentsLinkedToQueryWithIndex.clear();
    for(std::size_t i = 0, n = this->query().index().documentCount(); i < n; ++i) {
      mIndexesOfDocumentsLinkedToQueryWithIndex.push_back(std::vector<std::size_t>());
    }
    
		for(std::size_t i = 0, n = links().size(); i < n; ++i) {
			mIndexesOfQueriesLinkedToDocumentWithIndex[links()[i].documentIndex].push_back(links()[i].queryIndex);
      mIndexesOfDocumentsLinkedToQueryWithIndex[links()[i].queryIndex].push_back(links()[i].documentIndex);
		}
	}
	
	link_list_type		mLinks;
	std::vector<std::vector<std::size_t>>		mIndexesOfQueriesLinkedToDocumentWithIndex;
  std::vector<std::vector<std::size_t>>		mIndexesOfDocumentsLinkedToQueryWithIndex;
};

}}}}

#endif // defined __jerome_ir_rm_cl_ranker_hpp__
