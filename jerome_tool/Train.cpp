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

#include <jerome/type/Factory.hpp>
#include <jerome/ir/report/HTMLReporter.hpp>
#include <jerome/ir/report/XMLReporter.hpp>

static const char* oInputFile   = "input";
static const char* oOutputFile	= "output";
static const char* oReportFile	= "report";
static const char* oTestSplit   = "test-split";
static const char* oDevSplit    = "dev-split";
static const char* oMaxTime     = "max-time";

using namespace jerome;
using namespace jerome::npc;

po::options_description Train::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  options.add_options()
  (oInputFile, 	po::value<std::string>()->default_value("-"),
   "input file (default: standard input)")
  (oOutputFile, po::value<std::string>(),
   "output file (default: none)")
  (oReportFile, po::value<std::string>()->default_value("-"),
   "report file")
  (oMaxTime, po::value<int>(),
   "maximum training time in seconds")
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
  ;
  
  return options;
}

OptionalError Train::run1Classifier(const std::string& classifierName)
{
  double bestValue = 0;
  
  TrainingParameters<Utterance> params;
  params.stateName = classifierName;
  
  int maxTime = 0;
  if (!variables()[oMaxTime].empty())
    maxTime = variables()[oMaxTime].as<int>();
  
  params.callback = [&bestValue,maxTime](TrainingState& state) {
    if (maxTime > 0 && state.elapsedTimeInSeconds() > maxTime)
      state.stop();
    if (state.lastValue() > bestValue) {
      bestValue = state.lastValue();
      //      std::cout << state.name() << ": " << state.lastArgument() << " " << bestValue << std::endl;
    }
  };
  
  auto optState = platform().collection().states().optionalObjectWithName(params.stateName);
  if (!optState) {
    return Error(std::string("Invalid classifier name: ") + classifierName);
  }
  
  typedef List<Utterance> UL;
  
  std::string testPropText = variables()[oTestSplit].as<std::string>();
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
  
  std::string devPropText = variables()[oDevSplit].as<std::string>();
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
  
  EvaluationParameters<Utterance> eparams;
  
  Record eargs(jerome::detail::FactoryConst::PROVIDER_IDENTIFIER_KEY,
              jerome::ir::evaluation::detail::HTMLReporterBase::IDENTIFIER);
  
  eparams.stateName = classifierName;
  eparams.testQuestions = testTrainSplit.first;
  eparams.trainingQuestions = testTrainSplit.second;
  eparams.report = Command::nullOStream();
  eparams.reporterModel = eargs;
  
  auto acc_or_error_before = platform().evaluate(eparams);
  if (!acc_or_error_before) return acc_or_error_before.error();
  
  auto error = platform().train(params);
  if (error) return error;

  auto acc_or_error_after = platform().evaluate(eparams);
  if (!acc_or_error_after) return acc_or_error_after.error();

  std::cout << acc_or_error_before.value() << " -> " << acc_or_error_after.value() << std::endl;
  
  return Error::NO_ERROR;
}

OptionalError Train::setup()
{
  return platform().loadCollection(*istreamWithName(variables()[oInputFile]));
}

OptionalError Train::teardown()
{
  return platform().saveCollection(*ostreamWithName(variables()[oOutputFile]));
}

std::string Train::description() const
{
  return "train a classifier";
}

std::string Train::name() const
{
  return "train";
}


