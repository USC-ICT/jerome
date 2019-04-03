//
//  file_index.hpp
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

#ifndef __jerome_ir_collection_file_index_hpp
#define __jerome_ir_collection_file_index_hpp

#include <jerome/type/filesystem.hpp>
#include <jerome/type/persistence.hpp>
#include <jerome/ir/collection/field.hpp>
#include <jerome/ir/collection/index.hpp>
#include <jerome/ir/collection/persistence_containers.hpp>

namespace jerome { namespace ir {
  struct FileIndex;
}}

namespace jerome { namespace ir { namespace index {

  struct FileField;

  template <>
  struct indexTraits<FileIndex> {
    typedef FileField field_type;
    typedef jerome::persistence::void_allocator allocator_type;
    template <typename V, typename A>
    using vector_type = boost::interprocess::vector<V, A>;
  };

  // needed so BasicField can discover the member types
  template <>
  struct indexTraits<FileField> {
    typedef Alphabet::index_type    term_id_type;
    typedef uint32_t                document_length_type;
    typedef BasicTerm<
      boost::interprocess::vector,
      jerome::persistence::void_allocator
    > term_type;
    typedef jerome::persistence::map<
      term_id_type, term_type,
      jerome::persistence::void_allocator
    >::storage_type term_map_type;
    typedef jerome::persistence::vector<
      document_length_type,
      jerome::persistence::void_allocator
    >::storage_type document_length_vector_type;
    typedef jerome::persistence::map<
      term_id_type, persistence::string,
      jerome::persistence::void_allocator
    >::storage_type document_store_type;
  };

  struct FileField : public BasicField<FileField> {
    typedef BasicField<FileField> parent_type;
    typedef typename parent_type::term_type term_type;
    typedef typename parent_type::TermID TermID;
    typedef typename parent_type::Terms Terms;
    typedef typename parent_type::document_length_vector_type
      document_length_vector_type;
    typedef typename parent_type::size_type size_type;
    typedef typename parent_type::document_store_type document_store_type;

    const Terms& terms() const { return *mTerms.get(); }

    optional<std::string> documentWithID(document_id_type inID) const {
      if (!mDocuments) {
        return optional<std::string>();
      }
      auto iter = _documents().find(inID);
      if (iter != _documents().end()) {
        return std::to_string(iter->second);
      }
      return optional<std::string>();
    }

  private:
    friend parent_type;
    friend struct jerome::ir::FileIndex;
    template <class> friend class jerome::ir::index::Index;

    persistence::MappedPointer<Terms>  mTerms;
    persistence::MappedPointer<document_length_vector_type>  mDocumentLengths;
    persistence::MappedPointer<document_store_type>  mDocuments;

    FileField(persistence::Access inAccess,
              const fs::path& inPath);

    Terms& _terms() { return *mTerms.get(); }
    const document_length_vector_type&  _documentLengths() const {
      return *mDocumentLengths.get();
    }
    document_length_vector_type&  _documentLengths() {
      return *mDocumentLengths.get();
    }
    document_store_type&  _documents() {
      return *mDocuments.get();
    }
    const document_store_type&  _documents() const {
      return *mDocuments.get();
    }

    void  optimize(size_type inDocumentCount) {
      mTerms.perform([&] () mutable {
        this->_finalizeTerms(inDocumentCount);
      });
      mTerms.shrinkToFit();
      mDocumentLengths.perform([&] () mutable {
        this->_finalizeDocumentLengths(inDocumentCount);
      });
      mDocumentLengths.shrinkToFit();
    }

    void _appendTerm(TermID inTermID, const term_type& inTerm,
                     std::size_t inOffset, std::size_t inCount)
    {
      mTerms.perform([&] () mutable {
        parent_type::_appendTerm(inTermID, inTerm, inOffset, inCount);
      });
    }

    void _appendDocumentLengths(const BasicField& inField,
                               std::size_t inOffset, std::size_t inCount)
    {
      mDocumentLengths.perform([&] () mutable {
        parent_type::_appendDocumentLengths(inField, inOffset, inCount);
      });
    }

    void _appendTerm(TermID inTermID, typename term_type::size_type inDocumentID)
    {
      mTerms.perform([&] () mutable {
        parent_type::_appendTerm(inTermID, inDocumentID);
      });
    }

    void _appendDocumentLengths(typename term_type::size_type inDocumentID)
    {
      mDocumentLengths.perform([&] () mutable {
        parent_type::_appendDocumentLengths(inDocumentID);
      });
    }

    typename term_type::size_type  _addDocument()
    {
      return mDocumentLengths.perform([&] () mutable {
        return parent_type::_addDocument();
      });
    }

    // ok to throw
    typename Terms::iterator __findOrAddTermWithID(TermID inTermID) {
      auto p = _terms().find(inTermID);
      if (p == _terms().end()) {
        p = _terms().emplace(inTermID, term_type(mTerms.allocator())).first;
      }
      return p;
    }

    void _setDocumentContent(document_id_type inID, const String& inText) {
      mDocuments.perform([&] () mutable {
        _documents().emplace(inID, jerome::persistence::to_string(inText, _documents().get_allocator()));
      });
    }
  };

}}}

namespace jerome { namespace ir {

  JEROME_EXCEPTION(unsupported_index_version)

  using namespace index;
  class FileIndex : public Index<FileIndex> {
    typedef Index<FileIndex> parent_type;
    using parent_type::parent_type;

  public:
    typedef typename parent_type::field_type Field;
    typedef typename parent_type::allocator_type allocator_type;
    typedef typename parent_type::term_type Term;
    typedef typename parent_type::TermID TermID;
    typedef typename parent_type::Fields Fields;
    typedef typename parent_type::size_type size_type;

    FileIndex(persistence::Access inAccess,
              const fs::path& inPath,
              AlphabetPtr inAlphabet = nullptr);

    const fs::path& path() const { return mPath; }
    persistence::Access access() const { return mAccess; }

  private:
    friend parent_type;
    fs::path mPath;
    persistence::Access mAccess;

    auto makeField(const String& inName) -> Field;
    auto read() -> OptionalError;
    auto write();
    auto infoPath() const -> fs::path;
  };
}}


#endif // defined __jerome_ir_collection_file_index_hpp
