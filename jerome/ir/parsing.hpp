//
//  parsing.hpp
//
//  Created by Anton Leuski on 9/30/11.
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

#ifndef __jerome_ir_parsing_hpp__
#define __jerome_ir_parsing_hpp__

#include <exception>
#include <list>
#include <boost/parameter.hpp>
#include <jerome/types.hpp>

#include <jerome/ir/parsing/token.hpp>
#include <jerome/ir/collection.hpp>
#include <jerome/ir/parsing/filters.hpp>

#ifdef JEROME_IOS
#	include <jerome/ir/parsing/parsing_cf.hpp>
#elif defined(JEROME_ANDROID)
#warning "Fix locale and Unicodde support!"
// Android NDK doesn't support cf, and boost locale is a pain to built.
// Thus we skip the whole boost local and create another parsing_std parser that uses only standatd C++.
// This parser only works for simple token parsing and does not support different locale or Unicode.
#   include <jerome/ir/parsing/parsing_std.hpp>
#else
#	include <jerome/ir/parsing/parsing_icu.hpp>
#endif

#include <jerome/ir/parsing/parsing_pipes.hpp>


#endif // defined __jerome_ir_parsing_hpp__
