//
//  index_writer.hpp
//
//  Created by Anton Leuski on 9/26/18.
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

#ifndef __jerome_ir_parsing_filter_index_writer_hpp
#define __jerome_ir_parsing_filter_index_writer_hpp

namespace jerome { namespace stream {
  namespace stream_detail{

    template <typename Derived, typename Index>
    class index_writer_base : public stream_filter {
    protected:
      typedef typename Index::field_type field_type;
      typedef typename Index::term_type::size_type document_id_type;

      Index& mIndex;
      field_type& mField;
      document_id_type mDocumentID;
      bool mHasDocumentID;

    public:
      index_writer_base(Index& inIndex, const String& inFieldName)
      : mIndex(inIndex)
      , mField(mIndex.findField(inFieldName, true))
      , mHasDocumentID(false)
      {}

      template <class Stream, ASSERT_STREAM(Stream)>
      auto operator() (Stream& inStream)
      {
        auto token = inStream.next();
        if (token.isBOS()) {
          if (token.documentID()) {
            this->mDocumentID = *token.documentID();
          } else {
            this->mDocumentID = this->mField.addDocument();
            token.setDocumentID(this->mDocumentID);
          }
          this->mHasDocumentID = true;
        } else if (token.isEOS()) {
          this->mHasDocumentID = false;
        } else if (this->mHasDocumentID) {
          static_cast<Derived*>(this)->processToken(token,
                                                    this->mDocumentID,
                                                    mField, mIndex);
        }
        return token;
      }
    };

    template <typename Index>
    class index_writer_filter : public index_writer_base<
      index_writer_filter<Index>, Index>
    {
      typedef index_writer_base<index_writer_filter<Index>, Index> parent_type;
      typedef typename parent_type::field_type field_type;
      typedef typename parent_type::document_id_type document_id_type;

      using parent_type::parent_type;

      template <class Token>
      void processToken(const Token& inToken,
                        document_id_type inDocumentID,
                        field_type& inField,
                        Index& inIndex)
      {
        inIndex.addTerm(inToken, inDocumentID, inField);
      }

      friend parent_type;
    };

    template <typename Index>
    class store_writer_filter : public index_writer_base<
    store_writer_filter<Index>, Index>
    {
      typedef index_writer_base<store_writer_filter<Index>, Index> parent_type;
      typedef typename parent_type::field_type field_type;
      typedef typename parent_type::document_id_type document_id_type;

      using parent_type::parent_type;

      template <class Token>
      void processToken(const Token& inToken,
                        document_id_type inDocumentID,
                        field_type& inField,
                        Index& inIndex)
      {
        inField.setDocumentContent(inDocumentID, inToken.text());
      }

      friend parent_type;
    };

    struct index_writer_holder {
      template <typename Index>
      inline auto
      operator () (Index& inIndex, const String& inFieldName) const {
        return index_writer_filter<Index>(inIndex, inFieldName);
      }
    };

    struct store_writer_holder {
      template <typename Index>
      inline auto
      operator () (Index& inIndex, const String& inFieldName) const {
        return store_writer_filter<Index>(inIndex, inFieldName);
      }
    };
  }

  const auto write_to_index = stream_detail::index_writer_holder();
  const auto store_to_index = stream_detail::store_writer_holder();
}}

#endif // defined __jerome_ir_parsing_filter_index_writer_hpp
