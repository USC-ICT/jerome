//
//  scion.cpp
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

#include "scion.hpp"
#include <libxml/tree.h>
#include <jerome/type/regex.hpp>

#include <jerome/scripting/async_js.hpp>
#include <jerome/async.hpp>
#include <atomic>
#ifdef JEROME_IOS
#include <CoreFoundation/CoreFoundation.h>
#endif

namespace jerome {
	namespace scripting {
		namespace js = jerome::javascript;
		namespace detail {
			
			struct Timeout {
			
				// it is assumed that this function is called only from JS,
				// so we do not need to synchronize the call on the JS thread
				// but we do need to synchronize it on sTimers data structure
				static int	createTimer(js::Value inCallback, double inMilliSeconds)
				{
					// we want to keep the context around for the callback to execute
					// as soon as the current function exists the context will no longer be valid
					// (that's something the function docs are not very clear about)
					// we cannot copy the context, but we can create one in the same context group
					// and move the callback to the new context.

					js::ContextGroup	cg	= inCallback.context().group();
					js::Context			c	= cg.newContext();
					inCallback				= c.newValue(inCallback);
					
					int	timerID	= sTimerCounter++;
					timerStrand().call([=] {
						Timeout::createTimerWithID(timerID, inCallback, inMilliSeconds);
					});
					return timerID;
				}
				
				// it is assumed that this function is called only from JS,
				// so we do not need to synchronize the call on the JS thread
				// but we do need to synchronize it on sTimers data structure
				static void cancel(int inTimerID) {
					timerStrand().call([=] {
						Timeout::cancelTimerWithID(inTimerID);
					});
				}
				
				typedef std::function<void(void)> callback_type;
			private:
			
				static dispatch::queue& timerStrand()
				{
          // STATIC
					static auto queue = new dispatch::queue(dispatch::queue::SERIAL);
					return *queue;
				}
			
				// synchronized on sTimers
				static void removeTimerWithID(int inTimerID) {
					auto	it = sTimers.find(inTimerID);
					if (it != sTimers.end()) {
//						it->second->cancel();
						it->second.stop();
						sTimers.erase(it);
					}
				}

				// synchronized on sTimers
				static void createTimerWithID(int inTimerID, js::Value inCallback, double inMilliSeconds) {
				
					callback_type	callback(inCallback);
					dispatch::timer timer(strandFromContext(inCallback.context()),
						inMilliSeconds*1000, [=](const dispatch::timer&)
						{
							timerStrand().call([=] {
								Timeout::removeTimerWithID(inTimerID);
							});
							try {
								callback();
							} catch (const std::exception& ex) {
								std::cerr << ex.what() << std::endl; // TODO better error handling
							}
						});
					
					sTimers.emplace(inTimerID, timer);
					timer.start();
					
//					auto	waitTime	= boost::posix_time::milliseconds(std::max((double)0, inMilliSeconds));
//					dispatch::queue&	theStrand(strandFromContext(inCallback.context()));
//					Timer	timer = std::make_shared<timer_type>(theStrand.get_io_service());
//					sTimers[inTimerID] = timer;
//					timer->expires_from_now(waitTime);
//					timer->async_wait(theStrand.wrap(boost::bind(&Timeout::timerDidFire, _1,
//																 (callback_type)inCallback, inTimerID)));
				}
			
				// synchronized on JS because it executes on the strand corresponding to the
				// callback context group
//				static void timerDidFire(boost::system::error_code ec, const callback_type& inCallback, int inTimerID) {
//					timerStrand().dispatch(boost::bind(&Timeout::removeTimerWithID, inTimerID));
//					if (ec != boost::asio::error::operation_aborted) {
//						try {
//							inCallback();
//						} catch (const std::exception& ex) {
//							std::cerr << ex.what() << std::endl; // TODO better error handling
//						}
//					}
//				}

