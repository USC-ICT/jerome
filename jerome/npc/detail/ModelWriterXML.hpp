//
//  ModelWriterXML.hpp
//
//  Created by Anton Leuski on 9/4/15.
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
