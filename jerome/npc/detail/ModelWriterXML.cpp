//
//  ModelWriterXML.cpp
//
//  Created by Anton Leuski on 9/4/15.
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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#pragma clang diagnostic pop

#include <jerome/xml/writer_manip.hpp>
#include <jerome/npc/detail/ModelIO.hpp>
#include "ModelWriterXML.hpp"

namespace jerome {
  namespace npc {
    namespace detail {

			struct ModelWriterXML::SeqWriter {
				template <typename Seq>
				static void writeSeq(ModelWriterXML& wrt,
														 const Seq& seq,
														 const String& inName)
				{
					wrt.stream() << xml::startElement(inName);
					for (const auto& item : seq) {
						wrt.write(item);
					}
					wrt.stream() << xml::endElement;
				}

				template <typename Seq>
				static void writeRef(ModelWriterXML& wrt,
														 const Seq& seq,
														 const String& inName)
				{
					using boost::adaptors::transformed;
					using boost::algorithm::join;

					wrt.stream()
						<< xml::startElement(inName)
						<< join(seq |
						transformed([&wrt] (const ModelObject& e) {
						return wrt.idForExistingObject(e);
					})
								 , " ")
						<< xml::endElement;
				}
			};

      ModelWriterXML::ModelWriterXML(std::ostream& os)
        : mWriter(os)
      {
      }

      std::ostream& ModelWriterXML::stream()
      {
        return mWriter.stream();
      }

      void ModelWriterXML::write(std::ostream& os, const Utterance& inUtterance)
      {
        for (const auto& field : inUtterance.fieldNames()) {
          os
            << xml::startElement(tag::field)
            << xml::attribute(tag::name, field)
            << inUtterance.get(field)
            << xml::endElement
          ;
        }
      }

      void ModelWriterXML::write(std::ostream& os,
                                 const State::ranker_model_type& inRankerModel)
      {
        os << xml::record<CLASSIFIER_TAGS>(inRankerModel);
      }

      String ModelWriterXML::idForObject(const ModelObject& inObject)
      {
        auto i = mObject2ID.find(inObject);
        if (i != mObject2ID.end())
          return i->second;

        const int buffer_size = 10;
        char buffer[buffer_size];
        snprintf(buffer, buffer_size, "z%05lu", mObject2ID.size());
        String identifier(buffer);
        mObject2ID.emplace(inObject, identifier);
        return identifier;
      }

      String ModelWriterXML::idForExistingObject(const ModelObject& inObject)
      {
        auto i = mObject2ID.find(inObject);
        if (i != mObject2ID.end())
          return i->second;
        throw Error(
          "A fatal error while writing the model to xml. We should have seen the object.");
      }

      void ModelWriterXML::start(const ModelObject& inObject,
                                 const String& inElement)
      {
        stream()
          << xml::startElement(inElement)
          << xml::attribute(tag::id, idForObject(inObject))
        ;
      }

      void ModelWriterXML::start(const NamedModelObject& inObject,
                                 const String& inElement)
      {
        start((const ModelObject&)inObject, inElement);
        stream() << xml::attribute(tag::name, inObject.name());
      }

      void ModelWriterXML::write(const Utterance& utterance)
      {
        start(utterance, tag::utterance);
        stream()
          << xml::attribute(tag::count, utterance.count(), 1.0)
        ;

        write(stream(), utterance);

        stream() << xml::endElement;
      }

      void ModelWriterXML::write(const Link& link)
      {
        start(link, tag::link);

        stream()
          << xml::attribute(tag::value, (int)link.value())
          // the current value_type is 1 byte.
          // stream apis treat the value as a char unless we make an int.
          // if the value ever chages (float?!) we need to update this
          // statement.
        ;

        SeqWriter::writeRef(*this, link.utterances(), tag::utterances);

        stream() << xml::endElement;
      }

      void ModelWriterXML::write(const Domain& domain)
      {
        start(domain, tag::domain);
        SeqWriter::writeRef(*this, domain.utterances(), tag::utterances);
        stream() << xml::endElement;
      }

      void ModelWriterXML::write(const Lattice& lattice)
      {
        start(lattice, tag::lattice);
        SeqWriter::writeRef(*this, lattice.links(), tag::links);
        stream() << xml::endElement;
      }

      void ModelWriterXML::write(const State& state)
      {
        start(state, tag::state);

        auto classifier = state.rankerModel();

        stream()
          << xml::attribute(tag::questions,
          idForExistingObject(state.questions()))
          << xml::attribute(tag::answers,
          idForExistingObject(state.answers()))
          << xml::attribute(tag::links,
          idForExistingObject(state.links()))
        ;

        stream() << xml::startElement(tag::classifier);
				write(stream(), state.rankerModel());
        stream() << xml::endElement;

        stream()
          << xml::endElement
        ;
      }

      void ModelWriterXML::write(const Collection& inCollection)
      {
        stream()
          << xml::indent("  ")
          << xml::startElement(tag::database)
          << std::fixed << std::setprecision(1)
          << xml::attribute(tag::version, MODEL_XML_VERSION)
          << std::setprecision(12);
        ;

        SeqWriter::writeSeq(*this, inCollection.utterances(), tag::utterances);
        SeqWriter::writeSeq(*this, inCollection.links(), tag::links);
        SeqWriter::writeSeq(*this, inCollection.domains(), tag::domains);
        SeqWriter::writeSeq(*this, inCollection.lattices(), tag::lattices);
        SeqWriter::writeSeq(*this, inCollection.states(), tag::states);

        stream()
          << xml::startElement(tag::nextID)
          << mObject2ID.size()
          << xml::endElement
        ;

        stream() << xml::endElement;
      }

      OptionalError ModelWriterXML::writeCollection(std::ostream& os,
                                          const Collection& inCollection)
      {
        ModelWriterXML  wrt(os);

        try {
          wrt.write(inCollection);
          return Error::NO_ERROR;
        } catch (const Error& err) {
          return err;
        }
      }

    }
  }
}
