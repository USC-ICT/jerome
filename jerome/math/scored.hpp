//
//  scored.hpp
//
//  Created by Anton Leuski on 5/23/13.
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

#ifndef __jerome_math_scored_hpp__
#define __jerome_math_scored_hpp__

namespace jerome { namespace math {
	
	/** ----------------------------------------------------------------------------
	 *
	 */
	
	class Scored {
		double	mScore;
	public:
		Scored() = default;
		Scored(double inScore) : mScore(inScore) {}
		double score() const { return mScore; }
	};
	
	/** ----------------------------------------------------------------------------
	 *
	 */
	
	struct ScoresDescending {
		bool operator() (const Scored& i, const Scored& j) { return i.score() > j.score(); }
	};
	
	/** ----------------------------------------------------------------------------
	 *
	 */
	
	template<typename T>
	class ScoredObject : public Scored {
	public:
		typedef T object_type;
		
		ScoredObject() = default;
		
		ScoredObject(double inScore, const object_type& inObject)
		: Scored(inScore), mObject(inObject) {}
		
		object_type object() const { return mObject; }
	private:
		object_type		mObject;
	};
	
	template <typename T>
	inline std::ostream& operator << (std::ostream& outs, const ScoredObject<T>& o) {
		return outs << o.score() << "\t" << o.object();
	}
		
}}

#endif // defined __jerome_math_scored_hpp__
