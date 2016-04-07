//
//  reader.hpp
//
//  Created by Anton Leuski on 8/19/15.
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
