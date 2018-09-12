//
//  string.hpp
//
//  Created by Anton Leuski on 7/22/14.
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

#ifndef __jerome_type_string_hpp__
#define __jerome_type_string_hpp__

#include <string>
#include <jerome/type/optional.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <boost/algorithm/string.hpp>
#pragma clang diagnostic pop

#if defined(__ANDROID__)
// Android NDK doesn't support these C++ functions. Thus we just
// implement them here to avoid compiling errors.
#include <sstream>
#include <cstdlib>
namespace std
{
	template <typename T>
	inline std::string to_string(T value)
	{
		std::ostringstream os ;
		os << value ;
		return os.str() ;
	}
	int			stoi(const string& str, size_t* idx = 0, int base = 10) { return atoi(str.c_str()); }
	long		stol(const string& str, size_t* idx = 0, int base = 10) { return atol(str.c_str()); }
	float		stof(const string& str, size_t* idx = 0) { return atof(str.c_str()); }
	double	stod(const string& str, size_t* idx = 0) { return atof(str.c_str()); }
}
#endif

namespace jerome {

  typedef std::string		String;
  typedef boost::optional<String>	OptionalString;
  
  inline String trim(const String &s)
  {
    auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
    return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
  }

  bool 	hasSuffix(const String& inS, const String& inSuffix);
  String 	dropSuffix(const String& inS, const String& inSuffix);
  void 	replaceAll(String& ioString, const String& inOld, const String& inNew);
  
  template<typename ... Args>
  String string_format(const String& format, Args ... args)
  {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return String(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
  }
  
  inline String to_lower(const String& inString)
  {
    return boost::algorithm::to_lower_copy(inString);
  }
  
  inline bool stob(const String& inString)
  {
    if (to_lower(inString) == "true") { return true; }
    if (to_lower(inString) == "false") { return false; }
    try {
      return stoi(inString) != 0;
    } catch (std::exception&) {
      return false;
    }
  }
}

#endif // defined __jerome_type_string_hpp__
