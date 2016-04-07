//
//  writer_manip.hpp
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

#ifndef __jerome_xml_writer_manip_hpp__
#define __jerome_xml_writer_manip_hpp__

#include <iosfwd>
#include <jerome/types.hpp>

namespace jerome {
  namespace xml {

    namespace detail {
      class item
      {
      public:
        virtual ~item()
        {
        }

      private:
        virtual std::ostream& writeSelf(std::ostream&) const = 0;
        friend std::ostream& operator << (std::ostream&, const item&);
      };

      inline std::ostream& operator << (std::ostream& os, const item& it)
      {
        return it.writeSelf(os);
      }

      class nsItem
        : public item
      {
      public:
        nsItem(const String& inName)
          : mName(inName)
        {
        }

        nsItem(String&& inName)
          : mName(std::move(inName))
        {
        }

      protected:
        String mName;
        OptionalString  mPrefix;
        OptionalString  mURI;
      };

    }

    class startElement
      : public detail::nsItem
    {
      typedef detail::nsItem parent_type;

    public:
      using parent_type::parent_type;

    private:
      std::ostream& writeSelf(std::ostream&) const override;
    };

    class emptyElement
      : public detail::nsItem
    {
      typedef detail::nsItem parent_type;

    public:
      using parent_type::parent_type;

    private:
      std::ostream& writeSelf(std::ostream&) const override;
    };

    class startAttribute
      : public detail::nsItem
    {
      typedef detail::nsItem parent_type;

    public:
      using parent_type::parent_type;

    private:
      std::ostream& writeSelf(std::ostream&) const override;
    };

    std::ostream& endElement(std::ostream&);
    std::ostream& endAttribute(std::ostream&);

    namespace detail {

      template <typename T>
      class attribute
        : public detail::nsItem
      {
        typedef detail::nsItem parent_type;

      public:
        using parent_type::parent_type;

        attribute(const String& inName, const T& inValue)
          : parent_type(inName)
          , mValue(inValue)
        {
        }

        attribute(const String& inName,
          const T& inValue,
          const T& inDefaultValue)
          : parent_type(inName)
        {
          if (inValue != inDefaultValue)
            mValue = inValue;
        }

      private:
        optional<T> mValue;
        std::ostream& writeSelf(std::ostream& os) const override
        {
          if (mValue)
            os << startAttribute(mName) << *mValue << endAttribute;
          return os;
        }

      };

    }

    template <typename T>
    detail::attribute<T> attribute(const String& inName,
                                   const T& inValue,
                                   const T& inDefaultValue)
    {
      return detail::attribute<T>(inName, inValue, inDefaultValue);
    }

    template <typename T>
    detail::attribute<T> attribute(const String& inName, const T& inValue)
    {
      return detail::attribute<T>(inName, inValue);
    }

    inline detail::attribute<String> attribute(const String& inName,
                                               const char* inValue,
                                               const char* inDefaultValue)
    {
      return detail::attribute<String>(inName, inValue, inDefaultValue);
    }

    inline detail::attribute<String> attribute(const String& inName,
                                               const char* inValue)
    {
      return detail::attribute<String>(inName, inValue);
    }

    std::ostream& noindent(std::ostream&);

    class indent
      : public detail::item
    {
      typedef detail::item parent_type;

    public:
      indent(const String& inValue)
        : mValue(inValue)
      {
      }

    private:
      String mValue;
      std::ostream& writeSelf(std::ostream&) const override;
    };

    class quoteChar
      : public detail::item
    {
    public:
      quoteChar(unsigned char inValue)
        : mValue(inValue)
      {
      }

    private:
      unsigned char mValue;
      std::ostream& writeSelf(std::ostream&) const override;
    };

  }
}

#endif // defined __jerome_xml_writer_manip_hpp__
