//
//  stylesheet_impl.hpp
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

#ifndef __jerome_xml_impl_stylesheet_impl_hpp__
#define __jerome_xml_impl_stylesheet_impl_hpp__

#include <fstream>
#include <jerome/xml/impl/document_impl.hpp>
#include <jerome/xml/stylesheet.hpp>

#ifndef __IPHONE_OS_VERSION_MIN_REQUIRED
#include <libxslt/transform.h>
#else

// Apple does not have libxslt on iOS. We either need to compile it statically or
// disable this feature...
typedef void* xsltStylesheetPtr;
static xsltStylesheetPtr xsltParseStylesheetDoc	(xmlDocPtr doc) { return NULL; }
static void xsltFreeStylesheet(xsltStylesheetPtr style) {}
static xmlDocPtr xsltApplyStylesheet	(xsltStylesheetPtr style,
                     xmlDocPtr doc, const char **params) { return NULL; }
#endif

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
