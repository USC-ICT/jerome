//
//  parsing_icu.cpp
//
//  Created by Anton Leuski on 4/15/12.
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

#include <jerome/ir/parsing.hpp>

#ifndef JEROME_IOS

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>

namespace jerome { 

	void Locale::global(const String& inLocaleString) {
		boost::locale::generator gen;
		std::locale::global(gen(inLocaleString.c_str()));
	}

	namespace ir {

	namespace filter {

// -----------------------------------------------------------------------------	
#pragma mark - Lowercase
	
	bool Lowercase::getNextToken(Token& ioToken) {
		if (!TokenFilter::getNextToken(ioToken)) return false;

		String	newTokenText	= boost::locale::to_lower(ioToken.text(), locale());
		if (newTokenText != ioToken.text()) {
			ioToken.text() = std::move(newTokenText);
		}
		
		return true;
	}

	// -----------------------------------------------------------------------------	
#pragma mark - Alphanumeric
	
  // a lot depends on how the locale is defined.
  // note that
  //    std::locale loc("en_US.UTF-8");
  // works for utf8 encoding.
	bool Alphanumeric::getNextToken(Token& ioToken) {
		while (TokenFilter::getNextToken(ioToken)) {
      // we assume utf8 internal encoding
      auto inp = boost::locale::conv::utf_to_utf<wchar_t>(ioToken.text());
      if (boost::algorithm::all(inp, boost::algorithm::is_alnum(locale())))
        return true;
		}
		return false;
	}
	
	// -----------------------------------------------------------------------------	
#pragma mark - Alpha
	
	bool Alpha::getNextToken(Token& ioToken) {
		while (TokenFilter::getNextToken(ioToken)) {
      auto inp = boost::locale::conv::utf_to_utf<wchar_t>(ioToken.text());
      if (boost::algorithm::all(inp, boost::algorithm::is_alpha(locale())))
        return true;
		}
		return false;
	}
	
	} // namespace filter

}}

#endif

