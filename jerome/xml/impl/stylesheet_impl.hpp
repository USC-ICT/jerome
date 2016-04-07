//
//  stylesheet_impl.hpp
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

#ifndef __jerome_xml_impl_stylesheet_impl_hpp__
#define __jerome_xml_impl_stylesheet_impl_hpp__

#include <fstream>
#include <libxslt/transform.h>
#include <jerome/xml/impl/document_impl.hpp>
#include <jerome/xml/stylesheet.hpp>

namespace jerome {
  namespace xml {
    namespace detail {

      class stylesheet_impl
      {
      public:
        stylesheet_impl(std::istream& is)
				: mStylesheet(nullptr)
        {
          document doc(is);
          if (!(bool)doc) return;
          mStylesheet = xsltParseStylesheetDoc(document_impl::detachDocPtr(doc));
        }

        ~stylesheet_impl()
        {
          if (!mStylesheet) return;
          xsltFreeStylesheet(mStylesheet);
          mStylesheet = nullptr;
        }

        bool isGood() const
        {
          return mStylesheet != nullptr;
        }

        Result<document> apply(const document& document,
                               const StringStringMap& inParams)
        {
          if (!isGood())
            return Error("stylesheet is not defined");

          std::size_t n = inParams.size();
          std::unique_ptr<const char*[]>  params(new const char*[2*n + 1]);
					std::size_t i = 0;
					for(const auto& p : inParams) {
            params[i++] = p.first.c_str();
            params[i++] = p.second.c_str();
          }
          params[n] = nullptr;
					
//					std::ofstream tmp("test.xml");
//					document.save(tmp, true);

          xmlDocPtr resultDocPtr = xsltApplyStylesheet(mStylesheet,
						document_impl::docPtr(document), params.get());

          if (!resultDocPtr)
            return Error("stylesheet failed to apply");

          return document_impl::newDocument(resultDocPtr);
        }

      private:
        xsltStylesheetPtr mStylesheet;
      };

    }
  }
}

#endif // defined __jerome_xml_impl_stylesheet_impl_hpp__
