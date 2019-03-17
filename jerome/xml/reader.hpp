//
//  reader.hpp
//
//  Created by Anton Leuski on 8/19/15.
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

#ifndef __jerome_xml_reader_hpp__
#define __jerome_xml_reader_hpp__

#include <iosfwd>
#include <jerome/types.hpp>
#include <jerome/xml/xml_fwd.hpp>

namespace jerome {
  namespace xml {
    namespace detail {
      class reader_impl;
    }
    class reader
    {
    public:
			typedef std::size_t  size_type;
		
      reader(std::istream&);
      ~reader();

      std::istream& stream();
      bool isOpen() const;
      void close();

      enum NodeType {
        ATTRIBUTE,
        CDATA,
        COMMENT,
        DOCUMENT,
        DOCUMENT_FRAGMENT,
        DOCUMENT_TYPE,
        ELEMENT,
        END_ELEMENT,
        END_ENTITY,
        ENTITY,
        ENTITY_REFERENCE,
        NONE,
        NOTATION,
        PROCESSING_INSTRUCTION,
        SIGNIFICANT_WHITESPACE,
        TEXT,
        WHITESPACE,
        XML_DECLARATION
			};
			
      NodeType nextTag();
      String elementText();

      bool gotoNextNode();

      String localName() const;
      NodeType  type() const;
      size_type depth() const;
			int lineNumber() const;
			int columnNumber() const;
			
      bool hasAttributes() const;
      bool hasValue() const;
      String value() const;
      bool isEmptyElement() const;
			OptionalString attribute(const String& inName) const;
			String attribute(const String& inName, const String& inDefault) const;
			OptionalString attribute(size_type inIndex) const;
			String attribute(size_type inIndex, const String& inDefault) const;

			OptionalError assertNode(reader::NodeType inType, const String& inName);

			bool treatingEmptyElementAsTwoTags() const;
			void setTreatingEmptyElementAsTwoTags(bool inValue);

		private:
      reader(const reader&) = delete;
      reader(reader&&) = delete;
      reader& operator = (const reader&) = delete;
      reader& operator = (reader&&) = delete;

      std::unique_ptr<detail::reader_impl>  mImpl;
			
			OptionalError assertCondition(bool inCondition, String message);
			bool checkLocalName(const String& inName);
			std::ostream& dump(std::ostream& os) const;
			friend std::ostream& operator << (std::ostream& os, const reader& r);
    };

		inline std::ostream& operator << (std::ostream& os, const reader& r)
		{
			return r.dump(os);
		}

  }
}

namespace std {
	std::string to_string(jerome::xml::reader::NodeType type);
}

#endif // defined __jerome_xml_reader_hpp__
