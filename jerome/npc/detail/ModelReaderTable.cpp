//
//  ModelReaderTable.cpp
//
//  Created by Anton Leuski on 3/16/19.
//  Copyright © 2019 Anton Leuski & ICT/USC. All rights reserved.
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

#include <jerome/npc/model.hpp>
#include "ModelReaderTable.hpp"

/*
 Limitations:
 - does not support count
 - does not support domain inheritance.
    except for default domain. Utterances specified in the default domain
    (e.g., no domain name) will be placed in each domain including the default 
    one. Note, that this is done per category, questions go to question domains,
    answers go to answer domains.
 
    otherwise, if you want to put an utterances in multiple domains, 
    you have to specify that explicitly. But! You can do this:
  
    answer.id, answer.domain, answer.text
    an_id, domain0, Some text here
    an_id, domain1,
    an_id, domain2,
 
    no need to repeat fields if id is the same.
 */

namespace jerome {
  namespace npc {
    namespace detail {
      
      struct ModelReaderTableImpl {
        
        struct Header {
          bool isQuestion;
          String name;
        };
        
        struct Info {
          bool isQuestion;
          String defaultDomainName;
          optional<size_t> idIndex;
          optional<size_t> textIndex;
          optional<size_t> domainIndex;
          StringMap<Utterance> idCache;
          StringMap<Utterance> textCache;
          List<Domain> nonDefaultDomains;
          
          Info(bool inIsQuestion, const String& inName)
          : isQuestion(inIsQuestion)
          , defaultDomainName(inName)
          {}
          
          void assignIndex(size_t inIndex, const String& inName) {
            if (inName == Utterance::kFieldID) {
              idIndex = inIndex;
            }
            if (inName == Utterance::kFieldText) {
              textIndex = inIndex;
            }
            if (inName == "domain") {
              domainIndex = inIndex;
            }
          }
        };
        
        typedef std::pair<Domain, Utterance> DomainUtterance;
        
        struct Record {
          const std::vector<std::string>& mLine; 
          Record(const std::vector<std::string>& inLine)
          : mLine(inLine) {}
          
          optional<String> at(optional<size_t> inIndex) const {
            if (!inIndex 
                || inIndex.value() >= mLine.size())
            { return optional<String>(); }

            auto value = trim(mLine[inIndex.value()]);

            if (value.empty()) { return optional<String>(); }
            
            return value;
          }
        };

        struct RecordMultiline {
          const std::vector<std::vector<std::string>>& mLines; 
          const size_t mIndex;
          RecordMultiline(const std::vector<std::vector<std::string>>& inLines, 
                          size_t inIndex)
          : mLines(inLines)
          , mIndex(inIndex)
          {}
          
          optional<String> at(optional<size_t> inIndex) const {
            if (!inIndex 
                || inIndex.value() >= mLines.size())
            { return optional<String>(); }
            
            const auto& rows(mLines[inIndex.value()]);
            
            if (rows.empty()) { return optional<String>(); }

            auto rowIndex = std::min(mIndex, rows.size()-1);
            auto value = trim(rows[rowIndex]);

            if (value.empty()) { return optional<String>(); }

            return value;
          }
        };
        
        static constexpr const char* kHeaderQuestion = "question";

        ModelReaderTableImpl(const ObjectFactory& inObjectFactory)
        : mObjectFactory(inObjectFactory)
        , mCollection(of().makeNew<Collection>())
        , mQuestionInfo(true, "Anybody")
        , mAnswerInfo(false, "Anyone")
        {
        }
        
        const ObjectFactory& of()
        {
          return mObjectFactory;
        }
        
        Utterance 
        makeUtterance(Info& ioInfo, const optional<String>& inID, 
                      const optional<String>& inText)
        {
          if (inID) {
            auto utteranceIter = ioInfo.idCache.find(inID.value());
            if (utteranceIter != ioInfo.idCache.end()) {
              return utteranceIter->second;
            }
          }
         
          if (inText) {
            auto utteranceIter = ioInfo.textCache.find(inText.value());
            if (utteranceIter != ioInfo.textCache.end()) {
              return utteranceIter->second;
            }
          }
          
          Utterance utterance = of().makeNew<Utterance>();
          mCollection.utterances().insert(utterance);
          
          if (inID) {
            ioInfo.idCache[inID.value()] = utterance;
          }
          
          if (inText) {
            ioInfo.textCache[inText.value()] = utterance;
          }
          
          return utterance;
        } 
        
        template <typename Accessor>
        optional<DomainUtterance> 
        findUtterance(const Accessor& inAccessor, Info& ioInfo) {
          auto id = inAccessor.at(ioInfo.idIndex);
          auto text = inAccessor.at(ioInfo.textIndex);
          if (!id && !text) { return optional<DomainUtterance>(); }

          auto domainName = inAccessor.at(ioInfo.domainIndex);
          auto domain = findDomain(domainName 
                                   ? domainName.value() 
                                   : ioInfo.defaultDomainName, 
                                   ioInfo);

          Utterance utterance = makeUtterance(ioInfo, id, text);

          domain.utterances().insert(utterance);
          
          for(size_t i = 0; i < mHeaders.size(); ++i) {
            if (ioInfo.isQuestion != mHeaders[i].isQuestion) { continue; }
            auto value = inAccessor.at(i);
            if (!value) { continue; }
            auto header = mHeaders[i];
            if (header.name.empty()) { continue; }
            utterance.put(header.name, value.value());
          }
                    
          return DomainUtterance(domain, utterance);
        }
        
