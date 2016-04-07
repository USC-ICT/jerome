//
//  output_buffer.hpp
//
//  Created by Anton Leuski on 8/16/15.
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

#ifndef __jerome_xml_impl_output_buffer_hpp__
#define __jerome_xml_impl_output_buffer_hpp__

#include <libxml/xmlIO.h>

namespace jerome {
  namespace	xml {
		namespace detail {
		
			class output_buffer {
			
			public:
        output_buffer(std::ostream& inStream)
          : mStream(inStream)
        {
        }

        xmlOutputBufferPtr newOutputBuffer()
        {
          return xmlOutputBufferCreateIO(
            &OutputWriteCallback,
            &OutputCloseCallback,
            static_cast<void*>(this),
            nullptr);
        }
				
			private:
        std::ostream& mStream;

        std::ostream& targetStream()
        {
          return mStream;
        }

        static int OutputWriteCallback(void * context,
                                       const char * buffer,
                                       int len)
        {
          output_buffer* wrt = static_cast<output_buffer*>(context);
          return wrt->writeCallback(buffer, len);
        }

        static int OutputCloseCallback(void * context)
        {
          output_buffer* wrt = static_cast<output_buffer*>(context);
          return wrt->closeCallback();
        }

        int writeCallback(const char * buffer, int len)
        {
          targetStream().write(buffer, len);
          return targetStream().bad() ? -1 : len;
        }

        int closeCallback()
        {
          targetStream().flush();
          return targetStream().bad() ? -1 : 0;
        }

			};
			
		}
	}
}

#endif // defined __jerome_xml_impl_output_buffer_hpp__
