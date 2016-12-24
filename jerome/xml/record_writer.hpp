//
//  record_writer.hpp
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

#ifndef __jerome_xml_record_writer_hpp__
#define __jerome_xml_record_writer_hpp__

#include <boost/mpl/for_each.hpp>

#include <iostream>
#include <ostream>
#include <sstream>
#include <jerome/types.hpp>
#include <jerome/type/Record.hpp>
#include <jerome/xml/writer_manip.hpp>
#include <jerome/xml/reader.hpp>

namespace jerome {

  namespace xml {
    struct RecordTags
    {
      template <typename T>
      struct Trait;

      static constexpr const char* NAME_ATTRIBUTE = "name";
      static constexpr const char* DEFAULT_NAME = "";
    };

    template <>
    struct RecordTags::Trait<double>
    {
      static constexpr const char* TAG = "double";
    };

    template <>
    struct RecordTags::Trait<Bool>
    {
      static constexpr const char* TAG = "bool";
    };
    
    template <>
    struct RecordTags::Trait<String>
    {
      static constexpr const char* TAG = "string";
    };

    template <>
    struct RecordTags::Trait<Record>
    {
      static constexpr const char* TAG = "record";
    };

    namespace detail {

      template <typename Tags>
      class record
        : public detail::item
      {
        typedef detail::item parent_type;

      public:
        typedef Record value_type;
        using parent_type::parent_type;

        record(const value_type& inValue)
          : mValue(inValue)
        {
        }

      private:
        const value_type& mValue;

        class visitor
          : public boost::static_visitor<>
        {
        public:
          visitor(std::ostream& os, const String& inName)
            : mStream(os)
            , mName(inName)
          {
            mStream << std::boolalpha;
          }

          void start(const String& tag) const
          {
            mStream
              << xml::startElement(tag)
              << xml::attribute(Tags::NAME_ATTRIBUTE, mName);
          }

          template <typename T>
          void operator () (const T& value) const
          {
            start(Tags::template Trait<T>::TAG);
            mStream << value;
            mStream << xml::endElement;
          }

          void operator () (const Record& value) const
          {
            start(Tags::template Trait<Record>::TAG);
            for (const auto& p : value) {
              boost::apply_visitor(visitor(mStream, p.first), p.second);
            }
            mStream << xml::endElement;
          }

        private:
          std::ostream& mStream;
          const String& mName;
        };

        std::ostream& writeSelf(std::ostream& os) const override
        {
          for (const auto& p : mValue) {
            boost::apply_visitor(visitor(os, p.first), p.second);
          }
          return os;
        }

      };

			template <typename Tags, typename T>
			struct recordReader;

			template <typename Tags>
			struct recordReader<Tags, String> {
				static String read(xml::reader& ioReader) {
					return ioReader.elementText();
				}
			};

			template <typename Tags>
			struct recordReader<Tags, Bool> {
				static Bool read(xml::reader& ioReader) {
          return Bool(stob(ioReader.elementText()));
				}
			};

      template <typename Tags>
      struct recordReader<Tags, double> {
        static double read(xml::reader& ioReader) {
          return std::stod(ioReader.elementText());
        }
      };
      
			template <typename Tags>
			struct recordReader<Tags, Record> {
				static Record read(xml::reader& ioReader);
			};

      template <typename Tags>
      struct RecordReaderVisitor
      {
        RecordReaderVisitor(const String& inTag,
          const String& inName,
          Record& inRecord,
          xml::reader& ioReader)
          : mTag(inTag)
          , mName(inName)
          , mRecord(inRecord)
					, mReader(ioReader)
					, success(false)
        {
        }

        template <typename U>
        void operator () (U x)
        {
          typedef typename std::remove_reference<typename std::remove_cv<U>::
             type>::type UU;
					
          if (Tags::template Trait<UU>::TAG == mTag) {
            mRecord.emplace(mName, Record::mapped_type(recordReader<Tags, UU>::read(mReader)));
						success = true;
          }
        }

        const String& mTag;
        const String& mName;
        Record& mRecord;
				xml::reader& mReader;
				bool success;
      };
    }

    template <typename Tags>
    inline detail::record<Tags> record(const Record& inValue)
    {
      return detail::record<Tags>(inValue);
    }

    template <typename Tags, typename R = Record>
    inline R readRecord(xml::reader& ioReader)
    {
      R  result;

			ioReader.nextTag();
      while (ioReader.type() == xml::reader::NodeType::ELEMENT) {
        String tag = ioReader.localName();
        String name = ioReader.attribute(Tags::NAME_ATTRIBUTE,
          Tags::DEFAULT_NAME);
				detail::RecordReaderVisitor<Tags> visitor(tag, name, result, ioReader);
				// the ref() is crucially important, otherwise for_each can make a copy
        boost::mpl::for_each<Record::field_type::types>(std::ref(visitor));
				if (!visitor.success)
					throw Error("unexpected tag '" + tag + "' in Record xml.");
				ioReader.nextTag();
      }

      return result;
    }
		
		namespace detail {
		
			template <typename Tags>
			Record recordReader<Tags, Record>::read(xml::reader& ioReader)
			{
				return readRecord<Tags>(ioReader);
			}

		}

  }

}

#endif // defined __jerome_xml_record_writer_hpp__
