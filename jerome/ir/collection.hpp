//
//  collection.hpp
//
//  Created by Anton Leuski on 9/30/11.
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

#ifndef __jerome_ir_collection_hpp__
#define __jerome_ir_collection_hpp__

#include <jerome/types.hpp>
#include <jerome/type/matrix.hpp>
#include <jerome/ir/parsing/token.hpp>
#include <jerome/ir/collection/dictionary.hpp>

namespace jerome { namespace ir { namespace filter {
	template <class Index> class IndexWriter;
}}}

namespace jerome { namespace ir {

	template <class IndexStorage>
	class Index {
	public:
		class Field;

    typedef Alphabet::index_type    TermID;
		typedef std::unordered_map<TermID, Field>		Fields;
    typedef typename Fields::size_type	size_type;
		
	private:
		uint32_t		version;
		Fields			mFields;
		
	public:
		
		virtual ~Index() {}
		
		const Fields&		fields() const { return mFields; }
		Fields&				fields() { return mFields; }
		
    JEROME_EXCEPTION(field_not_found_exception)
    JEROME_EXCEPTION(cannot_insert_field)
	
		class Term {
		public:
			typedef uint8_t								          freq_type;
			// the idea is to separate frequencies from another information, e.g., term
			// locations because it is easier to a. operate on, when frequencies are
			// just a vector, b. customize -- avoid generating locations when they are not
			// needed.
			typedef SparseVector<freq_type>         Frequencies;
			typedef traits<Frequencies>::size_type  size_type;
			
		private:
			size_type   mCollectionCount; // SUM tf
			size_type   mDocumentCount;
			Frequencies mFrequencies;
			
		public:
			
			Term() : mCollectionCount(0), mDocumentCount(0) {}
			
			explicit Term(size_type inDocumentID, const Token& inToken)
			: mCollectionCount(0), mDocumentCount(0), mFrequencies(0)
			{
				add(inDocumentID, inToken);
			}
			
			// number of documents the term appears in
			const size_type df() const { return mDocumentCount; }
			// total number of term occurences in the collection
			const size_type cf() const { return mCollectionCount; }
			// term count for individual documents
			const Frequencies& tfs() const { return mFrequencies; }
			
			static const Term& missing_term() {
				return *_missing_term();
			}
			
			void add(size_type inDocumentID, const Token& inToken) {
				if (inDocumentID >= mFrequencies.size())
          jerome::resize(mFrequencies, inDocumentID+1);
				
				freq_type count = (increment_value_at_index_in_vector(inDocumentID, mFrequencies));
        if (count == 1) ++mDocumentCount;
        
				++mCollectionCount;
			}
			
			void addTerm(const Term& inTerm, std::size_t inOffset, std::size_t inCount) {
				mDocumentCount += inTerm.mDocumentCount;
				mCollectionCount += inTerm.mCollectionCount;

        jerome::resize(mFrequencies, inOffset+inCount);
        append_sparse_vector_to_sparse_vector(inTerm.mFrequencies,
                                              mFrequencies, inOffset);
			}
			
			void optimize(Index& inIndex, Field& inField) {
        jerome::resize(mFrequencies, (size_type)inIndex.documentCount());
			}
      
      bool isMissing() const {
        return this == Term::_missing_term();
      }
    
    private:
      static const Term* _missing_term() {
        // STATIC
        static auto s_missing_term = new Term;
        return s_missing_term;
      }
      
		};
		
		class Field {
			
		public:
      typedef std::unordered_map<TermID, Term>    Terms;
			typedef Vector<uint32_t>	DocumentLengths;
      typedef traits<DocumentLengths>::size_type			size_type;
			
		private:
			Terms			mTerms;
			size_type		mTotalTermCount;
			DocumentLengths	mDocumentLengths;
			
		public:
			
			explicit Field()
			: mTotalTermCount(0)
      , mDocumentLengths((size_type)0)
      {
      }
			
			// all terms
			const Terms&			terms()				const { return mTerms; }
			const size_type			vs()				const { return mTerms.size(); }
			const size_type			cs()				const { return mTotalTermCount; }
			const DocumentLengths&	documentLengths()	const { return mDocumentLengths; }
			size_type				documentCount()		const { return mDocumentLengths.size(); }
//      const Term&        findTerm(const String& text) const {
//        typename Terms::const_iterator i = terms().find(text);
//        return i == terms().end() ? Term::missing_term() : i->second;
//      }
      const Term&        findTerm(TermID inTermID) const {
        auto i = terms().find(inTermID);
        return i == terms().end() ? Term::missing_term() : i->second;
      }

			void	optimize(Index& inIndex) {
				for(typename Terms::value_type& f : mTerms) {
					f.second.optimize(inIndex, *this);
				}
			}
			
