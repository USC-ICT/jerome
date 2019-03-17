//
//  error.hpp
//
//  Created by Anton Leuski on 9/30/15.
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

#ifndef __jerome_type_error_hpp__
#define __jerome_type_error_hpp__

#include <iostream>

#include <exception>
#include <boost/variant.hpp>
#include <jerome/type/macros.hpp>
#include <jerome/type/string.hpp>

namespace jerome {

  struct Exception : public std::logic_error {
    typedef std::logic_error parent_type;
    using parent_type::parent_type;
    const char* what() const AL_NOEXCEPT override {
      const char* s = parent_type::what();
      if (!strlen(s)) s = typeid(*this).name();
        return s;
    }
    Exception() : parent_type("") {}
  };

  #define JEROME_EXCEPTION(name) \
    struct name : public jerome::Exception { \
    using jerome::Exception::Exception; \
    name() : jerome::Exception(#name) {} };

  struct OptionalError;
  
  struct JEROME_SYMBOL_EXPORT Error : std::exception {
    
    // STATIC
    static const OptionalError NO_ERROR;
    
    explicit Error(String inDescription, String inRecoverySuggestion = "")
    : mDescription(std::move(inDescription))
    , mRecoverySuggestion(std::move(inRecoverySuggestion))
    {}
    
    explicit Error(String inDescription, Error inReason)
    : mDescription(std::move(inDescription))
    , mReason(std::make_shared<Error>(std::move(inReason)))
    {}
    
    Error(const Error&) = default;
    Error(Error&&) = default;
    
    Error& operator=(const Error&) = default;
    Error& operator=(Error&&) = default;
    
    const String& description() const AL_NOEXCEPT { return mDescription; }
    const String& recoverySuggestion() const AL_NOEXCEPT { return mRecoverySuggestion; }
    const char* what() const AL_NOEXCEPT override {
      mBuffer = mDescription;
      if (mReason) {
        mBuffer += " because of\n";
        mBuffer += mReason->what();
      }
      return mBuffer.c_str();
    }
  private:
    String mDescription;
    String mRecoverySuggestion;
    std::shared_ptr<Error> mReason;
    mutable String mBuffer;
    
    
  };
  
  class OptionalError : public optional<Error>
  {
    typedef optional<Error> parent_type;
  public:
    // as of boost 1.54 and clang 6.1 you cannot use the convinient using parent
    // constructor notation. The compiler finds an issue with the class somewhere
    // deep in the templates (i.e., enable_if<>) and respond with a criptic error.
    // Overwriting the constructors explicitly seems to solve the problem. 6 hours
    // wasted...
    //		using parent_type::parent_type;
    OptionalError() {}
    OptionalError(const Error& e) : parent_type(e) {}
    OptionalError(Error&& e) : parent_type(std::move(e)) {}
  };
  
  template <class T>
  class Result : boost::variant<Error, T>
  {
    typedef boost::variant<Error, T> parent_type;
    
  public:
    // as of boost 1.54 and clang 6.1 you cannot use the convinient using parent
    // constructor notation. The compiler finds an issue with the class somewhere
    // deep in the templates (i.e., enable_if<) and> respond with a criptic error.
    // Overwriting the constructors explicitly seems to solve the problem.
    //		using parent_type::parent_type;
    
    Result(const Result<T>& e) = default; // : parent_type(e) {}
    Result(Result<T>&& e) = default; // : parent_type(std::move(e)) {}
    
    template <typename U>
    Result(U&& u) : parent_type(std::forward<U>(u)) {}
    
    //		Result(const Error& e) : parent_type(e) {}
    //		Result(const T& e) : parent_type(e) {}
    //		Result(Error&& e) : parent_type(std::move(e)) {}
    //		Result(T&& e) : parent_type(std::move(e)) {}
    
    operator bool () const { return boost::get<Error>(this) == nullptr; }
    const Error& error() const { return boost::get<Error>(*this); }
    const T& value() const { return boost::get<T>(*this); }
    Error error() { return boost::get<Error>(*this); }
    T& value() { return boost::get<T>(*this); }
  };

  inline
  std::ostream& operator << (std::ostream& os, const std::exception& error)
  {
    os << error.what();
    return os;
  }
  
}

#endif // defined __jerome_type_error_hpp__
