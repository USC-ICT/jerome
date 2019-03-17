//
//  reader.cpp
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

#include <jerome/xml/impl/reader_impl.hpp>
#include "reader.hpp"

namespace jerome {
  namespace xml {

    reader::reader(std::istream& is)
      : mImpl(new detail::reader_impl(is))
    {
    }

    // need this one even if its epty for the compiler to instantiate the
    // unique_ptr destructor
    reader::~reader()
    {
    }

    bool reader::isOpen() const
    {
      return mImpl->isGood();
    }

    String reader::localName() const
    {
      return mImpl->localName();
    }

    reader::NodeType  reader::type() const
    {
      return mImpl->type();
    }

    reader::size_type reader::depth() const
    {
      return mImpl->depth();
    }

		int reader::lineNumber() const
		{
			return mImpl->lineNumber();
		}

		int reader::columnNumber() const
		{
			return mImpl->columnNumber();
		}

    bool reader::hasAttributes() const
    {
      return mImpl->hasAttributes();
    }

    bool reader::hasValue() const
    {
      return mImpl->hasValue();
    }

    String reader::value() const
    {
      return mImpl->value();
    }

    bool reader::isEmptyElement() const
    {
      return mImpl->isEmptyElement();
    }

    OptionalString reader::attribute(const String& inName) const
    {
      return mImpl->attribute(inName);
    }

    OptionalString reader::attribute(size_type inIndex) const
    {
      return mImpl->attribute(inIndex);
    }

		String reader::attribute(const String& inName, const String& inDefault) const
		{
			OptionalString str = mImpl->attribute(inName);
			return str ? *str : inDefault;
		}
	
		String reader::attribute(size_type inIndex, const String& inDefault) const
		{
			OptionalString str = mImpl->attribute(inIndex);
			return str ? *str : inDefault;
		}

    bool reader::gotoNextNode()
    {
      return mImpl->gotoNextNode();
    }

		bool reader::treatingEmptyElementAsTwoTags() const
		{
			return mImpl->treatingEmptyElementAsTwoTags();
		}
		
		void reader::setTreatingEmptyElementAsTwoTags(bool inValue)
		{
			mImpl->setTreatingEmptyElementAsTwoTags(inValue);
		}

		String reader::elementText()
		{
			assertNode(NodeType::ELEMENT, "");
			
			String tag = localName();

			if (isEmptyElement()) {
        if (mImpl->treatingEmptyElementAsTwoTags()) {
          gotoNextNode();
          assertNode(NodeType::END_ELEMENT, tag);
        }
				return String();
      }
			
			String result;
			
			while (true) {
				gotoNextNode();
				NodeType t = type();
				
				if (t == NodeType::TEXT
					|| t == NodeType::WHITESPACE
					|| t == NodeType::SIGNIFICANT_WHITESPACE
					|| t == NodeType::CDATA
					|| t == NodeType::ENTITY_REFERENCE)
				{
					result += value();
					continue;
				} else if (t == NodeType::COMMENT) {
					continue;
				} else if (t == NodeType::END_ELEMENT) {
					break;
				}
				throw Error("unexpected node " + std::to_string(t));
			}
		
			assertNode(NodeType::END_ELEMENT, tag);
			
			return result;
		}

		reader::NodeType reader::nextTag()
		{
			while (true) {
				gotoNextNode();
				NodeType	t = type();
				if (t == NodeType::ELEMENT || t == NodeType::END_ELEMENT)
					return t;

				if (t == NodeType::COMMENT
					|| t == NodeType::WHITESPACE
					|| t == NodeType::SIGNIFICANT_WHITESPACE
					|| t == NodeType::PROCESSING_INSTRUCTION
					) continue;

				if (t == NodeType::TEXT
					|| t == NodeType::CDATA) {
					String text = value();
					if (trim(text).empty())
						continue;
				}
				throw Error("unexpected node " + std::to_string(t));			
			}
		}

    OptionalError reader::assertNode(reader::NodeType inType, const String& inName)
    {
      return assertCondition(type() == inType && checkLocalName(inName),
        "expecting xml node \"" + inName
				+ "\" of type " + std::to_string(inType));
    }

    bool reader::checkLocalName(const String& inName)
    {
      return inName.empty() || localName() == inName;
    }

    OptionalError reader::assertCondition(bool inCondition, String message)
    {
      if (inCondition) return Error::NO_ERROR;
      Error err(message);
      throw err;
      return err;
    }

    std::ostream& reader::dump(std::ostream& os) const
    {
			os << type() << " " << localName() << " " << value();
			return os;
    }

  }
}

namespace std {
	using namespace jerome::xml;
	std::string to_string(reader::NodeType type)
	{
		switch (type) {
			case reader::ATTRIBUTE: return "ATTRIBUTE";
			case reader::CDATA: return "CDATA";
			case reader::COMMENT: return "COMMENT";
			case reader::DOCUMENT: return "DOCUMENT";
			case reader::DOCUMENT_FRAGMENT: return "DOCUMENT_FRAGMENT";
			case reader::DOCUMENT_TYPE: return "DOCUMENT_TYPE";
			case reader::ELEMENT: return "BEGIN_ELEMENT";
			case reader::END_ELEMENT: return "END_ELEMENT";
			case reader::END_ENTITY: return "END_ENTITY";
			case reader::ENTITY: return "BEGIN_ENTITY";
			case reader::ENTITY_REFERENCE: return "ENTITY_REFERENCE";
			case reader::NONE: return "NONE";
			case reader::NOTATION: return "NOTATION";
			case reader::PROCESSING_INSTRUCTION: return "PROCESSING_INSTRUCTION";
			case reader::SIGNIFICANT_WHITESPACE: return "SIGNIFICANT_WHITESPACE";
			case reader::TEXT: return "TEXT";
			case reader::WHITESPACE: return "WHITESPACE";
			case reader::XML_DECLARATION: return "XML_DECLARATION";
		}
		return "UNKNOWN";
	}
}
