//
//  writer.cpp
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
