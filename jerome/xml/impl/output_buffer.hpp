//
//  output_buffer.hpp
//
//  Created by Anton Leuski on 8/16/15.
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
