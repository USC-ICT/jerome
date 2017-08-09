//
//  parsing_pipes.hpp
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

#ifndef __jerome_ir_parsing_parsing_pipes_hpp__
#define __jerome_ir_parsing_parsing_pipes_hpp__

//#include "parsing.h"

namespace jerome { namespace ir {

	namespace keyword {
		BOOST_PARAMETER_NAME(dictionary)
		BOOST_PARAMETER_NAME(locale)
		BOOST_PARAMETER_NAME(field)
		BOOST_PARAMETER_NAME(unigram_field)
		BOOST_PARAMETER_NAME(bigram_field)
		BOOST_PARAMETER_NAME(stem)
		BOOST_PARAMETER_NAME(stopwords)
		BOOST_PARAMETER_NAME(string)
	}

	using namespace jerome::ir::filter;

	namespace impl {

		template <class Index>
		class TokenPipe : public TokenFilter {
			template <class ArgumentPack>
			static TokenStream make_stream(ArgumentPack const& args) {
			
				using namespace jerome::ir::keyword;

				TokenStream	stream(new Tokenizer(args[_string],
                                         args[_locale | jerome::Locale()]));
				stream	= new Lowercase(stream);
				stream	= new Apostrophe(stream);
        stream  = new Alphanumeric(stream);

				String dictionaryPath				=
          args[_dictionary | Dictionary::defaultDictionaryName()];
				if (dictionaryPath.length())
					stream 	= new Dictionary(stream, dictionaryPath);
				
				if (args[_stem | true]) 
					stream	= new KStem(stream);
					
				const Stopper::Stopwords& stopwords =
          args[_stopwords | Stopper::defaultStopwords()];
        stream	= new Stopper(stream, stopwords);
					
				typename Index::Field*		unigrams =
          args[_unigram_field | ((typename Index::Field*)nullptr)];
				if (unigrams) 
					stream	= new IndexWriter<Index>(stream, *unigrams);

				typename Index::Field*		bigrams =
          args[_bigram_field | ((typename Index::Field*)nullptr)];
				if (bigrams) {
					stream	= new NGram(stream);
					stream	= new IndexWriter<Index>(stream, *bigrams);
				}

				return stream;
			}	
		public:
			template <class ArgumentPack>
			TokenPipe(ArgumentPack const& args)
      : TokenFilter(make_stream(args))
      {}
		};
	
	}
	
	template <class Index>
	class UniversalTokenPipe : public impl::TokenPipe<Index> {
	public:
		BOOST_PARAMETER_CONSTRUCTOR(
			UniversalTokenPipe
			, (impl::TokenPipe<Index>)
			, keyword::tag
			, (required 
				(string,*) ) 
			(optional 
				(locale, 		(jerome::Locale))//,			(jerome::Locale()) )
				(unigram_field,	(typename Index::Field*))//, 				((Field*)NULL) )
				(bigram_field,	(typename Index::Field*))//, 				((Field*)NULL) )
				(stem,			(bool))//, 				(true) )
				(stopwords,		(Stopper::Stopwords*))//,	((Stopper::Stopword*)NULL) )
				))
	};
		
	namespace impl {

		template <class Index>
		class NonTokenizingPipe : public TokenFilter {
			template <class ArgumentPack>
			static TokenStream make_stream(ArgumentPack const& args) {
			
				using namespace jerome::ir::keyword;
				TokenStream	stream(new NonTokenizer(args[_string],
                                            args[_locale | jerome::Locale()]));
				typename Index::Field* field	=
          args[_field | ((typename Index::Field*)nullptr)];
				if (field) 
					stream	= new IndexWriter<Index>(stream, *field);
				return stream;
			}	
		public:
			template <class ArgumentPack>
			NonTokenizingPipe(ArgumentPack const& args)
      : TokenFilter(make_stream(args))
      {}
		};		
	}
	
	template <class Index>
	class NonTokenizingPipe : public impl::NonTokenizingPipe<Index> {
	public:
		BOOST_PARAMETER_CONSTRUCTOR(
			NonTokenizingPipe
			, (impl::NonTokenizingPipe<Index>)
			, keyword::tag
			, (required 
				(string,*) ) 
			(optional 
				(locale, 		(jerome::Locale))//,			(jerome::Locale()) )
				(field,			(typename Index::Field*))//, 				((Field*)NULL) )
				))
	};

}}



#endif // defined __jerome_ir_parsing_parsing_pipes_hpp__