				static void cancelTimerWithID(int inTimerID) {
					auto	it = sTimers.find(inTimerID);
					if (it != sTimers.end()) {
//						it->second->cancel();
						it->second.stop();
					}
				}
				
//				typedef boost::asio::deadline_timer		timer_type;
//				typedef jerome::shared_ptr<timer_type>	Timer;
				typedef jerome::Map<int, dispatch::timer>		Timers;
				static	Timers							sTimers;
				static	std::atomic<int>				sTimerCounter;
			};
			
      // STATIC
			Timeout::Timers		Timeout::sTimers;
			std::atomic<int>	Timeout::sTimerCounter(0);
		}

	}
	
	namespace javascript {
		
		// JS description for XMLDoc class. We store the pointer to xmlDoc and free it when the document is GC'd
		template <>
		struct ClassTraits<xmlDocPtr> : DefaultClassTraits<xmlDocPtr> {
			typedef DefaultClassTraits<xmlDocPtr>	parent_type;
			ClassTraits();
			void destroy(void* ptr) override { if (ptr) xmlFreeDoc((xmlDocPtr)ptr); }
		};

		// JS representation of a xml node. We store the pointer and the xml document should care of deallocating.
		// it might be dangerous when JS keeps a reference to a node and discards the document.
		template <>
		struct ClassTraits<xmlNodePtr> : DefaultClassTraits<xmlNodePtr> {
		};
		
	}
}

namespace jerome {
	namespace scripting {
		namespace detail {
		

		}

		namespace js = jerome::javascript;
		namespace det = jerome::scripting::detail;

    // STATIC
		static const rgx::regex	URL_REGEX("^(((\\w+)\\:\\/\\/)?([^\\/\\?\\#\\s]*))(\\/[^\\?\\#\\s]*)?((\\?([^\\s\\#]*))?(\\#([^\\s]*))?)");


#ifdef JEROME_IOS
#define stringWithContentsOfURL_implemented
		bool ScionPlatform::stringWithContentsOfURL(const String& inURL, String& outContent, String& outError) {
			outError	= "";
			
			bool			result	= false;
			
			SInt32			errorCode;
			CFDataRef		resourceData;
			CFURLRef		url					= CFURLCreateWithBytes(kCFAllocatorDefault,
																	   (const UInt8*)inURL.c_str(), inURL.length(),
																	   kCFStringEncodingUTF8, NULL);
			CFDictionaryRef	properties;
			CFArrayRef		desiredProperties	= CFArrayCreate(kCFAllocatorDefault, NULL, 0, &kCFTypeArrayCallBacks);
			
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
			// TODO replace CFURLCreateDataAndPropertiesFromResource
			// CFURLCreateDataAndPropertiesFromResource is deprecated as of 10.9. I should replace it eventually,
			if (CFURLCreateDataAndPropertiesFromResource(kCFAllocatorDefault,
														 url, &resourceData,
														 &properties, desiredProperties,
														 &errorCode)) {
#pragma clang diagnostic pop
				
				outContent.assign((const char*)CFDataGetBytePtr(resourceData), CFDataGetLength(resourceData));
				CFRelease(resourceData);
				CFRelease(properties);
				result	= true;
			} else {
				outError = "error occured:";
				outError += errorCode;
			}
			
			CFRelease(desiredProperties);
			CFRelease(url);
			return result;
		}
		
		
#else
		//#define stringWithContentsOfURL_implemented
		bool ScionPlatform::stringWithContentsOfURL(const String& inURL, String& outContent, String& outError) {
			outError	= "stringWithContentsOfURL unimplemented";
			return false;
		}
		
		
#endif

