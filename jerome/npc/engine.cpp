//
//  engine.cpp
//
//  Created by Anton Leuski on 7/25/15.
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

#include <fstream>

#include <jerome/scripting/javascript.hpp>
#include <jerome/npc/model_cpp.hpp>
#include <jerome/npc/detail/ModelReader.hpp>
#include <jerome/npc/detail/ModelWriter.hpp>

#include <jerome/ir/training/Data.hpp>

#include <jerome/npc/detail/Ranker_impl.hpp>
#include <jerome/npc/detail/evaluation.hpp>
#include <jerome/npc/factories/RankerFactory.hpp>

#include "engine.hpp"

namespace jerome {
  namespace npc {
    namespace detail {

      Engine::Engine()
        : mCollection(cpp::ObjectFactory().makeNew<Collection>())
      {

      }

      struct ParametersParsingVisitor
        : public Record::Visitor
      {
				using Record::Visitor::operator();
        ir::value_vector  params;

        void operator () (const String& inKey, double inValue)
        {
          params.push_back(inValue);
        }
      };

      Result<Ranker> Engine::ranker(const String& inStateName)
      {
        auto found = mRankers.find(inStateName);
        if (found != mRankers.end()) {
          Result<Ranker>  value(const_cast<const Ranker&>(found->second));
          return value;
        }

        auto optState =
          mCollection.states().optionalObjectWithName(inStateName);
        if (!optState)
          return undefined_state_error(inStateName);

        State& state(*optState);

        auto data = dataFromState(state, mCollection.utterance_index());

        ParametersParsingVisitor  visitor;
        auto record = state.rankerModel().at(State::PARAMETERS_KEY, Record());
        record.visit(visitor);

        auto rankerResult = RankerFactory::sharedInstance().make(
          state, data, visitor.params);

        if (rankerResult) {
          auto pair = mRankers.emplace(inStateName, rankerResult.value());
          return pair.first->second;
        }

        return rankerResult;
      }

      std::vector<Utterance>  Engine::search(const String& theNameValue,
                                             const Utterance& query)
      {

        auto ranker_or_error = ranker(theNameValue);
        if (!ranker_or_error) {
          throw ranker_or_error.error();
        }

        typedef std::chrono::time_point<std::chrono::system_clock>	time_point_type;
        time_point_type mStartTime = time_point_type::clock::now();

        Ranker::result_type   rl = ranker_or_error.value()(query);
        // std::cout << *rl;

        time_point_type end = time_point_type::clock::now();
        std::chrono::duration<double> elapsed_seconds = end - mStartTime;
        std::cout << "TIME: " << elapsed_seconds.count() << std::endl;

        std::vector<Utterance>  result;

        for (const Ranker::ranked_list_type::value_type& s : rl[0]) {
          result.push_back(s.object());
        }

        return result;
      };

      Utterance Engine::queryForString(const String& question)
      {
        jerome::npc::cpp::ObjectFactory   of;
        Utterance       query = of.newUtterance();
        query.put(Utterance::kFieldText, question);
        return query;
      }

      std::vector<Utterance>  Engine::answers(const String& inName)
      {
        auto optState = collection().states().optionalObjectWithName(inName);
        return (!optState)
               ? std::vector<Utterance>()
               : std::vector<Utterance>(
          optState->answers().utterances().begin(),
          optState->answers().utterances().end());
      }

      std::vector<Utterance>  Engine::answers()
      {
        return std::vector<Utterance>(collection().utterances().begin(),
          collection().utterances().end());
      }

      optional<Utterance> Engine::utteranceWithID(const String& inUtteranceID)
      {
        // TODO should I cache the index?
        return collection().utterance_index().utteranceWithFieldValue("id",
            inUtteranceID);
      }


      OptionalError  Engine::loadCollection(std::istream& is,
                                            const ObjectFactory& inObjectFactory)
      {
        auto loadResult = jerome::npc::readCollection(inObjectFactory, is);
        if (!loadResult) return loadResult.error();
        mCollection = loadResult.value();
        return Error::NO_ERROR;
      }

      OptionalError  Engine::saveCollection(std::ostream& os)
      {
        return jerome::npc::writeCollection(os, mCollection);
      }

			class state_type_impl : public TrainingState {
			public:
				state_type_impl(const String& inName, Trainer::state_type& src)
				: mName(inName)
				, mSource(src)
				{}
				
				double lastValue() const override
				{
					return mSource.lastValue();
				}
				
				jerome::math::parameters::value_vector lastArgument() const override
				{
					return mSource.lastArgument();
				}
				
				double elapsedTimeInSeconds() const override
				{
					return mSource.elapsedTimeInSeconds();
				}
				
				void stop() override
				{
					mSource.stop();
				}
				
				String name() const override
				{
					return mName;
				}
				
			private:
				const String& mName;
				Trainer::state_type& mSource;
			};


      OptionalError Engine::train(const String& stateName,
                                  const TrainingCallback& callback)
      {
        auto ranker_or_error = ranker(stateName);
        if (!ranker_or_error) return ranker_or_error.error();

        auto ranker = ranker_or_error.value();
        auto trainer = Trainer::trainerFscore();

        auto optState = mCollection.states().optionalObjectWithName(stateName);
        if (!optState) {
          return undefined_state_error(stateName);
        }

        auto data = dataFromState(*optState, mCollection.utterance_index());

        std::pair<Data, Data>   testTrainSplit = data.split(0.05);
        std::pair<Data, Data>   devTrainSplit = testTrainSplit.second.split(0.5);
        trainer.setData(devTrainSplit.second, devTrainSplit.first);

//				std::cout << data.questions().size() << " " << data.answers().size();

				ranker.train(trainer, [&callback, &stateName](Trainer::state_type& state){
					state_type_impl	fakeState(stateName, state);
					callback(fakeState);
				});

				return Error::NO_ERROR;
      }
			
			OptionalError Engine::evaluate(const String& stateName,
																	const Data& inData)
			{
				auto ranker_or_error = ranker(stateName);
				if (!ranker_or_error) return ranker_or_error.error();
				
				auto ranker = ranker_or_error.value();

				auto rankerResult = RankerFactory::sharedInstance().make(ranker.state(),
																																 inData, ranker.values());
				auto xr = rankerResult.value();
				
				std::fstream out("result.html");
				return detail::evaluate(Record(), out, inData, xr);
			}
			
			void Engine::collectionWasUpdated(const OptionalString& inStateName)
			{
				if (inStateName) {
					auto found = mRankers.find(*inStateName);
					if (found != mRankers.end()) {
						mRankers.erase(found);
					}
				} else {
					mRankers.clear();
				}
			}
			
//			void Engine::didCreateStateMachine(const String& name, const String& scxml)
//			{
//				machines.emplace(name, scxml);
//			}
//			
//			void Engine::didDestroyStateMachine(const String& name)
//			{
//				auto i = machines.find(name);
//				if (i != machines.end())
//					machines.erase(i);
//			}
			
    }
  }
}