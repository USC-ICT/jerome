//
//  field.hpp
//
//  Created by Anton Leuski on 9/14/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_collection_field_hpp
#define __jerome_ir_collection_field_hpp

#include <jerome/ir/collection/dictionary.hpp>
#include <jerome/ir/collection/term.hpp>
#include <jerome/ir/collection/index.hpp>
#include <jerome/ir/parsing/token.hpp>

namespace jerome { namespace ir { namespace filter {
  template <class Index> class IndexWriter;
}}}

namespace jerome { namespace ir { namespace index {
  template <class> struct Index;

  template <class Derived>
  struct BasicField {
    typedef typename indexTraits<Derived>::term_type term_type;
    typedef typename indexTraits<Derived>::term_id_type TermID;
    typedef typename indexTraits<Derived>::term_map_type Terms;
    typedef typename indexTraits<Derived>::document_length_vector_type document_length_vector_type;
    typedef typename document_length_vector_type::size_type size_type;
    typedef jerome::DocumentLengths<
      typename document_length_vector_type::value_type
    > DocumentLengths;
    typedef typename indexTraits<Derived>::document_store_type document_store_type;
    typedef typename term_type::size_type document_id_type;

    // all terms
    const Terms& terms() const {
      return static_cast<const Derived*>(this)->terms();
    }
    DocumentLengths documentLengths() const {
      const document_length_vector_type& x(static_cast<const Derived*>(this)
                                           ->_documentLengths());
      return DocumentLengths(x.data(), x.size());
    }

    const size_type vs() const { return terms().size(); }
    const size_type cs() const { return mTotalTermCount; }
    size_type documentCount() const { return documentLengths().size(); }

    const term_type& findTerm(TermID inTermID) const {
      auto i = terms().find(inTermID);
      return i == terms().end() ? term_type::missing_term() : i->second;
    }

  protected:

    explicit BasicField()
    : mTotalTermCount(0)
    {
    }

    Terms& terms() {
      return static_cast<Derived*>(this)->_terms();
    }

    document_length_vector_type& documentLengths() {
      return static_cast<Derived*>(this)->_documentLengths();
    }

    void optimize(size_type inDocumentCount) {
      static_cast<Derived*>(this)->_finalizeTerms(inDocumentCount);
      static_cast<Derived*>(this)->_finalizeDocumentLengths(inDocumentCount);
    }

    void _finalizeTerms(size_type inDocumentCount) {
      for(auto& f : terms()) {
        f.second.finalize(inDocumentCount);
      }
    }

    void _finalizeDocumentLengths(size_type inDocumentCount) {
      documentLengths().resize(inDocumentCount);
      documentLengths().shrink_to_fit();
    }

    typename Terms::iterator __findOrAddTermWithID(TermID inTermID) {
      auto p = terms().find(inTermID);
      if (p == terms().end()) {
        p = terms().emplace(inTermID, term_type()).first;
      }
      return p;
    }

    void _appendTerm(TermID inTermID, const term_type& inTerm,
                     std::size_t inOffset, std::size_t inCount)
    {
      auto p = static_cast<Derived*>(this)->__findOrAddTermWithID(inTermID);
      p->second.addTerm(inTerm, inOffset, inCount);
    }

    void _appendTerm(TermID inTermID,
                     document_id_type inDocumentID)
    {
      auto p = static_cast<Derived*>(this)->__findOrAddTermWithID(inTermID);
      p->second.addOccurenceInDocument(inDocumentID);
    }

    void _appendDocumentLengths(const BasicField& inField,
                                std::size_t inOffset, std::size_t inCount)
    {
      documentLengths().resize(inOffset+inCount, 0);
      for(auto i = 0; i < inField.documentLengths().size(); ++i) {
        documentLengths()[inOffset+i] = inField.documentLengths()[i];
      }
    }

    void _appendDocumentLengths(document_id_type inDocumentID)
    {
      if (inDocumentID >= documentLengths().size()) {
        documentLengths().resize(inDocumentID+1, 0);
      }
      documentLengths()[inDocumentID] += 1;
    }

    document_id_type _addDocument() {
      auto newDocumentIndex  = documentLengths().size();
      documentLengths().resize(newDocumentIndex+1, 0);
      return (document_id_type)newDocumentIndex;
    }

    void _setDocumentContent(document_id_type inID, const String& inText) {
    }

    void  addField(const BasicField& inField,
                   std::size_t inOffset, std::size_t inCount)
    {
      for(const auto& t : inField.terms()) {
        static_cast<Derived*>(this)->_appendTerm(t.first, t.second,
                                                inOffset, inCount);
      }
      static_cast<Derived*>(this)->_appendDocumentLengths(inField,
                                                         inOffset, inCount);
      mTotalTermCount += inField.mTotalTermCount;
    }

    template <class> friend class jerome::ir::filter::IndexWriter;
    template <class> friend struct Index;
    template <class, class> friend class jerome::stream::stream_detail::index_writer_base;
    template <class> friend class jerome::stream::stream_detail::index_writer_filter;
    template <class> friend class jerome::stream::stream_detail::store_writer_filter;

    template <typename T>
    void  add(document_id_type inDocumentID,
              const Token& inToken,
              Index<T>& ioIndex)
    {

      //        if (inDocumentID < mDocumentLengths.size() && mDocumentLengths[inDocumentID] >= 75) {
      //          return;
      //        }

      TermID termID = ioIndex.stringID(inToken.text());
      static_cast<Derived*>(this)->_appendTerm(termID, inDocumentID);
      static_cast<Derived*>(this)->_appendDocumentLengths(inDocumentID);
      mTotalTermCount += 1;
    }

    document_id_type addDocument() {
      return static_cast<Derived*>(this)->_addDocument();
    }

    void setDocumentContent(document_id_type inID, const String& inText) {
      static_cast<Derived*>(this)->_setDocumentContent(inID, inText);
    }

  private:
    size_type    mTotalTermCount;
  };
}}}
#endif // defined __jerome_ir_collection_field_hpp
