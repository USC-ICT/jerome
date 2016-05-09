//
//  string.hpp
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