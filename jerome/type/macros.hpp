//
//  macros.hpp
//
//  Created by Anton Leuski on 9/30/15.
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
//# define JEROME_IOS
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
