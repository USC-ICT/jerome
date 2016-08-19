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
	
	struct UValidator {
		typedef int32_t												my_wchar_t;
		typedef boost::locale::utf::utf_traits<String::value_type>	inp_utf_traits_type;
		typedef boost::locale::utf::utf_traits<my_wchar_t>			out_utf_traits_type;
		typedef int8_t (*fn_type)(my_wchar_t inValue);
		
		UValidator(fn_type inFN, const jerome::Locale& inLocale) : mFN(inFN), mLocale(inLocale) {}
		
		bool isValid(const String& inString) {
			String::const_iterator	b	= inString.begin();
			String::const_iterator	e	= inString.end();
			boost::locale::utf::code_point	cp;
			my_wchar_t						out_buffer;
			
//			typedef	std::ctype<wchar_t>	ctype_type;			
//			const ctype_type& ct = std::use_facet<ctype_type>(mLocale);
//			ct.is(ctype_type::alpha, cp);
// OK, after about 4 hours trying to find how to check unicode character class -- 
// a feature that is standard in Java -- I found no good way but to use ICU library from IBM.
// Aparently the standard C++ approach you see above only works for ASCII characters. Bummer.
			
			while (b != e) {
				cp = inp_utf_traits_type::decode(b, e);
				if (cp == boost::locale::utf::illegal || cp == boost::locale::utf::incomplete) break;
				out_utf_traits_type::encode(cp, &out_buffer);
				if ((*mFN)(out_buffer)) return true;
			}
			return false;
		} 
		
	private:
		fn_type					mFN;
		const jerome::Locale&		mLocale;
	};
	
	bool Alphanumeric::getNextToken(Token& ioToken) {
		UValidator	validator(&u_isalnum, locale());

		while (TokenFilter::getNextToken(ioToken)) {
			if (validator.isValid(ioToken.text())) return true;
		}
		return false;
	}
	
	// -----------------------------------------------------------------------------	
#pragma mark - Alpha
	
	bool Alpha::getNextToken(Token& ioToken) {
		UValidator	validator(&u_isalpha, locale());
		while (TokenFilter::getNextToken(ioToken)) {
			if (validator.isValid(ioToken.text())) return true;
		}
		return false;
	}
	
	} // namespace filter

}}

#endif

