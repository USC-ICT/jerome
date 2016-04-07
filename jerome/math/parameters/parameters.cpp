//
//  parameters.cpp
//
//  Created by Anton Leuski on 2/17/12.
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

#include <sstream>
#include <random>

#include <jerome/math/parameters/parameterized.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace jerome {
namespace math {
namespace parameters {

	std::ostream& operator << (std::ostream& outs, const RangeDomain& value) {
		return (outs << "[" << value.minimum() << ", " << value.maximum() << "]");
	}

}}}

namespace jerome {
namespace math {
namespace parameters {
		
	value_vector	parseVector(const String& inString)
	{
		std::stringstream	s(inString);
		std::vector<double> tmp( (std::istream_iterator<double>(s)), std::istream_iterator<double>());
		value_vector		result(tmp.size());
		boost::copy(tmp, result.begin());
		return result;
	}
	
	List<RangeDomain::value_type> mean(const range_vector& ranges)
	{
		List<RangeDomain::value_type> result;
		for(const auto& r : ranges) {
			result.push_back((r.minimum()+r.maximum()) * 0.5);
		}
		return result;
	}

	List<RangeDomain::value_type> random(const range_vector& ranges)
	{
		List<RangeDomain::value_type> result;
		std::uniform_real_distribution<RangeDomain::value_type>	rand(0,1);
		std::random_device rd;
		std::default_random_engine gen(rd());
		
		for(const auto& r : ranges) {
			result.push_back(r.minimum() + (r.minimum()+r.maximum()) * rand(gen));
		}
		return result;
	}

	
}}}