        Domain
        findDomain(const String& inName, Info& ioInfo) {
          auto optionalDomain = mCollection.domains()
            .optionalObjectWithName(inName);
          if (optionalDomain) { return optionalDomain.value(); }
          
          auto domain = of().makeNew<Domain>();
          domain.setName(inName);
          mCollection.domains().insert(domain);
          
          if (inName != ioInfo.defaultDomainName) {
            ioInfo.nonDefaultDomains.push_back(domain);
          }
          
          return domain;
        }
        
        Lattice
        findLattice(const Domain& inQuestions, const Domain& inAnswers) {
          auto name = std::string("Lattice-") 
            + inQuestions.name() + "-" 
            + inAnswers.name(); 
          auto optionalLattice = mCollection.lattices()
            .optionalObjectWithName(name);
          if (optionalLattice) { return optionalLattice.value(); }
          auto lattice = of().makeNew<Lattice>();
          lattice.setName(name);
          mCollection.lattices().insert(lattice);
          
          auto state = of().makeNew<State>(inQuestions, inAnswers, lattice);
          state.setName(inQuestions.name() + " - " + inAnswers.name());
          mCollection.states().insert(state);
          
          return lattice;
        }
        
        void processLine(const std::vector<std::string>& inLine) {
          if (mHeaders.empty()) {
            parseHeaders(inLine);
            return;
          }

          std::vector<std::vector<std::string>> lines; 
          size_t maxCount = 0;
          
          for(size_t i = 0, 
              n = std::min(inLine.size(), mHeaders.size()); i < n; ++i)
          {
            if (!mHeaders[i].isQuestion) {
              lines.push_back(std::vector<std::string>());
            } else {
              auto value = trim(inLine[i]);
              if (value.empty()) {
                lines.push_back(std::vector<std::string>());
              } else {
                std::vector<String> parts;
                boost::algorithm::split(parts, value, boost::is_any_of("\n"));
                maxCount = std::max(maxCount, parts.size());
                lines.push_back(parts);
              }
            }
          }          

          auto answerPair = findUtterance(Record(inLine), mAnswerInfo);
          for(size_t j = 0; j < maxCount; ++j) {
            auto questionPair = findUtterance(RecordMultiline(lines, j), 
                                              mQuestionInfo);
            if (!answerPair || !questionPair) { continue; }
            auto lattice = findLattice(answerPair.value().first, 
                                       questionPair.value().first);
            auto link = of().makeNew<Link>();
            link.setValue(6);
            link.utterances().insert(questionPair.value().second);
            link.utterances().insert(answerPair.value().second);
            lattice.links().insert(link);
          }          
        }

        void parseHeaders(const std::vector<std::string>& inLine) {
          for(const auto& field: inLine) {
            auto lowercased = to_lower(trim(field));
            std::vector<String> parts;
            boost::algorithm::split(parts, lowercased, boost::is_any_of("."));
            Header header;
            if (parts.size() == 1) {
              header.isQuestion = false;
              header.name = parts[0];
            } else {
              header.isQuestion = parts[0] == kHeaderQuestion;
              header.name = parts[1];
            }
            int currentIndex = (int)mHeaders.size();
            if (header.isQuestion) {
              mQuestionInfo.assignIndex(currentIndex, header.name);
            } else {
              mAnswerInfo.assignIndex(currentIndex, header.name);
            }
            mHeaders.push_back(header);
          }
        }

        void
        finalize(Info& ioInfo) {
          if (!ioInfo.nonDefaultDomains.empty()) {
            auto optionalDomain = mCollection.domains()
              .optionalObjectWithName(ioInfo.defaultDomainName);
            if (optionalDomain) { 
              for(const auto& utterance: optionalDomain.value().utterances()) {
                for(auto& domain: ioInfo.nonDefaultDomains) {
                  domain.utterances().insert(utterance);
                }
              }
            }
          }
        }
        
        Result<Collection> collection() {
          finalize(mQuestionInfo);
          finalize(mAnswerInfo);
          return mCollection;
        }

        const ObjectFactory& mObjectFactory;
        Collection mCollection;
        List<Header> mHeaders;
        Info mQuestionInfo;
        Info mAnswerInfo;
      };
      
      ModelReaderTable::ModelReaderTable(const ObjectFactory& inObjectFactory)
      : parent_type(std::make_shared<ModelReaderTableImpl>(inObjectFactory))
      {
      }

      void 
      ModelReaderTable::processLine(const std::vector<std::string>& inLine) {
        implementation().processLine(inLine);
      }
      
      Result<Collection>  
      ModelReaderTable::collection() {
        return implementation().collection();
      }
    }
  }
}
