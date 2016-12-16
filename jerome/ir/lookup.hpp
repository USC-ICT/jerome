//
//  lookup.hpp
//  jerome
//
//  Created by Anton Leuski on 12/15/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
          // find the query term in the target index
          const auto indexTerm = indexField.findTerm(term.first);
          if (indexTerm.isMissing()) {
            // all the queries with this term cannot be matched.
            flags.setFrom(term.second);
            if (flags.isAllSet()) {
              return doc_lists;
            }
            continue;
          }
          
          // index all documents in the target index by TF of the term
          Map<term_freq_type, doc_id_set_type>  validDocs;
          for_each(indexTerm.tfs(),
                   [&](const term_index_type& i,
                       const term_freq_type& v)
                   {
                     auto iter = validDocs.find(v);
                     if (iter == validDocs.end()) {
                       validDocs.emplace(v, doc_id_set_type(1, i));
                     } else {
                       iter->second.push_back(i);
                     }
                   });
          
          
          for_each(term.second.tfs(),
                   [&](const term_index_type& i,
                       const term_freq_type& v)
                   {
                     if (flags.isSet(i)) { return; }
                     
                     auto iter = validDocs.find(v);
                     if (iter == validDocs.end()) {
                       flags.set(i);
                       return;
                     }
                     
                     if (doc_lists[i].empty()) {
                       doc_lists[i] = iter->second;
                       return;
                     }
                     
                     doc_id_set_type result;
                     std::set_intersection(std::begin(doc_lists[i]),
                                           std::end(doc_lists[i]),
                                           std::begin(iter->second),
                                           std::end(iter->second),
                                           std::back_inserter(result));
                     
                     if (result.empty()) {
                       flags.set(i);
                     }
                     
                     doc_lists[i] = result;
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
