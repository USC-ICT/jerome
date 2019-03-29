//
//  document_impl.hpp
//
//  Created by Anton Leuski on 8/15/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
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
