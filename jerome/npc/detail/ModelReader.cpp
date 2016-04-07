//
//  ModelReader.cpp
//
//  Created by Anton Leuski on 8/19/15.
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
#include <string>
#include <list>
#include <jerome/npc/model.hpp>
#include <jerome/xml/reader.hpp>
#include <jerome/xml/record_writer.hpp>
#include <jerome/npc/detail/ModelIO.hpp>

#include "ModelReader.hpp"

namespace jerome {
  namespace npc {

    namespace detail {

      class ModelReader;

      template <typename Obj>
      struct objectReader
      {
      };

      template <typename T>
      struct cache
      {
        StringMap<T> values;
      };

      template <>
      struct objectReader<Utterance>
        : public cache<Utterance>
      {
        static constexpr const std::size_t  index = 0;
      };

      template <>
      struct objectReader<Link>
        : public cache<Link>
      {
        static constexpr const std::size_t  index = 1;
      };

      template <>
      struct objectReader<Domain>
        : public cache<Domain>
      {
        static constexpr const std::size_t  index = 2;
      };

      template <>
      struct objectReader<Lattice>
        : public cache<Lattice>
      {
        static constexpr const std::size_t  index = 3;
      };

      template <>
      struct objectReader<State>
        : public cache<State>
      {
        static constexpr const std::size_t  index = 4;
      };

      class ModelReader
      {
      public:
        ModelReader(const ObjectFactory& inObjectFactory, std::istream& stream)
          : mReader(stream)
          , mObjectFactory(inObjectFactory)
        {
        }

        // we assume that there are no empty elements: for every start element
        // tag we get an end element tag.

        String requiredAttribute(const String& name)
        {
          OptionalString s = reader().attribute(name);
          if (s) return *s;
          throw Error("attribute " + name + " is required");
        }

        void assertStartElementAndAdvance(const String& name)
        {
          reader().assertNode(xml::reader::ELEMENT, name);
          reader().nextTag();
        }

        void assertEndElementAndAdvance(const String& name)
        {
          reader().assertNode(xml::reader::END_ELEMENT, name);
          reader().nextTag();
        }

        bool test(xml::reader::NodeType inType, const String& inName)
        {
          return reader().type() == inType
                 && reader().localName() == inName;
        }

        bool testStartElement(const String& inName)
        {
          return test(xml::reader::ELEMENT, inName);
        }

        template <typename Obj>
        Obj read();

        template <typename Obj, typename Seq>
        void readObjects(const String& name, Seq& seq)
        {
          if (!testStartElement(name))
            return;

          reader().nextTag();
          while (reader().type() == xml::reader::ELEMENT) {
            seq.insert(read<Obj>());
          }

          assertEndElementAndAdvance(name);
        }

        Result<Collection> readCollection()
        {
          Collection collection = of().makeNew<Collection>();

          try {
            reader().nextTag();
            reader().assertNode(xml::reader::ELEMENT, tag::database);
            double version = std::stod(requiredAttribute(tag::version));
            if (version != MODEL_XML_VERSION)
              throw Error("Cannot read format " + std::to_string(version)
                + ". Can only read version " + std::to_string(MODEL_XML_VERSION)
                + ".");
            reader().nextTag();

            readObjects<Utterance>(tag::utterances, collection.utterances());
            readObjects<Link>(tag::links, collection.links());
            readObjects<Domain>(tag::domains, collection.domains());
            readObjects<Lattice>(tag::lattices, collection.lattices());
            readObjects<State>(tag::states, collection.states());

            if (testStartElement(tag::nextID)) {
              reader().elementText();
              assertEndElementAndAdvance(tag::nextID);
            }

            reader().assertNode(xml::reader::END_ELEMENT, tag::database);

          } catch (const Error& ex) {
            std::cout << "|" << reader() << "|" << std::endl;
            // I try to avoid exceptions as much as possible,
            // but this is a localized case deep in the implementation.
            return Error("failed to parse the collection xml document", ex);
          }

          return collection;
        }

        xml::reader& reader()
        {
          return mReader;
        }

        const ObjectFactory& of()
        {
          return mObjectFactory;
        }

        template <typename T>
        objectReader<T>& cache()
        {
          return std::get<objectReader<T>::index>(mCaches);
        }

      private:
        xml::reader mReader;
        const ObjectFactory& mObjectFactory;

        typedef std::tuple<
          objectReader<Utterance>,
          objectReader<Link>,
          objectReader<Domain>,
          objectReader<Lattice>,
          objectReader<State>
          > map_type;

