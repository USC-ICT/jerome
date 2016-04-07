//
//  document_impl.hpp
//
//  Created by Anton Leuski on 8/15/15.
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

#ifndef __jerome_xml_impl_document_impl_hpp__
#define __jerome_xml_impl_document_impl_hpp__

#include <libxml/tree.h>
#include <jerome/xml/document.hpp>
#include <jerome/xml/impl/output_buffer.hpp>
#include <jerome/xml/impl/input_buffer.hpp>

namespace jerome {
  namespace xml {
    namespace detail {
      class document_impl
      {
			public:
        document_impl()
          : mDocument(xmlNewDoc(BAD_CAST XML_DEFAULT_VERSION))
        {
        }

        document_impl(xmlDocPtr inNativeDoc)
          : mDocument(inNativeDoc)
        {
        }

        document_impl(std::istream& is)
          : mDocument(nullptr)
        {
					input_buffer	buff(is);
					mDocument = buff.newDocument(NULL, NULL, 0);
        }

        ~document_impl()
        {
          xmlFreeDoc(mDocument);
        }

        static document newDocument(xmlDocPtr inNativeDoc)
        {
          return document(std::unique_ptr<document_impl>(
              new document_impl(inNativeDoc)));
        }
				
        static xmlDocPtr docPtr(const document& inDocument)
        {
          return inDocument.mImpl->mDocument;
        }

        static xmlDocPtr detachDocPtr(const document& inDocument)
        {
					xmlDocPtr	result = inDocument.mImpl->mDocument;
					inDocument.mImpl->mDocument = nullptr;
					return result;
        }

				OptionalError save(std::ostream& os, bool formatted) const
				{
					output_buffer	buff(os);
					int res = xmlSaveFormatFileTo(buff.newOutputBuffer(),
						mDocument, "utf-8", formatted ? 1 : 0);
					return res < 0
						? Error("Failed to save the xml document")
						: Error::NO_ERROR;
				}
				
				bool isGood() const
				{
					return mDocument != nullptr;
				}

			private:
				xmlDocPtr mDocument;
      };

    }
  }
}

#endif // defined __jerome_xml_impl_document_impl_hpp__
