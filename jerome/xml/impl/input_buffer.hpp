//
//  input_buffer.hpp
//
//  Created by Anton Leuski on 8/17/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#ifndef __jerome_xml_impl_input_buffer_hpp__
#define __jerome_xml_impl_input_buffer_hpp__

#include <iostream>
#include <libxml/xmlreader.h>
#include <jerome/types.hpp>

namespace jerome {
  namespace xml {
    namespace detail {

      class input_buffer
      {
      public:
        input_buffer(std::istream& inStream)
          : mStream(inStream)
        {
        }

        xmlDocPtr newDocument(const char* inURL,
                              const char* inEncoding,
                              int inOptions)
        {
          std::unique_ptr<xmlParserCtxt,
            parserCtxtDeleter> ctx(xmlNewParserCtxt());

          xmlDocPtr doc = xmlCtxtReadIO(ctx.get(), &InputReadCallback,
            &InputCloseCallback, this, inURL, inEncoding, inOptions);

          return doc;
        }

        xmlTextReaderPtr newReader(const char* inURL,
                              const char* inEncoding,
                              int inOptions)
        {
          return xmlReaderForIO(&InputReadCallback,
            &InputCloseCallback, this, inURL, inEncoding, inOptions);
        }

      private:
        class parserCtxtDeleter
        {
				public:
          void operator () (xmlParserCtxt* p) const
          {
						if (p)
							xmlFreeParserCtxt(p);
          }
        };

        int readCallback(char * buffer, int len)
        {
					mStream.read(buffer, len);
					return mStream.bad() ? -1 : (int)mStream.gcount();
        }

        int closeCallback()
        {
					return 0;
        }

        static int  InputReadCallback(void * context, char * buffer, int len)
        {
          input_buffer* rdr = static_cast<input_buffer*>(context);
          return rdr->readCallback(buffer, len);
        }

        static int InputCloseCallback(void * context)
        {
          input_buffer* rdr = static_cast<input_buffer*>(context);
          return rdr->closeCallback();
        }

        std::istream& mStream;
      };

    }
  }
}

#endif // defined __jerome_xml_impl_input_buffer_hpp__
