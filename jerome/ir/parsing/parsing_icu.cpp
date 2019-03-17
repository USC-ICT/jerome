//
//  parsing_icu.cpp
//
//  Created by Anton Leuski on 4/15/12.
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

#include <jerome/ir/parsing.hpp>

#ifndef JEROME_IOS

#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/cxx11/one_of.hpp>

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
      if (boost::algorithm::one_of(inp, boost::algorithm::is_alnum(locale())))
        return true;
		}
		return false;
	}
	
	// -----------------------------------------------------------------------------	
#pragma mark - Alpha
	
	bool Alpha::getNextToken(Token& ioToken) {
		while (TokenFilter::getNextToken(ioToken)) {
      auto inp = boost::locale::conv::utf_to_utf<wchar_t>(ioToken.text());
      if (boost::algorithm::one_of(inp, boost::algorithm::is_alpha(locale())))
        return true;
		}
		return false;
	}
	
	} // namespace filter

}}

#endif