		// getDocumentFromUrl : function(url,cb){}
		// Get an XML document from a URL url and invoke the callback cb, which has
		// the signature function(err : Error, doc : Document){}.
		void ScionPlatform::getDocumentFromUrl(const String& url, const xml_doc_callback& cb) {
#ifdef stringWithContentsOfURL_implemented
			String		error	= "";
			String		content;
			xmlDocPtr	doc		= nullptr;
			if (stringWithContentsOfURL(url, content, error)) {
				doc	= xmlReadMemory(content.c_str(), (int)content.length(), "noname.xml", NULL, 0);
				if (doc) {
					cb(nullptr, doc);
				} else {
					cb("failed to parse the xml document.", doc);
				}
			} else {
				cb(error.c_str(), doc);
			}
#endif
		};
		
		// Parse an XML document from string str, and return the parsed document.
		js::Value ScionPlatform::parseDocumentFromString(const String& content) {
			xmlDocPtr	doc	= xmlReadMemory(content.c_str(), (int)content.length(), "noname.xml", NULL, 0);
			return doc
				? js::Context::currentContext().newObjectOfNativeClass(doc)
				: js::Context::currentContext().nullValue();
		};
		
		// getDocumentFromFilesystem : function(path,cb){}
		// Parse an XML document from path, a string representing a path on the filesystem.
		// The parsed document is passed to the callback cb, which has
		// the signature function(err : Error, doc : Document){}.
		void ScionPlatform::getDocumentFromFilesystem(const String& path, const xml_doc_callback& cb) {
			xmlDocPtr	doc	= xmlReadFile(path.c_str(), NULL, 0);
			cb(doc ? nullptr : "failed to read and parse document", doc);
		};
		
		// getResourceFromUrl : function(url,cb){}
		// Get a resource from a url, which may be an 'http:' URL, or filesystem path,
		// and pass it to the callback cb as a string. Callback cb has
		// the signature function(err : Error, str : String){}.
		// Internally, this is used to resolve SCXML <script src=""/> tags.
		void ScionPlatform::getResourceFromUrl(const String& url, const string_rsrc_callback& cb) {
#ifdef stringWithContentsOfURL_implemented
			String		error	= "";
			String		content;
			//						std::cout << "asking for " << (String)url << " ";
			if (stringWithContentsOfURL(url, content, error)) {
				cb(nullptr, content);
				//							std::cout << "SUCCEDED" << std::endl;
				//							std::cout << content << std::endl;
			} else {
				cb(error.c_str(), content);
				//							std::cout << "FAILED" << std::endl;
			}
#endif
		};
		
		// postDataToUrl : function(url,data,cb){}
		void ScionPlatform::postDataToUrl(const String& url, js::Value data, js::Value cb) {
			throw js::illegal_arguments();
		};
		
		// setTimeout : function(cb,timeout){}
		// This function usually delegates to the setTimeout function in the browser or in node.js.
		// It is intended for environments driven by a main event loop, but can be implemented using
		// threads as well. In this case scxml.gen should be synchronized.
		// Internally, this is used by SCION to implement SCXML send/@delay.
		unsigned ScionPlatform::setTimeout(js::Value cb, double timeout) {
			if (!cb.isFunction()) throw js::illegal_arguments();
			//		std::cout << "SET_TIMEOUT " << arguments[1] << " " << arguments[1].toNumber() << std::endl;
			return det::Timeout::createTimer(cb, timeout);
		};
		
		// clearTimeout : function(timeoutId){}
		// This function usually delegates to the cleartimeout function in the browser or in node.js.
		// Internally, it is used by SCION to implement SCXML cancel.
		void ScionPlatform::clearTimeout(unsigned timeoutID) {
			det::Timeout::cancel(timeoutID);
		};
		
		// log : function(){}
		// Accepts a variable number of string arguments and passes them to a platform-specific logging function.
		// Internally, this is used by SCION to implement SCXML log.
		void ScionPlatform::log() {
      auto args = js::Context::currentArguments();
			std::cout << "log(" << args.size() << ") ";
			for(auto v : args) {
				try {
					std::cout << v.toJSONString(1);
				} catch (...) {
					try {
						std::cout << v;
					} catch (...) {
						std::cout << "UNKNOWN VALUE";
					}
				}
				std::cout << " ";
			}
			std::cout << std::endl;
		};
		