			void	addField(const Field& inField, std::size_t inOffset, std::size_t inCount) {
				for(const typename Field::Terms::value_type& t : inField.terms()) {
					typename Terms::iterator p = mTerms.find(t.first);
					if (p == mTerms.end()) {
						p = mTerms.emplace(t.first, Term()).first;
					}
					p->second.addTerm(t.second, inOffset, inCount);
				}
        jerome::resize(mDocumentLengths, inOffset+inCount);
        append_vector_to_vector(inField.mDocumentLengths, mDocumentLengths,
                                inOffset);
				mTotalTermCount += inField.mTotalTermCount;
			}
			
		protected:
			template <class> friend class filter::IndexWriter;
      template <class> friend class Index;

			void	add(typename Term::size_type inDocumentID, const Token& inToken, Index& ioIndex) {
        
//        if (inDocumentID < mDocumentLengths.size() && mDocumentLengths[inDocumentID] >= 75) {
//          return;
//        }

        TermID termID = ioIndex.stringID(inToken.text());
				auto p = mTerms.find(termID);
				if (p == mTerms.end()) {
					mTerms.emplace(termID, Term(inDocumentID, inToken));
				} else {
					p->second.add(inDocumentID, inToken);
				}

        if (inDocumentID >= mDocumentLengths.size()) {
          jerome::resize(mDocumentLengths, inDocumentID+1);
          set_value_at_index_in_vector(0, inDocumentID, mDocumentLengths);
        }
        increment_value_at_index_in_vector(inDocumentID, mDocumentLengths);
        ++mTotalTermCount;
			}
			
			typename Term::size_type	addDocument() {
        auto newDocumentIndex  = mDocumentLengths.size();
        jerome::resize(mDocumentLengths, newDocumentIndex+1);
        set_value_at_index_in_vector(0, newDocumentIndex, mDocumentLengths);
				return (typename Term::size_type)newDocumentIndex;
			}
			
		};
		
    typename Index::Field::size_type	documentCount() const {
      typename Field::size_type	count	= 0;
      for(const typename Fields::value_type& i : fields()) {
        count = std::max(count, i.second.documentCount());
      }
      return count;
    }
    
		void	optimize() {
			for(typename Fields::value_type& f : mFields) {
				f.second.optimize(*this);
			}
		}
		
    const Field&  findField(TermID inFieldNameID) const {
      if (fields().empty())
        throw _field_not_found_exception(inFieldNameID);

      auto  i  = fields().find(inFieldNameID);
      if (i == fields().end()) {
        throw _field_not_found_exception(inFieldNameID);
      }
      return i->second;
    }

    const Field&	findField(const String& inFieldName) const {
			if (fields().empty())
				throw field_not_found_exception(inFieldName);

      auto fieldID = stringID(inFieldName);
      if (!fieldID) {
        throw field_not_found_exception(inFieldName);
      }

      return findField(*fieldID);
		}
		
		Field&	findField(const String& inFieldName, bool inCreateIfNotFound) {
      TermID fieldID = stringID(inFieldName);
			auto i	= fields().find(fieldID);
			if (i != fields().end())
				return i->second;
			
			if (!inCreateIfNotFound)
				throw field_not_found_exception(inFieldName);
			
			auto	p = fields().emplace(fieldID, Field());
			if (!p.second)
				throw cannot_insert_field(inFieldName);
			
			return p.first->second;
		}
		
		void	addIndex(const Index& inIndex)
		{
			std::size_t	docCount = documentCount();
			std::size_t docToAddCount	= inIndex.documentCount();
			for(const typename Fields::value_type& f : inIndex.fields()) {
				Field&	myField(findField(f.first, true));
				myField.addField(f.second, docCount, docToAddCount);
			}
		}

    void addTerm(const Token& inToken, typename Term::size_type inDocumentID, Field& ioField) {
      ioField.add(inDocumentID, inToken, *this);
    }

    TermID stringID(const String& inString) {
      return mAlphabet->emplace(inString);
    }

    optional<TermID> stringID(const String& inString) const {
      auto termID = mAlphabet->string2index(inString);
      return termID == Alphabet::unknownIndex ? optional<TermID>() : optional<TermID>(termID);
    }

    const Term& findTerm(const Field& inField, const String& text) const {
      auto termID = stringID(text);
      if (!termID) return Term::missing_term();
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
	
	class HeapIndex : public Index<HeapIndex> {
    typedef Index<HeapIndex> parent_type;
    using parent_type::parent_type;
	};

	class FileIndex : public Index<FileIndex> {
    typedef Index<FileIndex> parent_type;
    using parent_type::parent_type;
	};

	
	template <class Storage>
	std::ostream&
	operator << (std::ostream& outs, const Index<Storage>& obj) {
		for(const auto& f : obj.fields()) {
			
			outs << f.first << "\t" << f.second.terms().size() << std::endl;
			
			std::map<String, typename Index<Storage>::Term>	tmp(f.second.terms().begin(), f.second.terms().end());
			
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
}}

#endif // defined __jerome_ir_collection_hpp__
