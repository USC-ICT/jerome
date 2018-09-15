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

    class Term {
      public:
      typedef uint8_t                         freq_type;
      // the idea is to separate frequencies from another information, e.g., term
      // locations because it is easier to a. operate on, when frequencies are
      // just a vector, b. customize -- avoid generating locations when they are not
      // needed.
      typedef TermFrequencies<freq_type>      Frequencies;
      typedef traits<Frequencies>::size_type  size_type;

      // number of documents the term appears in
      const size_type df() const { return mDocumentCount; }
      // total number of term occurences in the collection
      const size_type cf() const { return mCollectionCount; }
      // term count for individual documents
      Frequencies tfs() const {
        // Note this a column major matrix
        // we will make Nx1 matrix
        return Frequencies(mFrequencies.size(), 1, mFrequencies.entry_count(),
                           mOuterIndices, mFrequencies.indicies(),
                           mFrequencies.data());
      }

      static const Term& missing_term() {
        return *_missing_term();
      }

      bool isMissing() const {
        return this == Term::_missing_term();
      }

      private:
      size_type   mCollectionCount; // SUM tf
      // in theory, we can use the number of nonzero entries in the
      // mFrequencies for this, bu what if some are zeroes?
      size_type   mDocumentCount;
      // aux pieces needed to map the frequencies to the Eigen matrix;
      Frequencies::StorageIndex mOuterIndices[2];
      jerome::sparse_vector<freq_type, Frequencies::StorageIndex> mFrequencies;

      Term()
      : mCollectionCount(0)
      , mDocumentCount(0)
      , mOuterIndices {0,0}
      , mFrequencies()
      {}

      void addOccurenceInDocument(size_type inDocumentID) {
        if (inDocumentID >= mFrequencies.size()) {
          mFrequencies.resize(inDocumentID+1);
        }
        // can throw
        auto count = (mFrequencies.at(inDocumentID) += 1);
        if (count == 1) {
          mDocumentCount += 1;
        }
        mCollectionCount += 1;
        mOuterIndices[1] = mFrequencies.entry_count();
      }

      void addTerm(const Term& inTerm, std::size_t inOffset, size_type inCount) {
        // can throw
        mFrequencies.append(inTerm.mFrequencies, inOffset);
        mCollectionCount += inTerm.mCollectionCount;
        mDocumentCount += inTerm.mDocumentCount;
        mOuterIndices[1] = mFrequencies.entry_count();
      }

      void finalize(size_type inFinalSize) {
        mFrequencies.resize(inFinalSize);
        mFrequencies.shrink_to_fit();
      }

      private:
      friend class Field;
      static const Term* _missing_term() {
        // STATIC
        static auto s_missing_term = new Term;
        return s_missing_term;
      }
    };

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

		class Field {
			
		public:
      typedef std::unordered_map<TermID, Term>    Terms;
      typedef std::vector<uint32_t>	DocumentLengths;
      typedef DocumentLengths::size_type			size_type;
			
		private:
			Terms			mTerms;
			size_type		mTotalTermCount;
			DocumentLengths	mDocumentLengths;
			
		public:
			
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

      private:

      explicit Field()
      : mTotalTermCount(0)
      , mDocumentLengths((size_type)0)
      {
      }

			void	optimize(size_type inDocumentCount) {
				for(typename Terms::value_type& f : mTerms) {
					f.second.finalize(inDocumentCount);
				}
        mDocumentLengths.resize(inDocumentCount);
        mDocumentLengths.shrink_to_fit();
			}
			
			void	addField(const Field& inField, std::size_t inOffset, std::size_t inCount) {
				for(const typename Field::Terms::value_type& t : inField.terms()) {
					typename Terms::iterator p = mTerms.find(t.first);
					if (p == mTerms.end()) {
						p = mTerms.emplace(t.first, Term()).first;
					}
					p->second.addTerm(t.second, inOffset, inCount);
				}
        _resizeDocumentLengths(inOffset+inCount);
        _appendToDocumentLengths(inField.mDocumentLengths, inOffset);
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
					p = mTerms.emplace(termID, Term()).first;
				}
        p->second.addOccurenceInDocument(inDocumentID);

        if (inDocumentID >= mDocumentLengths.size()) {
          _resizeDocumentLengths(inDocumentID+1);
        }
        _incrementDocumentLength(inDocumentID);
        mTotalTermCount += 1;
			}
			
			typename Term::size_type	addDocument() {
        auto newDocumentIndex  = mDocumentLengths.size();
        _resizeDocumentLengths(newDocumentIndex+1);
				return (typename Term::size_type)newDocumentIndex;
			}

      inline void _appendToDocumentLengths(const DocumentLengths& inOther,
                                           size_type inOffset)
      {
        for(auto i = 0; i < inOther.size(); ++i) {
          mDocumentLengths[inOffset+1] = inOther[i];
        }
      }

      inline void _resizeDocumentLengths(size_type inNewSize) {
        mDocumentLengths.resize(inNewSize, 0);
      }

      inline void _incrementDocumentLength(size_type inDocumentID) {
        mDocumentLengths[inDocumentID] += 1;
      }
		};

    typename Index::Field::size_type	documentCount() const {
      typename Field::size_type	count	= 0;
      for(const auto& i : fields()) {
        count = std::max(count, i.second.documentCount());
      }
      return count;
    }
    
		void	optimize() {
      auto size = documentCount();
			for(auto& f : mFields) {
				f.second.optimize(size);
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
      assert(alphabet() == inIndex.alphabet());
			std::size_t	docCount = documentCount();
			std::size_t docToAddCount	= inIndex.documentCount();
			for(const auto& f : inIndex.fields()) {
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

    public:
    FileIndex(const String& inPath);
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
