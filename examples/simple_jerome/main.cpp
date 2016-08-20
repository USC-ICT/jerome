//
//  main.cpp
//  jerome
//
//  Created by Anton Leuski on 3/27/13.
//  Copyright (c) 2013 ICT. All rights reserved.
//

#include <fstream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace jerome;
using namespace jerome::npc;

static const char* oHelp		= "help";
static const char* oTrainData	= "train-data";
static const char* oDevData		= "dev-data";
static const char* oTestData	= "test-data";
static const char* oDMScript	= "dm";
static const char* oStateName	= "classifier-name";

int main(int argc, const char * argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
	(oHelp, "produce help message")
	(oTrainData, 	po::value<std::string>(), "train data file")
	(oDevData, 		po::value<std::string>(), "dev data file")
	(oTestData, 	po::value<std::string>(), "test data file")
	(oStateName, 	po::value<std::string>(), "classifier name in the data file")
	(oDMScript, 	po::value<std::string>(), "dialogue manager scxml script file")
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
	
	String machineName;
	
	try {
    // =================================
    // initializing the library

		Platform::initialize();
		
		Platform	p;

    // =================================
    // loading a database

		{
			std::ifstream file(vm[oTrainData].as<std::string>());
			auto error = p.loadCollection(file);
			if (error) throw *error;
		}

    // =================================
    // answering a question

		if (vm.count(oDMScript))
    {
      // if you have a dialogue manager scxml file
      
			std::ifstream file(vm[oDMScript].as<std::string>());
			p.loadDialogueManager(file, [p, &machineName](const Result<String>& name) mutable {
        if (name) {
          p.postEvent("vrSpeech_asr_complete", { { String(Utterance::kFieldText), "hello"} }, machineName = name.value());
        } else {
          std::cerr << name.error().what() << std::endl;
        }
      });
			
			sleep(2);
						
		} else {
      
      // if you just care for a single answer

      if (!vm.count(oStateName)) {
        std::cerr << "classifier name is required" << "\n";
        return 1;
      }

      auto classifierName = vm[oStateName].as<std::string>();
      
      auto response = p.respond(classifierName, "hello");
      if (response) {
        jerome::npc::detail::ModelWriterText::write(std::cout, *response);
        std::cout << std::endl;
      } else {
        std::cout << "no response" << std::endl;
      }
      
		}

		
    // =================================
    // training a classifier

    if (!vm.count(oStateName)) {
      auto classifierName = vm[oStateName].as<std::string>();
      double bestValue = 0;
      auto error = p.train(classifierName, [&bestValue](TrainingState& state) {
        if (state.elapsedTimeInSeconds() > 5)
          state.stop();
        if (state.lastValue() > bestValue) {
          bestValue = state.lastValue();
          std::cout << state.name() << ": " << state.lastArgument() << " " << bestValue << std::endl;
        }
      });
      if (error) throw *error;
    }
		
    // =================================
    // saving the database

		{
			std::ofstream file("test.output.csxml");
			auto error = p.saveCollection(file);
			if (error) throw *error;
		}

			//		delete p1;

			//		sleep(10);
		
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}
	
	
	
	
	return 0;
}