		// eval : function(content,name){}
		// eval evaluates String content. name is an optional string argument representing a file name.
		// This is is used to generate stack traces.
		// Internally, SCION uses lightweight code generation to impelment actions. eval is used evaluate generated
		// action code.
		js::Value ScionPlatform::eval(const String& content, const String& name) {
			// if I create a new context here, the action (script) code
			// will have no access to global variables and functions
			return js::Context::currentContext().evaluateScript("(function(){\nreturn " + content + ";})();", name);
		};
	
		
		//	path.join : function(path1,path2){}
		//	Join two paths, using a platform-specific path separator.
		String ScionPlatform::Path::join(String path1, const String& path2) {
			if (path1.length() && path1.back() == '/') path1.pop_back();
			if (path2.length() && path2.front() == '/') path1 += path2;
			else path1 += "/" + path2;
			return path1;
		};
		
		//	path.dirname : function(path){}
		//	Given a filesystem path, return the directory name corresponding to that path.
		String ScionPlatform::Path::dirname(const String& path) {
			std::size_t	pos = path.find_last_of('/');
			if (pos != String::npos) return path.substr(0, pos);
			return "";
		};
		
		//	path.basename : function(path,ext){}
		//	Given a filesystem path, return the filename. Optionally, return the filesystem without the extension ext.
		String ScionPlatform::Path::basename(String path, String ext) {
			std::size_t	pos = path.find_last_of('/');
			if (pos != String::npos) path = path.substr(pos+1);
			if (js::Context::currentArgumentCount() >= 2) {
				ext = "." + ext;
				if (path.length() > ext.length() && path.substr(path.length()-ext.length()) == ext)
					path = path.substr(0, path.length()-ext.length());
			}
			return path;
		};
		
		//	path.extname : function(path){}
		//	Given a filesystem path return extension of the filename.
		String ScionPlatform::Path::extname(String path) {
			std::size_t	pos = path.find_last_of('/');
			if (pos != String::npos) path = path.substr(pos+1);
			pos = path.find_last_of('.');
			if (pos != String::npos) return path.substr(pos+1);
			return "";
		};
		
		//	url.getPathFromUrl : function(url){}
		//	Given a URL, return the pathname.
		String ScionPlatform::Url::getPathFromUrl(const String& url) {
			
			String		path = "";
		
			rgx::smatch	m;
			if (rgx::regex_match(url, m, URL_REGEX)) {
				path = m[5].str();
			}
			//					std::cout << "extract " << url << " -> " << path << std::endl;
			return path;
		};
		
		//	url.changeUrlPath : function(url,newPath){}
		//	Given a URL, change the path of the URL to newPath, and return the new URL
		String ScionPlatform::Url::changeUrlPath(const String& url, const String& newPath) {
		
			String newURL = rgx::regex_replace (url, URL_REGEX,"$1"+newPath+"$6",
												rgx::regex_constants::format_first_only);
	
			//					std::cout << "replace " << url << " -> " << newURL << std::endl;
			return newURL;
		};
		
		// dom.getChildren : function(node){}
		// Return the children of the DOM node as a JavaScript array.
		std::vector<ScionPlatform::XMLNode> ScionPlatform::Dom::getChildren(XMLNode node) {
			std::vector<XMLNode>	result;
			for(xmlNodePtr aNode = node->children; aNode; aNode = aNode->next) {
				result.push_back(aNode);
			}
			return result;
		};
		
		// dom.localName : function(node){}
		// Return the local name (or tag name) of the DOM node.
		String ScionPlatform::Dom::localName(XMLNode node) {
			return (const char*)node->name;
		};
		
