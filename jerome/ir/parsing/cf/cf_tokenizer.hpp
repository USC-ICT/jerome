//
//  cf_tokenizer.hpp
//
//  Created by Anton Leuski on 9/19/18.
//  Copyright © 2018 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef __jerome_ir_parsing_cf_tokenizer_hpp
#define __jerome_ir_parsing_cf_tokenizer_hpp

#include <jerome/ir/parsing/cf/object.hpp>
#include <jerome/ir/parsing/cf/cf_string.hpp>
#include <jerome/ir/parsing/cf/cf_locale.hpp>
#include <jerome/ir/parsing/token.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wcomma"
#pragma clang diagnostic ignored "-Wdocumentation"
#include <boost/iterator/iterator_facade.hpp>
#pragma clang diagnostic pop

namespace jerome { namespace cf {
  struct Tokenizer {
    Tokenizer();
    Tokenizer(const jerome::String& inString, Locale const & inLocale = Locale());
    Tokenizer(const jerome::String* inString, Locale const & inLocale = Locale());
    Tokenizer(CFStringRef inString, Locale const & inLocale = Locale());
    Tokenizer(const uint8_t* inBytes, std::size_t inLength, bool inDoCopy,
              Locale const & inLocale);
    const Locale& locale() const { return mLocale; }

    template <class token_type>
    token_type nextToken() {
      if (!mTokenizer) return token_type::eos();
      auto  tokenType  = CFStringTokenizerAdvanceToNextToken(mTokenizer);
      if (tokenType == kCFStringTokenizerTokenNone) return token_type::eos();

      CFRange    range    = CFStringTokenizerGetCurrentTokenRange(mTokenizer);
      cf::String  substring(mString.substr(range.location, range.length));
      return token_type((typename token_type::value_type)substring,
                        (typename token_type::size_type)range.location,
                        (typename token_type::size_type)range.length);
    }

  private:
    Locale mLocale;
    String mString;
    basic_object<CFStringTokenizerRef>  mTokenizer;
  };

  typedef ir::BasicToken<String> Token;

  struct TokenizerIterator : public boost::iterator_facade<
    TokenizerIterator, Token const,
    boost::single_pass_traversal_tag
  >
  {
    TokenizerIterator()
    : mTokenizer()
    , mLastToken(Token::eos())
    {}

    /// very hacky. Assuming String containes chars in utf8.
    /// Assuming iterator dereferencing returns pointer to
    /// the start of the char buffer.
    TokenizerIterator(jerome::String::const_iterator inBegin,
                      jerome::String::const_iterator inEnd,
                      Locale const & inLocale = Locale())
    : mTokenizer((const uint8_t*)&(*inBegin), inEnd-inBegin, false, inLocale)
    , mLastToken(mTokenizer.nextToken<Token>())
    {}
  private:
    Tokenizer mTokenizer;
    Token mLastToken;

    friend class boost::iterator_core_access;

    void increment() { mLastToken = mTokenizer.nextToken<Token>(); }

    bool equal(TokenizerIterator const& other) const
    {
      return this->mLastToken.type() == other.mLastToken.type()
        && this->mLastToken.offset() == other.mLastToken.offset();
    }

    Token const& dereference() const { return mLastToken; }
  };

  struct NonTokenizerIterator : public boost::iterator_facade<
    NonTokenizerIterator, Token const,
    boost::single_pass_traversal_tag
  >
  {
    NonTokenizerIterator()
    : mLastToken(Token::eos())
    {}

    /// very hacky. Assuming String containes chars in utf8.
    /// Assuming iterator dereferencing returns pointer to
    /// the start of the char buffer.
    NonTokenizerIterator(jerome::String::const_iterator inBegin,
                         jerome::String::const_iterator inEnd)
    : mLastToken((const jerome::String::value_type*)&(*inBegin),
                 (Token::size_type)(inEnd-inBegin), 0, 0, 0)
    {}
  private:
    Token mLastToken;

    friend class boost::iterator_core_access;

    void increment() { mLastToken = Token::eos(); }

    bool equal(NonTokenizerIterator const& other) const
    {
      return this->mLastToken.type() == other.mLastToken.type()
      && this->mLastToken.offset() == other.mLastToken.offset();
    }

    Token const& dereference() const { return mLastToken; }
  };
}}

namespace jerome {
  namespace range_detail {

    template <class R>
    class tokenized_range :
      public boost::iterator_range<cf::TokenizerIterator>
    {
      typedef cf::TokenizerIterator base_iter;
      typedef boost::iterator_range<base_iter> base;
    public:
      tokenized_range(R& r, const Locale& inLocale)
      : base(base_iter(boost::begin(r), boost::end(r), inLocale), base_iter())
      {}
    };

    struct locale_holder {
      Locale locale;
      locale_holder(const Locale& inLocale)
      : locale(inLocale)
      {}
      locale_holder& operator=(const locale_holder&) = delete;
    };

    struct locale_forwarder
    {
      locale_holder
      operator()(const Locale& inLocale = Locale()) const
      {
        return locale_holder(inLocale);
      }
    };

    template <class SinglePathRng>
    inline tokenized_range<SinglePathRng>
    operator|(SinglePathRng& r,
              const locale_holder& f )
    {
      return tokenized_range<SinglePathRng>(r, f.locale);
    }

    template <class SinglePathRng>
    inline tokenized_range<const SinglePathRng>
    operator|(const SinglePathRng& r,
              const locale_holder& f )
    {
      return tokenized_range<const SinglePathRng>(r, f.locale);
    }
  }

  using range_detail::tokenized_range;

  namespace adaptors
  {
    namespace
    {
      const range_detail::locale_forwarder tokenized =
      range_detail::locale_forwarder();
    }

    template <class SinglePathRng>
    inline tokenized_range<SinglePathRng>
    tokenize(SinglePathRng& rng, const Locale& inLocale)
    {
      return tokenized_range<SinglePathRng>(rng, inLocale);
    }

    template <class SinglePathRng>
    inline tokenized_range<const SinglePathRng>
    tokenize(const SinglePathRng& rng, const Locale& inLocale)
    {
      return tokenized_range<const SinglePathRng>(rng, inLocale);
    }
  } // 'adaptors'

}

namespace jerome {
  namespace range_detail {

    template <class R>
    class nontokenized_range :
    public boost::iterator_range<cf::NonTokenizerIterator>
    {
      typedef cf::NonTokenizerIterator base_iter;
      typedef boost::iterator_range<base_iter> base;
    public:
      nontokenized_range(R& r)
      : base(base_iter(boost::begin(r), boost::end(r)), base_iter())
      {}
    };

    struct nontokenized_forwarder
    {
    };

    template <class SinglePathRng>
    inline nontokenized_range<SinglePathRng>
    operator|(SinglePathRng& r,
              nontokenized_forwarder)
    {
      return nontokenized_range<SinglePathRng>(r);
    }

    template <class SinglePathRng>
    inline nontokenized_range<const SinglePathRng>
    operator|(const SinglePathRng& r,
              nontokenized_forwarder)
    {
      return nontokenized_range<const SinglePathRng>(r);
    }
  }

  using range_detail::nontokenized_range;

  namespace adaptors
  {
    namespace
    {
      const range_detail::nontokenized_forwarder untokenized =
      range_detail::nontokenized_forwarder();
    }
  } // 'adaptors'

}
#endif /* __jerome_ir_parsing_cf_tokenizer_hpp */
