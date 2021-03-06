//
//  lookup.hpp
//  jerome
//
//  Created by Anton Leuski on 12/15/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_lookup_hpp__
#define __jerome_ir_lookup_hpp__

#include <jerome/types.hpp>
#include <jerome/ir/collection.hpp>

namespace jerome { namespace ir {

  namespace detail {
    struct Flags {
      
      typedef std::size_t size_type;
      
      Flags(size_type inCount)
      : mFlags(inCount, false)
      , mCount(inCount)
      {}
      
      void set(size_type inIndex)
      {
        if (!mFlags[inIndex]) {
          mFlags[inIndex] = true;
          mCount -= 1;
        }
      }
      
      bool isAllSet() const { return mCount == 0; }
      bool isSet(size_type inIndex) const { return mFlags[inIndex]; }
      
      template <class Term>
      void setFrom(const Term& term)
      {
        for_each(term.tfs(),
                 [&,this](const typename Term::size_type& i,
                          const typename Term::freq_type& v)
                 {
                   this->set(i);
                 });
      }
      
    private:
      List<bool> mFlags;
      size_type mCount;
    };
  }
  
  template <class QueryIndex, class TargetIndex>
  List<List<std::size_t>> lookup(const QueryIndex& inQuery,
                                 const TargetIndex& inIndex)
  {
    typedef std::size_t doc_id;
    typedef List<doc_id> doc_id_set_type;
    typedef List<doc_id> doc_id_list_type;
    
    List<doc_id_list_type> doc_lists(inQuery.documentCount(), doc_id_list_type());
    detail::Flags flags(inQuery.documentCount());
    
    typedef typename TargetIndex::Term::size_type term_index_type;
    typedef typename TargetIndex::Term::freq_type term_freq_type;
    
    for(const auto& field : inQuery.fields()) {
      try {
        // find the field in target index. throws an exception
        const auto& indexField(inIndex.findField(field.first));
        
        for(const auto& term : field.second.terms()) {
          const auto& termText(term.first);
          const auto& termInfo(term.second);
          
          // find the query term in the target index
          const auto indexTerm = indexField.findTerm(termText);
          if (indexTerm.isMissing()) {
            // all the queries with this term cannot be matched.
            flags.setFrom(termInfo);
            if (flags.isAllSet()) {
              return doc_lists;
            }
            continue;
          }
          
          // index all documents in the target index by TF of the term
          Map<term_freq_type, doc_id_set_type>  validDocs;
          for_each(indexTerm.tfs(),
                   [&](const term_index_type& docIndex,
                       const term_freq_type& tf)
                   {
                     auto iter = validDocs.find(tf);
                     if (iter == validDocs.end()) {
                       validDocs.emplace(tf, doc_id_set_type(1, docIndex));
                     } else {
                       iter->second.push_back(docIndex);
                     }
                   });
          
          
          for_each(termInfo.tfs(),
                   [&](const term_index_type& qryIndex,
                       const term_freq_type& tf)
                   {
                     if (flags.isSet(qryIndex)) { return; }
                     
                     auto iter = validDocs.find(tf);
                     if (iter == validDocs.end()) {
                       flags.set(qryIndex);
                       return;
                     }
                     
                     // iter->second <-> list of all documents that have
                     // term termText with the same tf as the query
                     
                     // we need to make sure the documents have the same
                     // number of terms as the query
                     auto qryLength = field.second.documentLengths()[qryIndex];
                     doc_id_set_type  clean_doc_set;
                     std::remove_copy_if(std::begin(iter->second),
                                         std::end(iter->second),
                                         std::back_inserter(clean_doc_set),
                                         [&](const term_index_type& docIndex) {
                                           return qryLength !=
                                           indexField.documentLengths()[docIndex];
                                         });
                     
                     if (clean_doc_set.empty()) {
                       // oops all matching documents have different number of
                       // terms. We done with the query.
                       doc_lists[qryIndex] = clean_doc_set;
                       flags.set(qryIndex);
                       return;
                     }
                     
                     if (doc_lists[qryIndex].empty()) {
                       doc_lists[qryIndex] = clean_doc_set;
                       return;
                     }

                     doc_id_set_type result;
                     std::set_intersection(std::begin(doc_lists[qryIndex]),
                                           std::end(doc_lists[qryIndex]),
                                           std::begin(clean_doc_set),
                                           std::end(clean_doc_set),
                                           std::back_inserter(result));
                     
                     if (result.empty()) {
                       flags.set(qryIndex);
                     }
                     
                     doc_lists[qryIndex] = result;
                   });
          
        }
        
        
        
      } catch (const std::exception& e) {
        // the field is missing from the target index
        // for all terms in this field, the documents containing them cannot
        // be matched
        for(const auto& term : field.second.terms()) {
          flags.setFrom(term.second);
          if (flags.isAllSet()) {
            return doc_lists;
          }
        }
      }
    }
    
    return doc_lists;
  }
  
}}

#endif // __jerome_ir_lookup_hpp__
