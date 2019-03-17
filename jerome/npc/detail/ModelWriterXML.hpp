//
//  ModelWriterXML.hpp
//
//  Created by Anton Leuski on 9/4/15.
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

#ifndef __jerome_npc_detail_ModelWriterXML_hpp__
#define __jerome_npc_detail_ModelWriterXML_hpp__

#include <jerome/npc/model.hpp>
#include <jerome/xml/writer.hpp>
#include <jerome/npc/detail/ModelWriter.hpp>


namespace jerome {
  namespace npc {
    namespace detail {

      class ModelWriterXML
        : public ModelWriter
      {
      public:
        ModelWriterXML(std::ostream& os);

        static OptionalError writeCollection(std::ostream& os,
                                   const Collection& inCollection);

				static void write(std::ostream&, const Utterance& inUtterance);
				static void write(std::ostream&, const State::ranker_model_type& inRankerModel);

      private:
        xml::writer mWriter;
        Map<ModelObject, String>  mObject2ID;

				String idForObject(const ModelObject& inObject);
				String idForExistingObject(const ModelObject& inObject) noexcept(false);
				
				void start(const ModelObject& inObject, const String& inElement);
				void start(const NamedModelObject& inObject, const String& inElement);

        void write(const Utterance& utterance) noexcept(false);
        void write(const Link& link) noexcept(false);
        void write(const Domain& domain) noexcept(false);
        void write(const Lattice& lattice) noexcept(false);
        void write(const State& state) noexcept(false);
        void write(const Collection& collection) noexcept(false);

				std::ostream& stream();

				struct SeqWriter;
      };

    }
  }
}

#endif // defined __jerome_npc_detail_ModelWriterXML_hpp__
