//
//  string.hpp
//
//  Created by Anton Leuski on 8/20/14.
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

#ifndef __jerome_javascript_string_hpp__
#define __jerome_javascript_string_hpp__

#include <jerome/scripting/javascript/types.hpp>

namespace jerome { namespace javascript { namespace detail {
	
	struct JSString {
		explicit JSString(JSStringRef inJSStringRef);
		~JSString();
		JSString(const JSString& inOther);
		JSString(JSString&& inOther);

		JSString() = delete;
		JSString& operator = (const JSString&) = delete;
		JSString& operator = (JSString&&) = delete;
		
		explicit JSString(const String& inString);
		
		explicit JSString(const char* inString);
		
		JSStringRef jsStringRef() const { return mStringRef; }
		
		String string() const;
	private:
		JSStringRef	mStringRef;
		
		void retain();
		void release();
	};
	
	struct StringRefArray {
		StringRefArray(const std::vector<String>& arguments);
		~StringRefArray();
		
		unsigned int	size() const { return mSize; }
		JSStringRef*	strings() const { return mSize ? mStrings : NULL; }
		
	private:
		StringRefArray() = delete;
		StringRefArray(const StringRefArray&) = delete;
		StringRefArray(StringRefArray&&) = delete;
		StringRefArray& operator = (const StringRefArray&) = delete;
		StringRefArray& operator = (StringRefArray&&) = delete;
		JSStringRef*	mStrings;
		unsigned int	mSize;
	};

}}}


#endif // __jerome_javascript_string_hpp__
