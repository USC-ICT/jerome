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

    namespace i {
      class Token {
      public:
        typedef uint32_t  size_type;

      private:
        size_type    mOffset;
        size_type    mLength;
        uint32_t    mType;

      public:

        Token(size_type inOffset = 0, size_type inLength = 0, uint32_t inType = 0) :
        mOffset(inOffset), mLength(inLength), mType(inType) {}

        Token(const Token& inSource)
        : mOffset(inSource.mOffset)
        , mLength(inSource.mLength)
        , mType(inSource.mType)
        {}

        Token(Token&& inSource)
        : mOffset(inSource.mOffset)
        , mLength(inSource.mLength)
        , mType(inSource.mType)
        {}

        Token& operator = (const Token& inSource)
        {
          mOffset = inSource.mOffset;
          mLength = inSource.mLength;
          mType  = inSource.mType;
          return *this;
        }

        Token& operator = (Token&& inSource)
        {
          mOffset = inSource.mOffset;
          mLength = inSource.mLength;
          mType  = inSource.mType;
          return *this;
        }

        size_type    offset()   const { return mOffset; }
        size_type    length()   const { return mLength; }
        size_type    end()    const { return offset() + length(); }
        uint32_t    type()     const { return mType; }

        size_type&    offset()   { return mOffset; }
        size_type&    length()   { return mLength; }
        uint32_t&    type()     { return mType; }

        Token& operator += (const Token& inToken);
      };

    }

		// -----------------------------------------------------------------------------
		
    class Token: i::Token {
			String			mText;
      typedef i::Token super_type;

		public:
			
			Token(const String& inText = "", size_type inOffset = 0, size_type inLength = 0, uint32_t inType = 0) :
			super_type(inOffset, inLength, inType), mText(inText) {}
			
			Token(const String::value_type* inText, size_type inOffset, size_type inLength, uint32_t inType = 0) :
			super_type(inOffset, inLength, inType), mText(inText+inOffset, inLength) {}
			
			Token(const String::value_type* inText, size_type inTextLength, size_type inOffset, size_type inLength, uint32_t inType = 0) :
			super_type(inOffset, inLength, inType), mText(inText, inTextLength) {}
			
			Token(const Token& inSource)
			: super_type(inSource)
      , mText(inSource.mText)
			{}
			
			Token(Token&& inSource)
      : super_type(inSource)
			, mText(std::move(inSource.mText))
			{}
			
			Token& operator = (const Token& inSource)
			{
        super_type::operator = (inSource);
				mText	= inSource.mText;
				return *this;
			}
			
			Token& operator = (Token&& inSource)
			{
        super_type::operator = (inSource);
				mText	= std::move(inSource.mText);
				return *this;
			}
			
			const String&	text() 		const { return mText; }
			String&			text()		{ return mText; }

			Token& operator += (const Token& inToken);
		};
		
		// -----------------------------------------------------------------------------

//    static const jerome::Locale  kDefaultLocale;

		struct TokenStream {
//      optional<token_type> nextToken() { return optional<token_type>(); }
//			const jerome::Locale& locale() const { return kDefaultLocale; }
		};

    template <class TS>
    class TokenFilter: TokenStream {
      typedef TS source_type;
			source_type		mSource;
		public:
			explicit TokenFilter(source_type inSource) : mSource(inSource) {};
			source_type& source() { return mSource; }
      // optional<token_type> nextToken() { return optional<token_type>(); }
			const jerome::Locale& locale() const { return source().locale(); }
		};
	
	}
}

#endif // defined __jerome_ir_parsing_token_hpp__
