//
//  link.hpp
//
//  Created by Anton Leuski on 7/23/14.
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

#ifndef __jerome_ir_rm_cl_link_hpp__
#define __jerome_ir_rm_cl_link_hpp__


namespace jerome { namespace ir { namespace rm { namespace cl {
	
	typedef uint32_t									index_type;
	
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

  template <typename Range>
  Map<typename Range::value_type, index_type>
  map_range(const Range& inRange)
  {
    typedef Map<typename Range::value_type, index_type> result_type;
    index_type        index = 0;
    result_type       omap;
    for(const auto& o : inRange) {
      omap.emplace(o, index++);
    }
    return omap;
  }
  
	template <class QueryRange,
    class DocumentRange,
    class LinkRange,
    typename L = BinaryLink>
	List<L>
	makeIndexedLinkList(const QueryRange& inQueries,
                      const DocumentRange& inDocuments,
                      const LinkRange& inLinks)
	{
		auto	qmap = map_range(inQueries);
		auto	dmap = map_range(inDocuments);
		
		List<L>	result;
		result.reserve(inLinks.size());
		
		for(const auto& l : inLinks) {
			auto	qiter = qmap.end();
			auto	diter = dmap.end();
			
			for(const auto& x : l.utterances()) {
        if (qiter == qmap.end()) qiter = qmap.find(x);
				if (diter == dmap.end()) diter = dmap.find(x);
			}
			
			if (qiter != qmap.end() && diter != dmap.end()) {
				result.push_back(L(qiter->second, diter->second));
				//								std::cout << "good link " << qiter->second << " " << diter->second << "\n";
			} else {
        // it's possible that one of the objects connected by the link
        // is not in the corresponding object set
        //        std::cout << "bad link" << std::endl;
			}
			
		}
		return result;
	}
}}}}
	
#endif // defined __jerome_ir_rm_cl_link_hpp__
