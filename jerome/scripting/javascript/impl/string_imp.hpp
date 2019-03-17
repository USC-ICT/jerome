//
//  string_imp.hpp
//
//  Created by Anton Leuski on 8/20/14.
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

#ifndef __jerome_javascript_string_imp_hpp__
#define __jerome_javascript_string_imp_hpp__

namespace jerome { namespace javascript { namespace detail {
	
	inline JSString::JSString(JSStringRef inJSStringRef)
	: mStringRef(inJSStringRef)
	{
		retain();
	}
	
	inline JSString::~JSString()
	{
		release();
	}
	
	inline JSString::JSString(const JSString& inOther)
	: mStringRef(inOther.mStringRef)
	{
		retain();
	}
	
	inline JSString::JSString(JSString&& inOther)
	: mStringRef(std::move(inOther.mStringRef))
	{
    inOther.mStringRef = nullptr;
	}
	
	inline JSString::JSString(const String& inString)
	: mStringRef(JSStringCreateWithUTF8CString(inString.c_str()))
	{
  }
	
	inline JSString::JSString(const char* inString)
	: mStringRef(JSStringCreateWithUTF8CString(inString))
	{
  }
		
	inline String JSString::string () const {
    if (!mStringRef) return "null";
		std::size_t	size	= JSStringGetMaximumUTF8CStringSize(mStringRef);
		std::unique_ptr<char[]> buffer(new char[size]);
		JSStringGetUTF8CString(mStringRef, buffer.get(), size);
		return String(buffer.get());
	}
	
	inline void JSString::retain() {
		if (mStringRef)
			JSStringRetain(mStringRef);
	}
	
	inline void JSString::release() {
		if (mStringRef) {
			JSStringRelease(mStringRef);
			mStringRef = nullptr;
		}
	}
	
	inline StringRefArray::StringRefArray(const std::vector<String>& arguments)
	: mSize((unsigned int)arguments.size())
	{
		mStrings		= new JSStringRef[arguments.size()];
		for(size_t i = 0, n = size(); i < n; ++i) {
			mStrings[i] = JSStringCreateWithUTF8CString(arguments[i].c_str());
		}
	}
	
	inline StringRefArray::~StringRefArray()
	{
		for(size_t i = 0, n = size(); i < n; ++i) {
			JSStringRelease(mStrings[i]);
		}
		delete mStrings;
	}
	

}}}

#endif // __jerome_javascript_string_imp_hpp__
