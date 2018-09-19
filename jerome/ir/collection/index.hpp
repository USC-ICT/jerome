//
//  index.hpp
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

#ifndef __jerome_ir_collection_index_hpp
#define __jerome_ir_collection_index_hpp

#include <jerome/ir/collection/dictionary.hpp>
#include <jerome/ir/collection/field.hpp>
#include <jerome/ir/parsing/token.hpp>

namespace jerome { namespace ir { namespace index {

  template <class Derived>
  class Index {
  public:
    typedef typename indexTraits<Derived>::field_type field_type;
    typedef typename indexTraits<Derived>::allocator_type allocator_type;
    typedef typename field_type::term_type term_type;
    typedef typename field_type::TermID TermID;
    typedef std::unordered_map<String, field_type>    Fields;
    typedef typename Fields::size_type  size_type;

  private:
    Fields      mFields;

  public:

    virtual ~Index() {}

    const Fields&    fields() const { return mFields; }
    Fields&        fields() { return mFields; }

    JEROME_EXCEPTION(field_not_found_exception)
    JEROME_EXCEPTION(cannot_insert_field)

    typename field_type::size_type  documentCount() const {
      typename field_type::size_type  count  = 0;
      for(const auto& i : fields()) {
        count = std::max(count, i.second.documentCount());
      }
      return count;
    }

    void  optimize() {
      auto size = documentCount();
      for(auto& f : mFields) {
        f.second.optimize(size);
      }
    }

    const field_type&  findField(const String& inFieldName) const {
      if (fields().empty())
        throw field_not_found_exception(inFieldName);

      auto  i  = fields().find(inFieldName);
      if (i == fields().end()) {
        throw field_not_found_exception(inFieldName);
      }
      return i->second;
    }

    field_type&  findField(const String& inFieldName, bool inCreateIfNotFound) {
      auto i  = fields().find(inFieldName);
      if (i != fields().end())
        return i->second;

      if (!inCreateIfNotFound)
        throw field_not_found_exception(inFieldName);

      auto  p = fields().emplace(inFieldName,
                                 static_cast<Derived*>(this)->makeField(inFieldName));
      if (!p.second)
        throw cannot_insert_field(inFieldName);

      return p.first->second;
    }

    void  addIndex(const Index& inIndex)
    {
      assert(alphabet() == inIndex.alphabet());
      std::size_t  docCount = documentCount();
      std::size_t docToAddCount  = inIndex.documentCount();
      for(const auto& f : inIndex.fields()) {
        field_type&  myField(findField(f.first, true));
        myField.addField(f.second, docCount, docToAddCount);
      }
    }

    void addTerm(const Token& inToken,
                 typename term_type::size_type inDocumentID,
                 field_type& ioField)
    {
      ioField.add(inDocumentID, inToken, *static_cast<Derived*>(this));
    }

    TermID stringID(const String& inString) {
      return mAlphabet->emplace(inString);
    }

    optional<TermID> stringID(const String& inString) const {
      auto termID = mAlphabet->string2index(inString);
      return termID == Alphabet::unknownIndex ? optional<TermID>() : optional<TermID>(termID);
    }

    const term_type& findTerm(const field_type& inField, const String& text) const {
      auto termID = stringID(text);
      if (!termID) return term_type::missing_term();
      return inField.findTerm(termID);
    }

    explicit Index(AlphabetPtr inAlphabet)
    : mAlphabet(inAlphabet)
    {
    }

    AlphabetPtr alphabet() const { return mAlphabet; }

  private:
    Index() = delete;
    AlphabetPtr mAlphabet;

    field_not_found_exception _field_not_found_exception(TermID inID) const {
      auto name = mAlphabet->index2string(inID);
      return field_not_found_exception(name ? *name : std::string("unknown field"));
    }
  };

  template <class Storage>
  std::ostream&
  operator << (std::ostream& outs, const Index<Storage>& obj) {
    for(const auto& f : obj.fields()) {

      outs << f.first << "\t" << f.second.terms().size() << std::endl;

      std::map<String, typename Index<Storage>::Term>  tmp(f.second.terms().begin(), f.second.terms().end());

      for(const auto& t : tmp) {
        outs << f.first << "\t" << t.first << "\t" << t.second.cf() << "\t" << t.second.df() << "\t" << t.second.tfs().size() ;
        // I need the iterator here to print indexes and values
        for(auto i = t.second.tfs().begin(), e = t.second.tfs().end(); i != e; ++i) {
          outs << " (" << i.index() << "," << (uint64_t)*i << ")";
        }
        outs << std::endl;
      }
    }
    return outs;
  }
}}}

#endif // defined __jerome_ir_collection_index_hpp
