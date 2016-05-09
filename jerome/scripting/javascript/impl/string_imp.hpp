//
//  string_imp.hpp
//
//  Created by Anton Leuski on 8/20/14.
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
		std::auto_ptr<char> buffer(new char[size]);
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