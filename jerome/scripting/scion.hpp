//
//  scion.hpp
//
//  Created by Anton Leuski on 1/31/13.
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
				static js::String getAttribute(XMLNode node, const js::String& attribute);
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
