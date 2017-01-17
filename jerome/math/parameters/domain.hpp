//
//  domain.hpp
//
//  Created by Anton Leuski on 8/5/15.
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

#ifndef __jerome_math_parameters_domain_hpp__
#define __jerome_math_parameters_domain_hpp__


namespace jerome {
namespace math {
namespace parameters {

	struct Domain {
		typedef double	value_type;
		bool 			contains(value_type inValue) const { return false; }
		value_type 		constrain(value_type inValue) { return inValue; }
		value_type		length() const { return 0; }
	};
	
	struct RangeDomain : public Domain {
		RangeDomain(value_type inMinimum = 0, value_type inMaximum = 1) : mMinimum(inMinimum), mMaximum(inMaximum) {}
		RangeDomain(const RangeDomain& inSource) : mMinimum(inSource.minimum()), mMaximum(inSource.maximum()) {}
		bool 			contains(value_type inValue) const { return (inValue >= minimum() && inValue <= maximum()); }
		value_type 		constrain(value_type inValue) { return inValue < minimum() ? minimum() : inValue > maximum() ? maximum() : inValue; }
		value_type		length()  const { return maximum() - minimum(); }
		value_type		minimum() const { return mMinimum; }
		value_type		maximum() const { return mMaximum; }
	private:
		friend std::ostream& operator << (std::ostream& outs, const RangeDomain& value);
		
		value_type		mMinimum, mMaximum;
	};
	
	std::ostream& operator << (std::ostream& outs, const RangeDomain& value);

	typedef	List<RangeDomain>	range_vector;

	List<RangeDomain::value_type> mean(const range_vector& ranges);
	List<RangeDomain::value_type> random(const range_vector& ranges);
  List<RangeDomain::value_type>
  clumpToMean(const List<RangeDomain::value_type>& value,
              const range_vector& ranges);

}
}
}

#endif // defined __jerome_math_parameters_domain_hpp__
