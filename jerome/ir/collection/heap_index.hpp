//
//  heap_index.hpp
//
//  Created by Anton Leuski on 9/17/18.
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

#ifndef __jerome_ir_collection_heap_index_hpp
#define __jerome_ir_collection_heap_index_hpp

#include <jerome/ir/collection/term.hpp>
#include <jerome/ir/collection/field.hpp>
#include <jerome/ir/collection/index.hpp>

namespace jerome { namespace ir {
  struct HeapIndex;
}}

namespace jerome { namespace ir { namespace index {
  struct HeapField;

  template <>
  struct indexTraits<HeapIndex> {
    typedef HeapField field_type;
    typedef std::allocator<void> allocator_type;
    template <typename V, typename A>
    using vector_type = std::vector<V, A>;
  };

  template <>
  struct indexTraits<HeapField> {
    typedef Alphabet::index_type    term_id_type;
    typedef uint32_t                document_length_type;
    typedef BasicTerm<
      std::vector,
      std::allocator<void>
    > term_type;
    typedef std::unordered_map<term_id_type, term_type
    > term_map_type;
    typedef std::vector<document_length_type>  document_length_vector_type;
    typedef std::unordered_map<term_id_type, std::string> document_store_type;
  };

  struct HeapField : public BasicField<HeapField> {
    typedef BasicField<HeapField> parent_type;
    typedef typename parent_type::term_type term_type;
    typedef typename parent_type::TermID TermID;
    typedef typename parent_type::Terms Terms;
    typedef typename parent_type::document_length_vector_type
      document_length_vector_type;
    typedef typename parent_type::size_type size_type;
    typedef typename parent_type::document_store_type document_store_type;
    typedef typename parent_type::document_id_type document_id_type;

    const Terms& terms() const { return mTerms; }

    optional<std::string> documentWithID(document_id_type inID) const {
      auto iter = mDocuments.find(inID);
      if (iter != mDocuments.end()) {
        return iter->second;
      }
      return optional<std::string>();
    }

  private:
    friend parent_type;
    friend struct jerome::ir::HeapIndex;

    Terms mTerms;
    document_length_vector_type mDocumentLengths;
    document_store_type mDocuments;

    HeapField()
    : parent_type()
    , mTerms()
    , mDocumentLengths()
    , mDocuments()
    {}

    const document_length_vector_type&  _documentLengths()  const {
      return mDocumentLengths;
    }
    Terms& _terms() { return mTerms; }

    document_length_vector_type&  _documentLengths() {
      return mDocumentLengths;
    }

    typename Terms::iterator __findOrAddTermWithID(TermID inTermID) {
      auto p = _terms().find(inTermID);
      if (p == _terms().end()) {
        p = _terms().emplace(inTermID, term_type()).first;
      }
      return p;
    }

    void _setDocumentContent(document_id_type inID, const String& inText) {
      mDocuments.emplace(inID, inText);
    }
  };

}}}

namespace jerome { namespace ir {
  using namespace index;
  struct HeapIndex : public Index<HeapIndex> {
    typedef Index<HeapIndex> parent_type;
    using parent_type::parent_type;

    typedef typename parent_type::field_type Field;
    typedef typename parent_type::allocator_type allocator_type;
    typedef typename parent_type::term_type Term;
    typedef typename parent_type::TermID TermID;
    typedef typename parent_type::Fields Fields;
    typedef typename parent_type::size_type size_type;

  private:
    friend parent_type;
    Field makeField(const String& inName)
    {
      return Field();
    }

  };
}}

#endif // defined __jerome_ir_collection_heap_index_hpp
