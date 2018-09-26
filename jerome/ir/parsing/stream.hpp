//
//  stream.hpp
//
//  Created by Anton Leuski on 9/21/18.
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

#ifndef stream_hpp
#define stream_hpp

#include <jerome/type/meta.hpp>
#include <jerome/type/FunctionSignature.hpp>

namespace jerome { namespace stream {

  struct stream_filter {};
  struct stream_base {};

  template<typename T>
  using AssertStream = enable_if_t<is_base_of_v<stream_base, decay_t<T>>, int>;

#define ASSERT_STREAM(T) ::jerome::stream::AssertStream<T> = 0

  template<typename T>
  using AssertFilter = enable_if_t<is_base_of_v<stream_filter, decay_t<T>>, int>;

#define ASSERT_FILTER(T) ::jerome::stream::AssertFilter<T> = 0

  template <class Derived, typename Value>
  struct stream : public stream_base {
    typedef Value value_type;
    value_type next() {
      return static_cast<Derived*>(this)->get_next();
    }
  };

  template <typename Derived>
  struct conditional_filter : public stream_filter {
    template <class Stream, ASSERT_STREAM(Stream)>
    auto operator() (Stream& inStream)
    {
      while (true) {
        auto token = inStream.next();
        if (static_cast<const Derived*>(this)->operator()(token))
          return token;
      }
    }
  };

  template <typename Stream, typename Filter>
  struct filtered_stream : public stream_base {
  private:
    Filter mFilter;
    Stream mStream;
  public:
    typedef decltype(mFilter(mStream)) value_type;
    filtered_stream(Stream s, Filter p)
    : mStream(s)
    , mFilter(p)
    {}
    value_type next() {
      return mFilter(mStream);
    }
  };

  namespace stream_detail {
    template <typename Derived>
    struct locale_based_filter : stream_filter {
      const Locale locale;
      locale_based_filter(const Locale& inLocale = Locale())
      : locale(inLocale) {}
      inline auto operator() (const Locale& inLocale) -> Derived {
        return Derived(inLocale);
      }
    };
  }
}}

namespace jerome {

  template <class Stream, typename Filter, ASSERT_STREAM(Stream), ASSERT_FILTER(Filter)>
  inline auto
  operator|(Stream&& s, const Filter& f)
  {
    typedef decay_t<Stream> Stream_t;
    return stream::filtered_stream<Stream_t, Filter
      >(::std::forward<Stream_t>(s), f);
  }

  template <class Stream, ASSERT_STREAM(Stream)>
  inline auto operator|(Stream&& r, std::tuple<>&& tuple) {
    typedef typename std::remove_reference<Stream>::type Stream_t;
    return std::forward<Stream_t>(r);
  }

  template <class Stream, class ...Args, ASSERT_STREAM(Stream)>
  inline auto operator|(Stream&& r, std::tuple<Args...> tuple) {
    typedef typename std::remove_reference<Stream>::type Stream_t;
    return (std::forward<Stream_t>(r) | head(tuple)) | tail(tuple);
  }

  template <class ...Args>
  inline auto operator|(const String& s, std::tuple<Args...> tuple) {
    return (s | head(tuple)) | tail(tuple);
  }

  template <class A, class ...Args, ASSERT_FILTER(A)>
  inline auto operator|(A r, std::tuple<Args...> tuple) {
    typedef typename std::remove_reference<A>::type A_t;
    return std::tuple_cat(std::make_tuple(r), tuple);
  }

  template <class A, class ...Args, ASSERT_FILTER(A)>
  inline auto operator|(std::tuple<Args...> tuple, A r) {
    return std::tuple_cat(tuple, std::make_tuple(r));
  }

  template <class A, class B, ASSERT_FILTER(A), ASSERT_FILTER(B)>
  inline auto operator|(A a, B b) {
    return std::tuple<A, B>(a, b);
  }
}

#endif // defined stream_hpp
