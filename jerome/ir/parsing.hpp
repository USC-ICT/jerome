//
//  parsing.hpp
//
//  Created by Anton Leuski on 9/30/11.
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

#ifndef __jerome_ir_parsing_hpp__
#define __jerome_ir_parsing_hpp__

#include <jerome/ir/parsing/parsing_fwd.hpp>
#include <jerome/ir/parsing/locale.hpp>

#include <jerome/ir/parsing/stream.hpp>

#include <jerome/ir/parsing/token.hpp>
#include <jerome/ir/parsing/filters.hpp>

//#undef JEROME_IOS

#if JEROME_PARSING == JEROME_PARSING_CF
#	include <jerome/ir/parsing/parsing_cf.hpp>
#elif JEROME_PARSING == JEROME_PARSING_STD
#warning "Fix locale and Unicode support!"
// Android NDK doesn't support cf, and boost locale is a pain to built.
// Thus we skip the whole boost local and create another parsing_std
// parser that uses only standatd C++.
// This parser only works for simple token parsing and does not support
// different locale or Unicode.
# include <jerome/ir/parsing/parsing_std.hpp>
#elif JEROME_PARSING == JEROME_PARSING_ICU
#	include <jerome/ir/parsing/parsing_icu.hpp>
#else
# error "unknown value for JEROME_PARSING"
#endif

#include <jerome/ir/parsing/parsing_pipes.hpp>

#include <jerome/ir/parsing/filter/alpha.hpp>
#include <jerome/ir/parsing/filter/as_token.hpp>
#include <jerome/ir/parsing/filter/contractions.hpp>
#include <jerome/ir/parsing/filter/dictionary.hpp>
#include <jerome/ir/parsing/filter/echo.hpp>
#include <jerome/ir/parsing/filter/index_writer.hpp>
#include <jerome/ir/parsing/filter/kstem.hpp>
#include <jerome/ir/parsing/filter/lowercased.hpp>
#include <jerome/ir/parsing/filter/ngram.hpp>
#include <jerome/ir/parsing/filter/stopped.hpp>
#include <jerome/ir/parsing/filter/tokenized.hpp>

#endif // defined __jerome_ir_parsing_hpp__
