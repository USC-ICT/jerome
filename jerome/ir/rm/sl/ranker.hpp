//
//  ranker.hpp
//
//  Created by Anton Leuski on 2/13/14.
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

#ifndef __jerome_ir_rm_sl_ranker_hpp__
#define __jerome_ir_rm_sl_ranker_hpp__

#include <jerome/types.hpp>
#include <jerome/ir/ranker.hpp>
#include <jerome/ir/rm/rm.hpp>
#include <jerome/ir/evaluation.hpp>

namespace jerome { namespace ir { namespace rm { namespace sl {

	
template <typename Q, typename A>
struct Ranker : public jerome::ir::rm::Ranker<Q, A, Ranker<Q,A>> {
	typedef Ranker<Q,A> this_type;
	typedef	jerome::ir::rm::Ranker<Q, A, this_type>		parent_type;
	
	JEROME_INTERNAL_RANKER_TYPES(parent_type)
		
	typedef typename A::weighting_type		document_weighting_type;
	typedef typename Q::weighting_type			query_weighting_type;
	
	template <class DocumentRange, class DocumentAnalyzer>
	void index(
			   const query_weighting_type& inQueryWeighting,
			   const document_weighting_type& inDocumentWeighting,
			   const DocumentRange& inDocuments,
				 const DocumentAnalyzer& inDocumentAnalyzer)
	{
		parent_type::index(inQueryWeighting, inDocumentWeighting);
		this->document().model().index(inDocuments, inDocumentAnalyzer);
	}
	
  template <typename DM, typename QM>
  auto prod(const DM& dm, const QM& qm) const
  -> decltype(jerome::prod(dm, qm))
  {
    return jerome::prod(dm, qm);
  }
  
  const typename A::context_type&	documentWeightingContext() 	const { return this->document().context(); }
	const typename A::context_type&	queryWeightingContext() 	const { return this->document().context(); }
	
	std::size_t countOfDocuments() const { return this->document().index().documentCount(); }
	std::size_t countOfQueries()   const { return 0; }
	
//	typedef typename DM::object_iterator_type	document_iterator_type;
//	document_iterator_type documentIterator() const { return mDocumentModel.objectIterator(); }

};


}}}}

#endif // defined __jerome_ir_rm_sl_ranker_hpp__
