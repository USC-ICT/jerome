//
//  scored.hpp
//
//  Created by Anton Leuski on 5/23/13.
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
