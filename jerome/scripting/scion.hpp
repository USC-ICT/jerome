//
//  scion.hpp
//
//  Created by Anton Leuski on 1/31/13.
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

#ifndef __jerome_scripting_scion_hpp__
#define __jerome_scripting_scion_hpp__

#include <iostream>
#include <jerome/scripting/javascript.hpp>
#include <libxml/parser.h>

namespace jerome {
	namespace scripting {
		namespace js = jerome::javascript;

		void setupScionPlatform(js::Value& scionPlatform);
		
		struct ScionPlatform {
			typedef xmlNodePtr	XMLNode;
			
			typedef std::function<void(const char* error, xmlDocPtr doc)>			xml_doc_callback;
			typedef std::function<void(const char* error, const js::String& doc)>	string_rsrc_callback;
			
			static bool stringWithContentsOfURL(const js::String& inURL, js::String& outContent, js::String& outError);

			static void getDocumentFromUrl(const js::String& url, const xml_doc_callback& cb);
			static js::Value parseDocumentFromString(const js::String& content);
			static void getDocumentFromFilesystem(const js::String& path, const xml_doc_callback& cb);
			static void getResourceFromUrl(const js::String& url, const string_rsrc_callback& cb);
			static void postDataToUrl(const js::String& url, js::Value data, js::Value cb);
			static unsigned setTimeout(js::Value cb, double timeout);
			static void clearTimeout(unsigned timeoutID);
			static void log();
			static js::Value eval(const js::String& content, const js::String& name);
			
			
			struct Url {
				static js::String getPathFromUrl(const js::String& url);
				static js::String changeUrlPath(const js::String& url, const js::String& newPath);
			};
			
			struct Dom {
				static std::vector<XMLNode> getChildren(XMLNode node);
				static js::String localName(XMLNode node);
				static const char* getAttribute(XMLNode node, const js::String& attribute);
				static bool hasAttribute(XMLNode node, const js::String& attribute);
				static const char* namespaceURI(XMLNode node);
				static XMLNode createElementNS(xmlDocPtr doc, const char* ns, const js::String& localName);
				static void setAttribute(XMLNode node, const js::String& name, const js::String& value);
				static XMLNode appendChild(XMLNode parent, XMLNode child);
				static js::String textContent(XMLNode node, const js::String& txt);
				static std::vector<XMLNode> getElementChildren(XMLNode node);
			};
			
			struct Path {
				static js::String join(js::String path1, const js::String& path2);
				static js::String dirname(const js::String& path);
				static js::String basename(js::String path, js::String ext);
				static js::String extname(js::String path);
			};
			
			Url	url;
			static Dom	dom;
			Path path;

		};
		
	}
	
	namespace javascript {
		using namespace jerome::scripting;
		template <>
		struct ClassTraits<ScionPlatform> : DefaultClassTraits<ScionPlatform> {
			ClassTraits();
		};
		template <>
		struct ClassTraits<ScionPlatform::Url> : DefaultClassTraits<ScionPlatform::Url> {
			ClassTraits();
		};
		template <>
		struct ClassTraits<ScionPlatform::Dom> : DefaultClassTraits<ScionPlatform::Dom> {
			ClassTraits();
		};
		template <>
		struct ClassTraits<ScionPlatform::Path> : DefaultClassTraits<ScionPlatform::Path> {
			ClassTraits();
		};
	}

}


#endif // defined __jerome_scripting_scion_hpp__
