//
//  writer.hpp
//
//  Created by Anton Leuski on 8/15/15.
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
