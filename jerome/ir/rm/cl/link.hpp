//
//  link.hpp
//
//  Created by Anton Leuski on 7/23/14.
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

#ifndef __jerome_ir_rm_cl_link_hpp__
#define __jerome_ir_rm_cl_link_hpp__


namespace jerome { namespace ir { namespace rm { namespace cl {
	
	typedef uint16_t									index_type;
	
	struct BinaryLink {
		index_type		queryIndex;
		index_type		documentIndex;
		BinaryLink(index_type inQueryIndex, index_type inDocumentIndex) : queryIndex(inQueryIndex), documentIndex(inDocumentIndex) {}
		
		struct QueryIndex {
			index_type operator() (const BinaryLink& l) const { return l.queryIndex; }
		};
		struct DocumentIndex {
			index_type operator() (const BinaryLink& l) const { return l.documentIndex; }
		};
	};

	inline std::ostream& operator << (std::ostream& outs, const BinaryLink& obj)
	{
		return outs << "(" << obj.queryIndex << "," << obj.documentIndex << ")";
	}

	template <class QueryRange, class DocumentRange, class LinkRange, typename L = BinaryLink>
	List<L>
	makeIndexedLinkList(const QueryRange& inQueries, const DocumentRange& inDocuments, const LinkRange& inLinks)
	{
		typedef Map<typename QueryRange::value_type, index_type>					query_map_type;
		typedef Map<typename DocumentRange::value_type, index_type>					document_map_type;
		
		index_type			query_index = 0;
		query_map_type		qmap;
		for(const auto& q : inQueries) {
			qmap.emplace(q, query_index++);
		}
		
		index_type			document_index = 0;
		document_map_type	dmap;
		for(const auto& d : inDocuments) {
			dmap.emplace(d, document_index++);
		}
		
		List<L>	result;
		result.reserve(inLinks.size());
		
		for(const auto& l : inLinks) {
			typename query_map_type::const_iterator		qiter = qmap.end();
			typename document_map_type::const_iterator	diter = dmap.end();
			
			for(const auto& x : l.utterances()) {
				if (qiter == qmap.end()) qiter = qmap.find(x);
				if (diter == dmap.end()) diter = dmap.find(x);
			}
			
			if (qiter != qmap.end() && diter != dmap.end()) {
				result.push_back(L(qiter->second, diter->second));
				//								std::cout << "good link " << qiter->second << " " << diter->second << "\n";
			} else {
				//								std::cout << "bad link\n";
			}
			
		}
//		std::cout << inLinks.size() << " " << result.size() << " " << result << std::endl;
		
		return result;
	}
}}}}
	
#endif // defined __jerome_ir_rm_cl_link_hpp__
