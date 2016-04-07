//
//  writer_impl.hpp
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
