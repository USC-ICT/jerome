//
//  RankerFactory.hpp
//
//  Created by Anton Leuski on 9/1/15.
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

#ifndef __jerome_npc_factories_RankerFactory_hpp__
#define __jerome_npc_factories_RankerFactory_hpp__

#include <jerome/type/Factory.hpp>
#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/math/parameters/types.hpp>

namespace jerome {

	namespace npc {
	
		class State;
		
    class RankerFactory
      : public Factory<
        RankerFactory,
				detail::Ranker,
				const State&,
        const detail::Data&,
        const math::parameters::value_vector&>
    {
    public:
			typedef Factory<
        RankerFactory,
				detail::Ranker,
				const State&,
        const detail::Data&,
        const math::parameters::value_vector&> parent_type;
		
			static constexpr const char* CLASSIFIER_RECORD = "classifier";
		
      RankerFactory();
			Result<object_type> make(const State&,
        const detail::Data&,
        const math::parameters::value_vector&);
			
      template <typename BaseRanker>
			void registerProvider();
			
    };

		namespace detail {
      template <typename BaseRanker>
      struct DefaultRankerProvider
        : public RankerFactory::Provider
      {
				static constexpr const char* IDENTIFIER = BaseRanker::IDENTIFIER;
			
        Result<Ranker> provide(const State& inState,
                               const Data& inData,
                               const ir::value_vector& inParams) override
        {
					typedef Providable<ClassifierImplementationTemplate<BaseRanker>> Impl;
					auto impl = std::make_shared<Impl>(String(IDENTIFIER), inState, inData);
					if (!impl) return Error("failed to instantiate ranker");
					auto ranker = Ranker(impl);
					ranker.setValues(inParams);
					// make sure the ranker model stored in the state matches the actual ranker 
					ranker.state().setRankerModel(ranker.model());
					return ranker;
        }

      };
		}
		
		template <typename BaseRanker>
		void RankerFactory::registerProvider()
		{
			registerProviderClass<detail::DefaultRankerProvider<BaseRanker>>();
		}

    struct UtteranceCLRankerModel
    {
      static constexpr const char* IDENTIFIER = "jerome.ranker.cl";
      static constexpr const char* ANSWER_WEIGHTING_KEY = "answer.weighting";
      static constexpr const char* QUESTION_WEIGHTING_KEY = "question.weighting";
      static constexpr const char* ANSWER_ANALYZER_KEY = "answer.analyzer";
      static constexpr const char* QUESTION_ANALYZER_KEY = "question.analyzer";
    };
    
	}
}

#endif // defined __jerome_npc_factories_RankerFactory_hpp__
