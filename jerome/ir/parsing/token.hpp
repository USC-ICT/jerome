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

#include <cassert>
#include <jerome/types.hpp>
#include <jerome/ir/parsing/parsing_fwd.hpp>
#include <jerome/ir/parsing/locale.hpp>

namespace jerome {
	namespace ir {
    namespace detail {
      struct TokenBase {
        typedef uint64_t  size_type;

        enum struct Type : int {
          eos = -1, bos = -2, unknown = 0, word, separator
        };

        typedef Type type_type;

        TokenBase(size_type inOffset,
                  size_type inLength,
                  type_type inType)
        : mOffset(inOffset)
        , mLength(inLength)
        , mType(inType)
        {}

        TokenBase(TokenBase&&) = default;
        TokenBase(const TokenBase&) = default;
        TokenBase& operator = (TokenBase&& inSource) = default;
        TokenBase& operator = (const TokenBase& inSource) = default;

        size_type    offset()   const { return mOffset; }
        size_type    length()   const { return isBOS() ? 0 : mLength; }
        size_type    end()      const { return offset() + length(); }
        type_type    type()     const { return mType; }

        bool isEOS() const { return type() == Type::eos; }
        bool isBOS() const { return type() == Type::bos; }

        optional<size_type> documentID() const {
          return isBOS() && mLength != std::numeric_limits<size_type>::max()
          ? mLength : optional<size_type>();
        }

        void setDocumentID(size_type inID) {
          assert(isBOS());
          mLength = inID;
        }

      protected:
        size_type    mOffset;
        size_type    mLength;
        type_type    mType;
      };

      inline bool operator == (const TokenBase& lhs,
                               const TokenBase& rhs)
      {
        return lhs.offset() == rhs.offset()
        && lhs.length() == rhs.length()
        && lhs.type() == rhs.type();
      }
    }

    template <class S = String>
    class BasicToken : public detail::TokenBase {
      typedef detail::TokenBase parent_type;

		public:
      typedef S value_type;
      using parent_type::parent_type;

      template <class Other>
      BasicToken(const BasicToken<Other>& inOther)
      : parent_type(inOther)
      , mText(inOther.text())
      {}

      template <class Other>
      BasicToken(BasicToken<Other>&& inOther)
      : parent_type(std::move(inOther))
      , mText(std::move(inOther.text()))
      {}

      template <class Other>
      BasicToken(const value_type& inText,
                 const Other& inOther)
      : parent_type(inOther)
      , mText(inText)
      {}

      BasicToken(const value_type& inText,
                 size_type inOffset,
                 size_type inLength,
                 type_type inType)
      : parent_type(inOffset, inLength, inType)
      , mText(inText)
      {}

			const value_type&	text() 		const { return mText; }
			value_type&			text()		{ return mText; }

      BasicToken& operator += (const BasicToken& inToken) {
        if (text().size() > 0) {
          text()  += ngramSeparatorText();
        }
        text()    += inToken.text();
        size_type  finish  = std::max(end(), inToken.end());
        mOffset  = std::min(offset(), inToken.offset());
        mLength  = finish - offset();
        return *this;
      }

      static value_type ngramSeparatorText() {
        static const value_type separator = "_";
        return separator;
      }

      static const BasicToken& ngramSeparator() {
        static const BasicToken token(ngramSeparatorText(), 0, 0,
                                      BasicToken::Type::separator);
        return token;
      }

      static const BasicToken& unknown() {
        static const BasicToken token("", 0, 0, BasicToken::Type::unknown);
        return token;
      }

      static const BasicToken& eos() {
        static const BasicToken token("", 0, 0, BasicToken::Type::eos);
        return token;
      }

      static const BasicToken& bos() {
        static const BasicToken token("", 0,
                                      std::numeric_limits<size_type>::max(),
                                      BasicToken::Type::bos);
        return token;
      }

      inline bool operator == (const BasicToken& rhs)
      {
        return static_cast<const TokenBase&>(*this)
          == static_cast<const TokenBase&>(rhs)
        && text() == rhs.text();
      }
    private:
      value_type      mText;
    };

    inline std::ostream& operator << (std::ostream& outs,
                                      const detail::TokenBase::type_type& o)
    {
      typedef detail::TokenBase::type_type Type;
      switch (o) {
        case Type::bos: return outs << "bos";
        case Type::eos: return outs << "eos";
        case Type::unknown: return outs << "??";
        case Type::word: return outs << "word";
        case Type::separator: return outs << "-";
        default: return outs << static_cast<int>(o);
      }
    }

    template <typename T>
    inline std::ostream& operator << (std::ostream& outs, const BasicToken<T>& o) {
      if (o.isBOS()) {
        return outs << "{BEGIN}";
      }
      if (o.isEOS()) {
        return outs << "{END}";
      }
      return outs << "{"
        << "\"" << o.text() << "\", "
        << "(" << o.type() << ")"
        << "[" << o.offset() << "," << o.end() << "]"
        << "}";
    }

    typedef BasicToken<String>  Token;
		
		// -----------------------------------------------------------------------------
		
		struct TokenStream;
		
		namespace i {
			
			struct TokenStreamImpl {
				virtual ~TokenStreamImpl() {}
				virtual bool getNextToken(Token& ioToken) = 0;
				virtual const jerome::Locale& locale() const { return kDefaultLocale; }
				void run() {
          Token tok(Token::unknown());
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
