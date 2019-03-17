//
//  parameters.cpp
//
//  Created by Anton Leuski on 2/17/12.
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

#include <sstream>
#include <random>

#include <jerome/type/random.hpp>
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

  List<RangeDomain::value_type>
  clumpToMean(const List<RangeDomain::value_type>& value,
              const range_vector& ranges)
  {
    List<RangeDomain::value_type> result;
    for(List<RangeDomain::value_type>::size_type i = 0, n = value.size();
        i < n; ++i)
    {
      const auto& r = ranges[i];
      const auto& v = value[i];
      if (v >= r.minimum() && v <= r.maximum()) {
        result.push_back(v);
      } else {
        result.push_back((r.minimum()+r.maximum()) * 0.5);
      }
    }
    return result;
  }

  List<RangeDomain::value_type> random(const range_vector& ranges)
	{
		List<RangeDomain::value_type> result;
    static jerome::random<RangeDomain::value_type> random_gen(0, 1);
		
		for(const auto& r : ranges) {
			result.push_back(r.minimum() + (r.minimum()+r.maximum()) * random_gen());
		}
		return result;
	}

	
}}}
