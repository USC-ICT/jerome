//
//  main.cpp
//  vhmsg_jerome
//
//  Created by Anton Leuski on 9/29/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "VHMSG.hpp"
#include "vrExpress.hpp"

#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace jerome;
using namespace jerome::npc;

static const char* oHelp    = "help";
static const char* oTrainData = "train-data";
static const char* oDevData   = "dev-data";
static const char* oTestData  = "test-data";
static const char* oDMScript  = "dm";
static const char* oStateName = "classifier-name";

int main(int argc, const char * argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
    (oHelp, "produce help message")
    (oTrainData,  po::value<std::string>(), "train data file")
    (oDevData,    po::value<std::string>(), "dev data file")
    (oTestData,   po::value<std::string>(), "test data file")
    (oStateName,  po::value<std::string>(), "classifier name in the data file")
    (oDMScript,   po::value<std::string>(),
    "dialogue manager scxml script file")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count(oHelp)) {
    std::cout << desc << "\n";
    return 1;
  }

  if (!vm.count(oTrainData)) {
    std::cerr << "train data file is required" << "\n";
    return 1;
  }

  if (!vm.count(oStateName)) {
    std::cerr << "classifier name is required" << "\n";
    return 1;
  }

  try {
    // =================================
    // initializing the library

    Platform::initialize();

    Platform  p;

    // =================================
    // loading a database

    {
      std::ifstream file(vm[oTrainData].as<std::string>());
      auto error = p.loadCollection(file);
      if (error) throw *error;
    }

    // load the dialogue manager script and find out the
    // state machine identifier

    String  mainStateMachineName;

    std::ifstream file(vm[oDMScript].as<std::string>());
    p.loadDialogueManager(file, [&] (const Result<String>& name) mutable {
      if (name) {
        mainStateMachineName = name.value();
      } else {
        std::cerr << name.error().what() << std::endl;
      }
    });

    std::mutex m;
    std::condition_variable cv;
    bool isDone = false;

		String sessionID = "s";
		std::size_t	messageCount = 0;
		
		std::unordered_map<String, StringStringMap>	sentMessage;

    // =================================
    // setup vhmsg connection

    ict::vhmsg::Client  vhmsg(ict::vhmsg::Model("jerome"));

    p.setEngineEventHandler([&] (const EngineEvent& event) {
      if (event.name() != "sendUtterance") return;
      StringStringMap data = event.data();
      if (data.find("utteranceID") == data.end()) return;
      String utteranceID = data["utteranceID"];

      auto optUtt = p.utteranceWithID(utteranceID);
      if (!optUtt) return;

      auto utt = *optUtt;
			
			char buffer[128];
			snprintf(buffer, sizeof(buffer), "%s%07lu", sessionID.c_str(), messageCount++);
			String messageID = String(buffer);

      vhmsg.send("vrExpress",
				utt.get("speaker", "all"),	// speaker
				"all", // addressee
				messageID,
        ict::vhmsg::vrExpress::compose(
          utt.get("speaker", "all"),
					utt.get("text", ""),
					utteranceID,
					""));
			
			sentMessage.emplace(messageID, data);
    });

    // =================================
    // handle vrKillComponent

    vhmsg.setListenerForEvent([&] (const String& id) mutable {
      if (id == vhmsg.model().componentID() || id == "all") {
        {
          std::lock_guard<std::mutex> lk(m);
          isDone = true;
        }
        cv.notify_one();
      }
    }, "vrKillComponent");

    // =================================
    // handle vrSpeech

    String lastUtterance;
    vhmsg.setListenerForEvent([&] (const String& symbol, int id, double prob,
                                   const String& quality, const String& text) {
      // store the last user utterance
      lastUtterance = text;
    }, "vrSpeech", "interp");

    vhmsg.setListenerForEvent([&] (const String& symbol) {
      // send the stored utterance to the dialogue manager
      p.postEvent("vrSpeech_asr_complete",
        { { String(Utterance::kFieldText), lastUtterance } },
        mainStateMachineName);
    }, "vrSpeech", "asr-complete");

    // =================================
    // handle vrAgentBML

    vhmsg.setListenerForEvent([&] (const String& agentID,
                                   const String& messageID,
                                   const String& command,
                                   const String& parameter) {

			if (command != "end") return;

			String parameterCopy = parameter;
			String param = parameterCopy.empty()
				? "complete" : ict::vhmsg::detail::popToken(parameterCopy);
			
			auto dataIter = sentMessage.find(messageID);
			if (dataIter == sentMessage.end()) return;
			
			// looking for "complete", "interrupted", or "error"
			auto eventNameIter = dataIter->second.find(param);
			if (eventNameIter == dataIter->second.end()) return;

			auto machineNameIter = dataIter->second.find("machineName");
			if (machineNameIter == dataIter->second.end()) return;
			
      p.postEvent(eventNameIter->second,
        { },
        machineNameIter->second);
			
    }, "vrAgentBML");

    // =================================
    // start vhmsg client and
    // wait here until someone kills us

    auto error = vhmsg.start();
    if (error) throw *error;

    {
      std::unique_lock<std::mutex> lk(m);
      cv.wait(lk, [&] { return isDone; });
    }

    vhmsg.stop();

  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}
