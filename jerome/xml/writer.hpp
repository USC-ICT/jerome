//
//  writer.hpp
//
//  Created by Anton Leuski on 8/15/15.
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

#ifndef __jerome_xml_writer_hpp__
#define __jerome_xml_writer_hpp__

#include <iosfwd>
#include <jerome/types.hpp>
#include <jerome/xml/xml_fwd.hpp>

namespace jerome {
  namespace xml {
		namespace detail {
			class writer_impl;
		}
    class writer
    {
		public:
      writer(std::ostream&);
      writer(document&);
      ~writer();

      std::ostream& stream();
			bool isOpen() const;
			void close();

    private:
      writer(const writer&) = delete;
      writer(writer&&) = delete;
      writer& operator = (const writer&) = delete;
      writer& operator = (writer&&) = delete;

      std::unique_ptr<detail::writer_impl>  mImpl;
    };
  }
}

#endif // defined __jerome_xml_writer_hpp__
