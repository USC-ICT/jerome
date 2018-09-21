//
//  parsing_icu.cpp
//  jerome
//
//  Created by Wei-Wen Feng on ??/??/??.
//  Copyright (c) 2016 USC/ICT. All rights reserved.
//

#ifdef JEROME_ANDROID // Android only build
#include <jerome/ir/parsing.hpp>
#include <unicode/uchar.h>
//#include <boost/locale.hpp>


namespace jerome {

	void Locale::global(const String&  inLocaleString) {
		//boost::locale::generator gen;
		//std::locale::global(gen(inLocaleString)); 
		// Using only std::locale instead of boost::locale.
		std::locale::global(std::locale(inLocaleString.c_str()));
	}

	namespace ir {

	namespace filter {

// -----------------------------------------------------------------------------	
#pragma mark - Lowercase
	
	bool Lowercase::getNextToken(Token& ioToken) {
		if (!TokenFilter::getNextToken(ioToken)) 
			return false;

		String	newTokenText	= ioToken.text();
		for (unsigned int i=0;i<newTokenText.size();i++)
		{
			newTokenText[i] = std::tolower(newTokenText[i]);
		}
		if (newTokenText != ioToken.text()) {
			ioToken.text() = std::move(newTokenText);
		}
		
		return true;
	}

	// -----------------------------------------------------------------------------	
#pragma mark - Alphanumeric
	
	struct UValidator {		
		
		static bool isAlphanumeric(const String& inString) {
			for (unsigned int i=0;i<inString.size();i++)
			{
				if (isalnum(inString[i]))
				return true;
			}			
			return false;
		} 
		
		static bool isAlpha(const String& inString) {
			for (unsigned int i=0;i<inString.size();i++)
			{
				if (isalpha(inString[i]))
				return true;
			}			
			return false;
		} 
		

	};
	
	bool Alphanumeric::getNextToken(Token& ioToken) {		
		for(;TokenFilter::getNextToken(ioToken);) {
			if (UValidator::isAlphanumeric(ioToken.text())) 
			return true;
		}
		return false;
	}
	
	// -----------------------------------------------------------------------------	
#pragma mark - Alpha
	
	bool Alpha::getNextToken(Token& ioToken) {		
		for(;TokenFilter::getNextToken(ioToken);) {
			if (UValidator::isAlpha(ioToken.text())) 
			return true;
		}
		return false;
	}
	
	} // namespace filter

}}

#endif

