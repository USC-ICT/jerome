//
//  ranker.hpp
//
//  Created by Anton Leuski on 2/13/14.
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