		// dom.getAttribute : function(node,attribute){}
		// Return the attribute value of the attribute attribute on the given DOM node.
		String ScionPlatform::Dom::getAttribute(XMLNode node, const String& attribute) {
      void* value = xmlGetProp(node, (const xmlChar*)attribute.c_str());
      if (!value) {
        return String("");
      }
      String result = String((const char*)value);
      xmlFree(value);
      return result;
		};
		
		// dom.hasAttribute : function(node,attribute){}
		// Return a boolean value indicating whether the node node has the attribute attribute.
		bool ScionPlatform::Dom::hasAttribute(XMLNode node, const String& attribute) {
			return NULL != xmlHasProp(node, (const xmlChar*)attribute.c_str());
		};
		
		// dom.namespaceURI : function(node){}
		// Return the namespace URI of the given DOM node.
		const char* ScionPlatform::Dom::namespaceURI(XMLNode node) {
			return (const char*)(node->ns ? node->ns->href : NULL);
		};
		
		// dom.createElementNS : function(doc,ns,localName){}
		// Create an element, given the DOM document doc, the namesapce ns, and the local name localName.
		ScionPlatform::XMLNode ScionPlatform::Dom::createElementNS(xmlDocPtr doc, const char* ns, const String& localName) {
			xmlNsPtr	theNs	= NULL;
			if (ns) {
				theNs	= xmlSearchNsByHref(doc, xmlDocGetRootElement(doc), (xmlChar*)ns);
				if (!theNs) {
					std::cerr << "Cannot find prefix for " << ns << std::endl;
					//						theNs	= xmlNewNs(xmlDocGetRootElement(theDoc), (xmlChar*)nsURL, NULL);
				}
			}
			return xmlNewNode(theNs, (const xmlChar*)localName.c_str());
		};
		
		// dom.setAttribute : function(node,name,value){}
		// Set the attribute name to value value on the given DOM node.
		void ScionPlatform::Dom::setAttribute(XMLNode node, const String& name, const String& value) {
			xmlNewProp(node, (const xmlChar*)name.c_str(), (const xmlChar*)value.c_str());
		};
		
		// dom.appendChild : function(parent,child){}
		// Append DOM node child to the children of DOM node parent.
		ScionPlatform::XMLNode ScionPlatform::Dom::appendChild(XMLNode parent, XMLNode child) {
			xmlAddChild(parent, child);
			return child;
		};
		
		static int elementIndex(ScionPlatform::XMLNode node) {
			int index = 1;
			for(ScionPlatform::XMLNode n = node; n != node->parent->children; n = n->prev) {
				if (n != node && 0 == strcmp((const char*)node->name, (const char*)n->name))
					index++;
			}
			return index;
		}
		
		static String xpathToNode(ScionPlatform::XMLNode node) {
			if (!node->parent)
				return "";
			if (0 == strcmp((const char*)node->name, "scxml")) {
        void* value = xmlGetProp(node, (const xmlChar*)"name");
        if (!value) {
          return String("/scxml");
        }
        String result = String("/scxml[@name='") + (const char*)value + "']";
        xmlFree(value);
        return result;
			}
			return xpathToNode(node->parent)
				+ "/" + (const char*)node->name + "[" + std::to_string(elementIndex(node)) + "]";
		}

		// dom.textContent : function(node,txt){}
		// Set the text content txt of the DOM node.
		String ScionPlatform::Dom::textContent(XMLNode node, const String& txt) {
			if (js::Context::currentArgumentCount() >= 2) {
				const xmlChar* theTxt	= (const xmlChar*)txt.c_str();
				xmlNodeSetContent(node, (const xmlChar*)"");
				xmlNodeAddContent(node, theTxt);
				return txt;
			} else {
				const xmlChar*	theContent	= xmlNodeGetContent(node);
				String	content((const char*)theContent);
				xmlFree((void*)theContent);
				if (0 == strcmp((const char*)node->name, "script")) {
					String xpath = xpathToNode(node);
					content = "\n/* " + xpath + "/begin */\n"
						+ content
						+ "\n/* " + xpath + "/end */\n";
				}
				return content;
			}
		};
		