        map_type  mCaches;

        template <typename Obj, typename ... Args>
        Obj objectOfType(const String& inID, bool existsHint, Args&& ... args)
        {
          auto& objCache(cache<Obj>());
          auto p = objCache.values.find(inID);
          if (p == objCache.values.end()) {
            assert(!existsHint);
            p = objCache.values.emplace(inID,
              of().makeNew<Obj>(std::forward<Args>(args) ...)).first;
          }
          return p->second;
        }

        String readRefList(const String& tag, const String& listTag)
        {
          String refList;
          reader().nextTag();
          if (testStartElement(listTag)) {
            refList = reader().elementText();
            assertEndElementAndAdvance(listTag);
          }
          assertEndElementAndAdvance(tag);
          return refList;
        }

      };

      template <>
      Utterance ModelReader::read<Utterance>()
      {
        reader().assertNode(xml::reader::ELEMENT, tag::utterance);
        String identifier = requiredAttribute(tag::id);
        String count = reader().attribute(tag::count, "1.0");

        Utterance utterance = objectOfType<Utterance>(identifier, false);
        utterance.setCount(std::stod(count));

        reader().nextTag();

        while (testStartElement(tag::field)) {
          String name = requiredAttribute(tag::name);
          String value = reader().elementText();
          if (name == "externalID")
            name = Utterance::kFieldID;
          utterance.put(name, value);
          reader().nextTag();
        }
        assertEndElementAndAdvance(tag::utterance);

        return utterance;
      }

      template <>
      Link ModelReader::read<Link>()
      {
        reader().assertNode(xml::reader::ELEMENT, tag::link);
        String identifier = requiredAttribute(tag::id);
        String value = requiredAttribute(tag::value);

        String utteranceRefs = readRefList(tag::link, tag::utterances);

        Link link = objectOfType<Link>(identifier, false);

        link.setValue(std::stoi(value));
        String uid;
        for (std::stringstream s(utteranceRefs); s >> uid; )
          link.utterances().insert(objectOfType<Utterance>(uid, true));

        return link;
      }

      template <>
      Domain ModelReader::read<Domain>()
      {
        reader().assertNode(xml::reader::ELEMENT, tag::domain);
        String identifier = requiredAttribute(tag::id);
        String name = requiredAttribute(tag::name);

        String utteranceRefs = readRefList(tag::domain, tag::utterances);

        Domain domain = objectOfType<Domain>(identifier, false);
        domain.setName(name);
        String uid;
        for (std::stringstream s(utteranceRefs); s >> uid; ) {
          domain.utterances().insert(objectOfType<Utterance>(uid, true));
        }

        return domain;
      }

      template <>
      Lattice ModelReader::read<Lattice>()
      {
        reader().assertNode(xml::reader::ELEMENT, tag::lattice);
        String identifier = requiredAttribute(tag::id);
        String name = requiredAttribute(tag::name);
        String linkRefs = readRefList(tag::lattice, tag::links);;

        Lattice lattice = objectOfType<Lattice>(identifier, false);
        lattice.setName(name);
        String lid;
        for (std::stringstream s(linkRefs); s >> lid; ) {
          lattice.links().insert(objectOfType<Link>(lid, true));
        }

        return lattice;
      }

      template <>
      State ModelReader::read<State>()
      {
        reader().assertNode(xml::reader::ELEMENT, tag::state);
        String identifier = requiredAttribute(tag::id);
        String name = requiredAttribute(tag::name);
        String answersID = requiredAttribute(tag::answers);
        String questionsID = requiredAttribute(tag::questions);
        String linksID = requiredAttribute(tag::links);
        State::ranker_model_type classifier;

        reader().nextTag();

        if (testStartElement(tag::classifier)) {
          classifier = xml::readRecord<CLASSIFIER_TAGS, State::ranker_model_type>(reader());
          assertEndElementAndAdvance(tag::classifier);
        }
        assertEndElementAndAdvance(tag::state);

        State state = objectOfType<State>(identifier, false,
          objectOfType<Domain>(questionsID, true),
          objectOfType<Domain>(answersID, true),
          objectOfType<Lattice>(linksID, true));
        state.setName(name);
        state.setRankerModel(classifier);

        return state;
      }

    }

    Result<Collection>  readCollection(const ObjectFactory& inObjectFactory,
                                       std::istream& stream)
    {
      detail::ModelReader  rdr(inObjectFactory, stream);
      return rdr.readCollection();
    }

  }
}