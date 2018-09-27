//
//  echo.hpp
//
//  Created by Anton Leuski on 9/26/18.
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

#ifndef __jerome_ir_parsing_filter_echo_hpp
#define __jerome_ir_parsing_filter_echo_hpp

namespace jerome { namespace stream {
  namespace stream_detail{
    template <typename OutpuStream>
    struct echo_holder : public stream_filter {
      OutpuStream& ios;
      echo_holder(OutpuStream& inStream)
      : ios(inStream)
      {}

      template <typename OtherOutpuStream>
      inline auto
      operator () (OtherOutpuStream& os) const {
        return echo_holder<OtherOutpuStream>(os);
      }

      template <class Stream, ASSERT_STREAM(Stream)>
      auto operator() (Stream& inStream)
      {
        auto token = inStream.next();
        ios << token << std::endl;
        return token;
      }
    };
  }

  const auto echo = stream_detail::echo_holder<decltype(std::cout)>(std::cout);
}}

#endif // defined __jerome_ir_parsing_filter_echo_hpp
