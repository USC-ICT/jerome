//
//  Evaluate.cpp
//  jerome
//
//  Created by Anton Leuski on 9/9/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <fstream>

#include "Evaluate.hpp"

#include <jerome/type/algorithm.hpp>
#include <jerome/type/Factory.hpp>
#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>
#include <jerome/ir/report/HTMLReporter.hpp>
#include <jerome/ir/report/XMLReporter.hpp>

static const char* oInputFile   = "input";
static const char* oReportFile	= "report";
static const char* oTestSplit   = "test-split";
static const char* oStateName   = "classifier-name";

using namespace jerome;
using namespace jerome::npc;

Evaluate::Evaluate()
: Command("evaluate", "Evaluate options")
{
  options().add_options()
  (oInputFile, 	po::value<std::string>()->default_value("-"), "input file (default standard input)")
  (oReportFile, po::value<std::string>()->default_value("-"), "report file")
  (oTestSplit,  po::value<std::string>()->default_value("label"),
   (std::string("How to select test questions. Provide one of \n")
    + "auto      \t\n"
    + "label     \t\n"
    + "<number>  \t\n"
    + "<number>% \t\n"
    )
   .c_str())
  (oStateName, 	po::value<std::vector<std::string>>()->composing(), "classifier names in the data file")
  ;
}

static void run1(Platform& p, const std::string& classifierName, const po::variables_map& vm)
{
  auto optState = p.collection().states().optionalObjectWithName(classifierName);
  if (!optState) {
    throw std::invalid_argument(classifierName);
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

  EvaluationParameters<Utterance> params;

  Record args(jerome::detail::FactoryConst::PROVIDER_IDENTIFIER_KEY,
              jerome::ir::evaluation::detail::XMLReporterBase::IDENTIFIER);
  
  params.stateName = classifierName;
  params.testQuestions = testTrainSplit.first;
  params.report = Command::ostreamWithName(vm[oReportFile].as<std::string>());
  params.reporterModel = args;
  
  auto acc_or_error = p.evaluate(params);
  
  if (!acc_or_error) {
    throw acc_or_error.error();
  }
  
  std::cout << acc_or_error.value() << std::endl;
}

void Evaluate::run(const po::variables_map& vm) {
  
  Platform::initialize();
		
  Platform	p;
  
  // =================================
  // loading a database
  
  {
    auto file = istreamWithName(vm[oInputFile].as<std::string>());
    auto error = p.loadCollection(*file);
    if (error) throw *error;
  }
  
  // =================================
  // evaluating a classifier
  
  if (vm.count(oStateName)) {
    for(auto cn : vm[oStateName].as<std::vector<std::string>>()) {
      run1(p, cn, vm);
    }
  } else {
    for(const auto& cn : p.collection().states()) {
      run1(p, cn.name(), vm);
    }
  }
  
  
}

std::string Evaluate::description() const
{
  return "evaluate a classifier";
}

