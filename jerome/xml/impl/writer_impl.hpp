//
//  writer_impl.hpp
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

#ifndef __jerome_xml_impl_writer_impl_hpp__
#define __jerome_xml_impl_writer_impl_hpp__

#include <libxml/xmlwriter.h>
#include <jerome/xml/record_writer.hpp>
#include <jerome/xml/document.hpp>
#include <jerome/xml/writer_manip.hpp>
#include <jerome/xml/impl/document_impl.hpp>
#include <jerome/xml/impl/output_buffer.hpp>

namespace jerome {
  namespace xml {
    namespace detail {

      class writer_buffer
        : public std::stringbuf
      {
      public:
        writer_buffer()
          : mStream(this)
        {
        }

        std::ostream& stream()
        {
          return mStream;
        }

        String pop()
        {
          mStream.flush();
          String result = str();
          str("");
          return result;
        }

      private:
        std::ostream  mStream;
      };

      class writer_impl
        : public writer_buffer
      {
			public:
        writer_impl()
          : mWriter(nullptr)
          , mStarted(false)
        {
        }

        ~writer_impl()
        {
          close();
        }

        void start()
        {
          stream() << std::boolalpha;
          if (!mWriter) return;
          xmlTextWriterStartDocument(mWriter, "1.0", "UTF-8", "yes");
          mStarted = true;
        }

        void close()
        {
          if (!mWriter) return;
          flushText();
          xmlTextWriterEndDocument(mWriter);
          xmlTextWriterFlush(mWriter);
          xmlFreeTextWriter(mWriter);
          mWriter = nullptr;
        }

        bool isOpen() const
        {
          return mWriter != nullptr;
        }

        static writer_impl * fromStream(std::ostream& os)
        {
          return dynamic_cast<writer_impl*>(os.rdbuf());
        }

        void flushText()
        {
          if (!mStarted)
            start();
          if (str().length()) {
						writeString(pop());
          }
        }

				void writeString(const String& inString)
				{
          if (!mWriter) return;
					xmlTextWriterWriteString(mWriter, BAD_CAST inString.c_str());
				}

        void startElement(const String& inName,
                          const OptionalString& inPrefix,
                          const OptionalString& inURI)
        {
          flushText();
          if (!mWriter) return;
          xmlTextWriterStartElementNS(mWriter,
            BAD_CAST(inPrefix ? (*inPrefix).c_str() : NULL),
            BAD_CAST inName.c_str(),
            BAD_CAST(inURI ? (*inURI).c_str() : NULL));
        }

        void startAttribute(const String& inName,
                            const OptionalString& inPrefix,
                            const OptionalString& inURI)
        {
          flushText();
          if (!mWriter) return;
          xmlTextWriterStartAttributeNS(mWriter,
            BAD_CAST(inPrefix ? (*inPrefix).c_str() : NULL),
            BAD_CAST inName.c_str(),
            BAD_CAST(inURI ? (*inURI).c_str() : NULL));
        }

        void endElement()
        {
          flushText();
          if (!mWriter) return;
          xmlTextWriterEndElement(mWriter);
        }

        void endAttribute()
        {
          flushText();
          if (!mWriter) return;
          xmlTextWriterEndAttribute(mWriter);
        }

        void setIndent(bool inIndent)
        {
          if (!mWriter) return;
          xmlTextWriterSetIndent(mWriter, inIndent ? 1 : 0);
        }

        void setIndentString(const String& inString)
        {
          if (!mWriter) return;
          xmlTextWriterSetIndentString(mWriter, BAD_CAST inString.c_str());
        }

        void setQuoteChar(unsigned char inChar)
        {
          if (!mWriter) return;
          xmlTextWriterSetQuoteChar(mWriter, inChar);
        }
				
      protected:
        xmlTextWriterPtr  mWriter;

      private:
        bool mStarted;

      };

      class writer_impl_stream
        : public writer_impl
      {
      public:
        writer_impl_stream(std::ostream& inStream)
          : writer_impl()
          , mBuffer(inStream)
        {
          xmlOutputBufferPtr  buffer = mBuffer.newOutputBuffer();
          if (buffer) {
            mWriter = xmlNewTextWriter(buffer);
            if (!mWriter) {
              xmlOutputBufferClose(buffer);
            } else {
              // the buffer will be destroyed when we close the writer
            }
          }
        }

      private:
        output_buffer mBuffer;

      };

      class writer_impl_document
        : public writer_impl
      {
      public:
        writer_impl_document(document& ioDocument)
        {
          mWriter = xmlNewTextWriterTree(document_impl::docPtr(ioDocument),
            NULL, 0);
        }

      };

    }
  }
}

#endif // defined __jerome_xml_impl_writer_impl_hpp__
