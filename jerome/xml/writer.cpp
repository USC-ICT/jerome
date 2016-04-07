//
//  writer.cpp
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

#include <jerome/xml/record_writer.hpp>
#include <jerome/xml/impl/writer_impl.hpp>

#include "writer.hpp"
#include "writer_manip.hpp"

namespace jerome {
  namespace xml {

    writer::writer(std::ostream& os)
      : mImpl(new detail::writer_impl_stream(os))
    {
    }

    writer::writer(document& ioDocument)
      : mImpl(new detail::writer_impl_document(ioDocument))
    {
    }

    // need this one even if its epty for the compiler to instantiate the
    // unique_ptr destructor
    writer::~writer()
    {
    }

    std::ostream& writer::stream()
    {
      return mImpl->stream();
    }

    bool writer::isOpen() const
    {
      return mImpl->isOpen();
    }

    void writer::close()
    {
      return mImpl->close();
    }

    std::ostream& startElement::writeSelf(std::ostream& os) const
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) w->startElement(mName, mPrefix, mURI);
      return os;
    }

    std::ostream& emptyElement::writeSelf(std::ostream& os) const
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) {
        w->startElement(mName, mPrefix, mURI);
        w->endElement();
      }
      return os;
    }

    std::ostream& startAttribute::writeSelf(std::ostream& os) const
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) w->startAttribute(mName, mPrefix, mURI);
      return os;
    }
		
    std::ostream& endElement(std::ostream& os)
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) w->endElement();
      return os;
    }

    std::ostream& endAttribute(std::ostream& os)
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) w->endAttribute();
      return os;
    }

    std::ostream& noindent(std::ostream& os)
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) w->setIndent(false);
      return os;
    }

    std::ostream& indent::writeSelf(std::ostream& os) const
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) {
        w->setIndent(true);
        w->setIndentString(mValue);
      }
      return os;
    }

    std::ostream& quoteChar::writeSelf(std::ostream& os) const
    {
      detail::writer_impl* w = detail::writer_impl::fromStream(os);
      if (w) {
        w->setQuoteChar(mValue);
      }
      return os;
    }

  }
}