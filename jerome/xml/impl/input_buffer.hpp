//
//  input_buffer.hpp
//
//  Created by Anton Leuski on 8/17/15.
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