		// dom.getElementChildren : function(node){}
		// Return the element children of the DOM node as an array.
		std::vector<ScionPlatform::XMLNode> ScionPlatform::Dom::getElementChildren(XMLNode node) {
			std::vector<XMLNode>	result;
			for(xmlNodePtr aNode = node->children; aNode; aNode = aNode->next) { if (aNode->type == XML_ELEMENT_NODE)
				result.push_back(aNode);
			}
			return result;
		};

		ScionPlatform::Dom ScionPlatform::dom;
		
	}
	
	namespace javascript {
		using namespace jerome::scripting;

		ClassTraits<ScionPlatform>::ClassTraits()
		{
			AL_JS_EXPORT_MEMBER_METHOD(getDocumentFromUrl);
			AL_JS_EXPORT_MEMBER_METHOD(parseDocumentFromString);
			AL_JS_EXPORT_MEMBER_METHOD(getDocumentFromFilesystem);
			AL_JS_EXPORT_MEMBER_METHOD(getResourceFromUrl);
			AL_JS_EXPORT_MEMBER_METHOD(postDataToUrl);
			AL_JS_EXPORT_MEMBER_METHOD(setTimeout);
			AL_JS_EXPORT_MEMBER_METHOD(clearTimeout);
			AL_JS_EXPORT_MEMBER_METHOD(log);
			AL_JS_EXPORT_MEMBER_METHOD(eval);
			
			AL_JS_EXPORT_MEMBER_OBJECT(path);
			AL_JS_EXPORT_MEMBER_OBJECT(url);
			AL_JS_EXPORT_MEMBER_OBJECT(dom);
		}
		
		ClassTraits<ScionPlatform::Path>::ClassTraits()
		{
			AL_JS_EXPORT_MEMBER_METHOD(join);
			AL_JS_EXPORT_MEMBER_METHOD(dirname);
			AL_JS_EXPORT_MEMBER_METHOD(basename);
			AL_JS_EXPORT_MEMBER_METHOD(extname);
		}

		ClassTraits<ScionPlatform::Url>::ClassTraits()
		{
			AL_JS_EXPORT_MEMBER_METHOD(getPathFromUrl);
			AL_JS_EXPORT_MEMBER_METHOD(changeUrlPath);
		}

		ClassTraits<ScionPlatform::Dom>::ClassTraits()
		{
			AL_JS_EXPORT_MEMBER_METHOD(getChildren);
			AL_JS_EXPORT_MEMBER_METHOD(localName);
			AL_JS_EXPORT_MEMBER_METHOD(getAttribute);
			AL_JS_EXPORT_MEMBER_METHOD(hasAttribute);
			AL_JS_EXPORT_MEMBER_METHOD(namespaceURI);
			AL_JS_EXPORT_MEMBER_METHOD(createElementNS);
			AL_JS_EXPORT_MEMBER_METHOD(setAttribute);
			AL_JS_EXPORT_MEMBER_METHOD(appendChild);
			AL_JS_EXPORT_MEMBER_METHOD(textContent);
			AL_JS_EXPORT_MEMBER_METHOD(getElementChildren);
		}


	}
}

namespace jerome { namespace javascript {
	
	struct DocumentElement : public UserProperty<> {
		static JSValueRef get(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef* exc)
		{
			xmlDocPtr	doc = ClassTraits<xmlDocPtr>::instance().representedObject(object);
			detail::CallbackContext	context(ctx, exc);
			return detail::to_value<xmlNodePtr>::convert(context, xmlDocGetRootElement(doc)).returnValueRef();
		}
		
	};
	
	ClassTraits<xmlDocPtr>::ClassTraits() {
		AL_JS_EXPORT_USER_PROPERTY_AS(DocumentElement, "documentElement");
	}
	
}}
