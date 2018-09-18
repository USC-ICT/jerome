//
//  term.hpp
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

#ifndef __jerome_ir_collection_term_hpp
#define __jerome_ir_collection_term_hpp

#include <jerome/type/matrix.hpp>
#include <jerome/type/sparse_vector.hpp>

namespace jerome {
  namespace ir {
    namespace index {

      template <
        template<class, class> class VC = std::vector,
        class Allocator = std::allocator<void>
      >
      struct Term {
        typedef uint8_t                         freq_type;
        // the idea is to separate frequencies from another information, e.g., term
        // locations because it is easier to a. operate on, when frequencies are
        // just a vector, b. customize -- avoid generating locations when they are not
        // needed.
        typedef TermFrequencies<freq_type>      Frequencies;
        typedef traits<Frequencies>::size_type  size_type;

        typedef jerome::sparse_vector<
          freq_type, Frequencies::StorageIndex,
          VC, Allocator> frequency_storage_type;

        typedef Allocator allocator_type;

        // number of documents the term appears in
        const size_type df() const { return mDocumentCount; }
        // total number of term occurences in the collection
        const size_type cf() const { return mCollectionCount; }
        // term count for individual documents

      private:
        const frequency_storage_type& frequencies_store() const {
          return mFrequencies;
        }

        frequency_storage_type& frequencies_store() {
          return mFrequencies;
        }

      public:
        Frequencies tfs() const {
          // Note this a column major matrix
          // we will make Nx1 matrix
          return Frequencies(frequencies_store().size(), 1,
                             frequencies_store().entry_count(),
                             mOuterIndices,
                             frequencies_store().indicies(),
                             frequencies_store().data());
        }

        static const Term& missing_term() {
          return *_missing_term();
        }

        bool isMissing() const {
          return this == Term::_missing_term();
        }

        Term(const allocator_type& inAllocator)
        : mCollectionCount(0)
        , mDocumentCount(0)
        , mOuterIndices {0,0}
        , mFrequencies(inAllocator)
        {}

        Term()
        : mCollectionCount(0)
        , mDocumentCount(0)
        , mOuterIndices {0,0}
        , mFrequencies()
        {}

      private:
        size_type   mCollectionCount; // SUM tf
        // in theory, we can use the number of nonzero entries in the
        // mFrequencies for this, bu what if some are zeroes?
        size_type   mDocumentCount;
        // aux pieces needed to map the frequencies to the Eigen matrix;
        Frequencies::StorageIndex mOuterIndices[2];
        frequency_storage_type mFrequencies;

      protected:
       void addOccurenceInDocument(size_type inDocumentID) // can throw
        {
          if (inDocumentID >= frequencies_store().size()) {
            frequencies_store().resize(inDocumentID+1);
          }
          // this exceution order is important, we preallocate space first,
          // so if an allocation exception is thrown, we have the data in a
          // consistent state.
          auto count = (frequencies_store().at(inDocumentID) += 1); // can throw
          if (count == 1) {
            mDocumentCount += 1;
          }
          mCollectionCount += 1;
          mOuterIndices[1] = frequencies_store().entry_count();
        }

        void addTerm(const Term& inTerm, std::size_t inOffset,
                     size_type inCount) // can throw
        {
          frequencies_store().append(inTerm.frequencies_store(), inOffset); // can throw
          mCollectionCount += inTerm.mCollectionCount;
          mDocumentCount += inTerm.mDocumentCount;
          mOuterIndices[1] = frequencies_store().entry_count();
        }

        void finalize(size_type inFinalSize) {
          frequencies_store().resize(inFinalSize);
          frequencies_store().shrink_to_fit();
        }

        private:

        template <class> friend struct BasicField;
        static const Term* _missing_term() {
          // STATIC
          static auto s_missing_term = new Term;
          return s_missing_term;
        }
      };

    }
  }
}

#endif // defined __jerome_ir_collection_term_hpp
