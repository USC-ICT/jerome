//
//  token.hpp
//
//  Created by Anton Leuski on 11/1/14.
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
