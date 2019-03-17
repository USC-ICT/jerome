//
//  macros.hpp
//
//  Created by Anton Leuski on 9/30/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#ifndef __jerome_type_macros_hpp__
#define __jerome_type_macros_hpp__

#include <boost/version.hpp>

#if BOOST_VERSION < 105400
// we need 1.54 for the variant type to support movable objects
#error "Jerome requires boost version 1.54 or greater"
#elif BOOST_VERSION < 105900
// it means, it would probably compile with no or small changes
#warning "Jerome prefers boost version 1.59 or greater"
#endif


#ifdef __ANDROID__
# define JEROME_ANDROID
#elif __APPLE__
# define JEROME_IOS
#endif

#if !defined(_WIN32) && !defined(__WIN32__) && !defined(WIN32)
#  define JEROME_SYMBOL_EXPORT __attribute__((__visibility__("default")))
#  define JEROME_SYMBOL_IMPORT
#  define JEROME_SYMBOL_VISIBLE __attribute__((__visibility__("default")))
#endif

#ifndef CATCH_CPP11_OR_GREATER
#	if (__cplusplus >= 201103L)
#		define CATCH_CPP11_OR_GREATER 1
#	elif defined(__clang__)
#		if __has_feature(cxx_noexcept)
#			define CATCH_CPP11_OR_GREATER 1
#		endif
#	elif defined(__ANDROID__)
#		define CATCH_CPP11_OR_GREATER 1
#	else
#		define CATCH_CPP11_OR_GREATER 0
#	endif
#endif

#if CATCH_CPP11_OR_GREATER
#  define AL_NOEXCEPT noexcept
#  define AL_NOEXCEPT_(x) noexcept(x)
#  define AL_NOEXCEPT_OR_FALSE(x) noexcept(x)
#else
#  define AL_NOEXCEPT throw()
#  define AL_NOEXCEPT_(x)
#  define AL_NOEXCEPT_OR_FALSE(x) false
#endif

#endif // defined __jerome_type_macros_hpp__
