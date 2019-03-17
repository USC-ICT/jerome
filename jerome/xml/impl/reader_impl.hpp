//
//  reader_impl.hpp
//
//  Created by Anton Leuski on 8/19/15.
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

#ifndef __jerome_xml_impl_reader_impl_hpp__
#define __jerome_xml_impl_reader_impl_hpp__

#include <iosfwd>
#include <jerome/xml/impl/input_buffer.hpp>
#include <jerome/xml/reader.hpp>

namespace jerome {
  namespace xml {
    namespace detail {
      
      static const std::size_t kNodeTypeCount = 18;
      static const reader::NodeType sTypes[kNodeTypeCount] = {
        reader::NodeType::NONE, // 0
        reader::NodeType::ELEMENT, // 1
        reader::NodeType::ATTRIBUTE, // 2
        reader::NodeType::TEXT, //3
        reader::NodeType::CDATA, // 4
        reader::NodeType::ENTITY_REFERENCE, // 5
        reader::NodeType::ENTITY, // 6
        reader::NodeType::PROCESSING_INSTRUCTION, // 7
        reader::NodeType::COMMENT, // 8
        reader::NodeType::DOCUMENT, // 9
        reader::NodeType::DOCUMENT_TYPE, // 10
        reader::NodeType::DOCUMENT_FRAGMENT, // 11
        reader::NodeType::NOTATION, // 12
        reader::NodeType::WHITESPACE, // 13
        reader::NodeType::SIGNIFICANT_WHITESPACE, // 14
        reader::NodeType::END_ELEMENT, // 15
        reader::NodeType::END_ENTITY, // 16
        reader::NodeType::XML_DECLARATION // 17
      };
      
      class reader_impl
      {
      public:
        typedef std::size_t size_type;
        
        reader_impl(std::istream& is)
        : mBuffer(is)
        , mReader(mBuffer.newReader(NULL, "utf-8", 0))
        , mThrow(true)
        , mTreatEmptyAsTwoTags(true)
        , mFakeEndElementTag(false)
        {
        }
        
        ~reader_impl()
        {
          if (!mReader) return;
          xmlTextReaderClose(mReader);
          xmlFreeTextReader(mReader);
          mReader = nullptr;
        }
        
        bool isGood() const
        {
          return mReader != nullptr;
        }
        
        
#define JEROME_XML_READER_THROW_ERROR(result, F) \
throw_error(result, #F)
        
#define JEROME_XML_READER_GET_SIZE(F) \
getSize(F, #F)
        
#define JEROME_XML_READER_GET_BOOL(F) \
getBool(F, #F)

#define JEROME_XML_READER_GET_INT(F) \
getInt(F, #F)

      private:
        template <typename F>
        String getString(F f) const
        {
          return toString(f(mReader));
        }
        
				template <typename F>
				int getInt(F f, const char* N) const
				{
					int result = f(mReader);
					if (result < 0)
						throw_error(result, N);
					return result;
				}

				template <typename F>
        size_type getSize(F f, const char* N) const
        {
          int result = getInt(f, N);
          return (result >= 0 ? result : 0);
        }

        template <typename F>
        bool getBool(F f, const char* N) const
        {
          return getInt(f, N) == 1;
        }
        
				
      public:
				
        reader::NodeType  type() const
        {
          int result = xmlTextReaderNodeType(mReader);
          if (result < 0 || result >= kNodeTypeCount) {
            throw_error(result, "xmlTextReaderNodeType");
            return reader::NodeType::NONE;
          }
          return mFakeEndElementTag
            ? reader::NodeType::END_ELEMENT
            : sTypes[result];
        }
        
        String baseURI() const
        {
          return getString(xmlTextReaderConstBaseUri);
        }
        
        String localName() const
        {
          return getString(xmlTextReaderConstLocalName);
        }
        
        String name() const
        {
          return getString(xmlTextReaderConstName);
        }
        
        String namespaceURI() const
        {
          return getString(xmlTextReaderConstNamespaceUri);
        }
        
        String prefix() const
        {
          return getString(xmlTextReaderConstPrefix);
        }
        
        String value() const
        {
          return getString(xmlTextReaderConstValue);
        }
        
        size_type depth() const
        {
          return JEROME_XML_READER_GET_SIZE(xmlTextReaderDepth);
        }

				int lineNumber() const
				{
					return JEROME_XML_READER_GET_INT(xmlTextReaderGetParserLineNumber);
				}

				int columnNumber() const
				{
					return JEROME_XML_READER_GET_INT(xmlTextReaderGetParserColumnNumber);
				}

        size_type attributeCount() const
        {
          return JEROME_XML_READER_GET_SIZE(xmlTextReaderAttributeCount);
        }
        
        bool hasAttributes() const
        {
          return JEROME_XML_READER_GET_BOOL(xmlTextReaderHasAttributes);
        }
        
        bool hasValue() const
        {
          return JEROME_XML_READER_GET_BOOL(xmlTextReaderHasValue);
        }
        
        bool isEmptyElement() const
        {
          return JEROME_XML_READER_GET_BOOL(xmlTextReaderIsEmptyElement);
        }
        
        bool gotoFirstAttribute()
        {
          return JEROME_XML_READER_GET_BOOL(xmlTextReaderMoveToFirstAttribute);
        }
        
        bool gotoNextAttribute()
        {
          return JEROME_XML_READER_GET_BOOL(xmlTextReaderMoveToNextAttribute);
        }
        
        OptionalString attribute(const String& name)
        {
          return toStringAndFree(xmlTextReaderGetAttribute(mReader,
                                                           BAD_CAST name.c_str()));
        }
        
        OptionalString attribute(size_type index)
        {
          return toStringAndFree(xmlTextReaderGetAttributeNo(mReader,
                                                             (int)index));
        }
        
        bool moveToElement()
        {
          return JEROME_XML_READER_GET_BOOL(xmlTextReaderMoveToElement);
        }
        
        bool gotoNextNode()
        {
          if (mFakeEndElementTag) {
            mFakeEndElementTag = false;
          } else if (type() == xml::reader::ELEMENT
            && isEmptyElement()
            && treatingEmptyElementAsTwoTags()) {
            mFakeEndElementTag = true;
            return true;
          }
          return JEROME_XML_READER_GET_BOOL(xmlTextReaderRead);
        }
        
        bool treatingEmptyElementAsTwoTags() const
        {
          return mTreatEmptyAsTwoTags;
        }
        
        void setTreatingEmptyElementAsTwoTags(bool inValue)
        {
          mTreatEmptyAsTwoTags = inValue;
        }
        
      private:
        static OptionalString toStringAndFree(xmlChar* xml)
        {
          if (!xml)
            return OptionalString();
          
          String result((const char*)xml);
          xmlFree(xml);
          return result;
        }
        
        static String toString(const xmlChar* xml)
        {
          return String(xml ? (const char*)xml : "");
        }
        
        void throw_error(int inError, const String& function) const
        {
          throw Error(function);
        }
        
        input_buffer  mBuffer;
        xmlTextReaderPtr  mReader;
        bool mThrow;
        bool mTreatEmptyAsTwoTags;
        bool mFakeEndElementTag;
      };
      
    }
  }
}

#endif // defined __jerome_xml_impl_reader_impl_hpp__
