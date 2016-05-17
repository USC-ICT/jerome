//
//  token.hpp
//
//  Created by Anton Leuski on 11/1/14.
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

#ifndef __jerome_ir_parsing_token_hpp__
#define __jerome_ir_parsing_token_hpp__

namespace jerome {
	
	class Locale;
	
	namespace ir {
		
		// -----------------------------------------------------------------------------
		
		class Token {
		public:
			typedef uint32_t	size_type;
			
		private:
			String			mText;
			size_type		mOffset;
			size_type		mLength;
			uint32_t		mType;
			
		public:
			
			Token(const String& inText = "", size_type inOffset = 0, size_type inLength = 0, uint32_t inType = 0) :
			mText(inText), mOffset(inOffset), mLength(inLength), mType(inType) {}
			
			Token(const String::value_type* inText, size_type inOffset, size_type inLength, uint32_t inType = 0) :
			mText(inText+inOffset, inLength), mOffset(inOffset), mLength(inLength), mType(inType) {}
			
			Token(const String::value_type* inText, size_type inTextLength, size_type inOffset, size_type inLength, uint32_t inType = 0) :
			mText(inText, inTextLength), mOffset(inOffset), mLength(inLength), mType(inType) {}
			
			Token(const Token& inSource)
			: mText(inSource.mText)
			, mOffset(inSource.mOffset)
			, mLength(inSource.mLength)
			, mType(inSource.mType)
			{}
			
			Token(Token&& inSource)
			: mText(std::move(inSource.mText))
			, mOffset(inSource.mOffset)
			, mLength(inSource.mLength)
			, mType(inSource.mType)
			{}
			
			Token& operator = (const Token& inSource)
			{
				mText	= inSource.mText;
				mOffset = inSource.mOffset;
				mLength = inSource.mLength;
				mType	= inSource.mType;
				return *this;
			}
			
			Token& operator = (Token&& inSource)
			{
				mText	= std::move(inSource.mText);
				mOffset = inSource.mOffset;
				mLength = inSource.mLength;
				mType	= inSource.mType;
				return *this;
			}
			
			const String&	text() 		const { return mText; }
			size_type		offset() 	const { return mOffset; }
			size_type		length() 	const { return mLength; }
			size_type		end()		const { return offset() + length(); }
			uint32_t		type() 		const { return mType; }
			
			String&			text()		{ return mText; }
			size_type&		offset() 	{ return mOffset; }
			size_type&		length() 	{ return mLength; }
			uint32_t&		type() 		{ return mType; }
			
			Token& operator += (const Token& inToken);
		};
		
		// -----------------------------------------------------------------------------
		
		struct TokenStream;
		
		namespace i {
			
			struct TokenStreamImpl {
				virtual ~TokenStreamImpl() {}
				virtual bool getNextToken(Token& ioToken) = 0;
				virtual const jerome::Locale& locale() const { return kDefaultLocale; }
				void run() {
					Token tok;
					while (getNextToken(tok));
				}
			private:
        // STATIC
				static const jerome::Locale	kDefaultLocale;
			};
			
		}
		
		struct TokenStream : public ReferenceClassInterface<i::TokenStreamImpl> {
			typedef ReferenceClassInterface<i::TokenStreamImpl> parent_type;
			TokenStream() = default;
			TokenStream(i::TokenStreamImpl* inSource) : parent_type(shared_ptr<implementation_type>(inSource)) {}
			bool getNextToken(Token& ioToken) { return implementation().getNextToken(ioToken); }
			const jerome::Locale& locale() const { return implementation().locale(); }
			void run() { implementation().run(); }
		};
		
		// -----------------------------------------------------------------------------
		
		class TokenFilter : public i::TokenStreamImpl {
			jerome::ir::TokenStream		mSource;
		public:
			explicit TokenFilter(jerome::ir::TokenStream inSource) : mSource(inSource) {};
			jerome::ir::TokenStream& source() { return mSource; }
			const jerome::ir::TokenStream& source() const { return mSource; }
			bool getNextToken(Token& ioToken) { return source().getNextToken(ioToken); }
			const jerome::Locale& locale() const { return source().locale(); }
		};
	
	}
}

#endif // defined __jerome_ir_parsing_token_hpp__
