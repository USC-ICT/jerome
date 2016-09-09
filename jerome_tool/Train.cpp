//
//  Train.cpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>
#include "Train.hpp"

#include <jerome/type/algorithm.hpp>
#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>

static const char* oInputFile   = "input";
static const char* oOutputFile	= "output";
static const char* oReportFile	= "report";
static const char* oTestSplit   = "test-split";
static const char* oDevSplit    = "dev-split";
static const char* oStateName   = "classifier-name";

using namespace jerome;
using namespace jerome::npc;

Train::Train()
: Command("train")
, mOptions("Options")
{
  mOptions.add_options()
  (oInputFile, 	po::value<std::string>()->default_value("-"), "input file (default standard input)")
  (oOutputFile, po::value<std::string>()->default_value("-"), "output file (default standard output)")
  (oReportFile, po::value<std::string>()->default_value("-"), "report file")
  (oTestSplit,  po::value<std::string>()->default_value("label"),
   (std::string("How to select test questions. Provide one of \n")
    + "auto      \t\n"
    + "label     \t\n"
    + "<number>  \t\n"
    + "<number>% \t\n"
    )
   .c_str())
  (oDevSplit,   po::value<std::string>()->default_value("label"),
   (std::string("How to select development questions. Provide one of \n")
    + "auto      \t\n"
    + "label     \t\n"
    + "<number>  \t\n"
    + "<number>% \t\n"
    )
   .c_str())
  (oStateName, 	po::value<std::vector<std::string>>()->composing(), "classifier names in the data file")
  ;
}

static void run1(Platform& p, std::string& classifierName, const po::variables_map& vm)
{
  double bestValue = 0;
  
  TrainingParameters<Utterance> params;
  params.stateName = classifierName;
  params.callback = [&bestValue](TrainingState& state) {
    if (state.elapsedTimeInSeconds() > 5)
      state.stop();
    if (state.lastValue() > bestValue) {
      bestValue = state.lastValue();
      std::cout << state.name() << ": " << state.lastArgument() << " " << bestValue << std::endl;
    }
  };
  
  auto optState = p.collection().states().optionalObjectWithName(params.stateName);
  if (!optState) {
    throw std::invalid_argument(params.stateName);
  }
  
  typedef List<Utterance> UL;
  
  std::string testPropText = vm[oTestSplit].as<std::string>();
  std::pair<UL, UL>   testTrainSplit;
  std::pair<UL, UL>   devTrainSplit;
  
  if (testPropText == "label") {
    testTrainSplit = jerome::split<UL, const Domain::utterances_type&>
    (optState->questions().utterances(),
     [](const Utterance& u) {
       return u.get("train_test", "train") == "test";
     });
  } else {
    double testProp = 0.1;
    if (testPropText == "auto") {
    } else if (hasSuffix(testPropText, "%")) {
      testProp = std::atof(testPropText.c_str()) / 100.0;
    } else {
      testProp = std::atof(testPropText.c_str());
    }
    testTrainSplit = jerome::split<UL, const Domain::utterances_type&>
    (optState->questions().utterances(), testProp);
  }
  
  std::string devPropText = vm[oDevSplit].as<std::string>();
  if (devPropText == "label") {
    devTrainSplit = jerome::split<UL>
    (testTrainSplit.second,
     [](const Utterance& u) {
       return u.get("train_test", "train") == "dev";
     });
  } else {
    double devProp = 0.25;
    if (devPropText == "auto") {
    } else if (hasSuffix(devPropText, "%")) {
      devProp = std::atof(devPropText.c_str()) / 100.0;
    } else {
      devProp = std::atof(devPropText.c_str());
    }
    devTrainSplit = jerome::split<UL>(testTrainSplit.second, devProp);
  }
  
  params.developmentQuestions = devTrainSplit.first;
  params.trainingQuestions = devTrainSplit.second;
  params.trainer = jerome::npc::detail::Trainer::trainerFscore();
  
  auto error = p.train(params);
  if (error) throw *error;
  
  
}

void Train::run(const std::vector<std::string>& args, po::variables_map& vm) {
  
  Platform::initialize();
		
  Platform	p;

  // =================================
  // loading a database

  {
    std::string fileName = vm[oInputFile].as<std::string>();
    if (fileName == "-") {
      auto error = p.loadCollection(std::cin);
      if (error) throw *error;
    } else {
      std::ifstream file(vm[oInputFile].as<std::string>());
      auto error = p.loadCollection(file);
      if (error) throw *error;
    }
  }
  
  // =================================
  // training a classifier
  
  if (!vm.count(oStateName)) {
    for(auto cn : vm[oStateName].as<std::vector<std::string>>()) {
      run1(p, cn, vm);
    }
  }

  // =================================
  // saving the database
  
  {
    std::string fileName = vm[oOutputFile].as<std::string>();
    if (fileName == "-") {
      auto error = p.saveCollection(std::cout);
      if (error) throw *error;
    } else {
      std::ofstream file(fileName);
      auto error = p.saveCollection(file);
      if (error) throw *error;
		}
  }

}

std::string Train::description() const
{
  return "train a classifier";
}

void Train::manual(std::ostream& out) const
{
  out << description() << std::endl
  << "usage: " << Command::executable()
  << " " << name() << " [<options>]" << std::endl;
  out << mOptions << std::endl;
}

