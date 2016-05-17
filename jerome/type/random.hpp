//
//  random.hpp
//
//  Created by Anton Leuski on 7/22/14.
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

#ifndef __jerome_type_random_hpp__
#define __jerome_type_random_hpp__

#include <random>

namespace jerome {
	template <class T>
	struct rand_base {
		typedef T	value_type;
		inline value_type	rand_value()
		{
      // STATIC
			static auto distribution = new std::uniform_real_distribution<value_type>(0.0,1.0);
			return (*distribution)(generator);
		}
		std::default_random_engine generator;
	};
	
	template <class T>
	struct random : public rand_base<T> {};
}

#endif // defined __jerome_type_random_hpp__
