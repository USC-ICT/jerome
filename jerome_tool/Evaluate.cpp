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
static const char* oReportFormat= "report-format";
static const char* oTestSplit   = "test-split";

using namespace jerome;
using namespace jerome::npc;

po::options_description Evaluate::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  options.add_options()
  (oInputFile, 	po::value<std::string>()->default_value("-"),
   "input file (default standard input)")
  (oReportFile, po::value<std::string>(),
   "report file name format string (default: none), e.g., \"report-%s.xml\". "\
   "The file will be named by replacing the first argument in the format " \
   "with the classifier name." )
  (oReportFormat, po::value<std::string>()->default_value("html"),
   "report file format. One of xml or html.")
  (oTestSplit,  po::value<std::string>()->default_value("label"),
   (std::string("How to select test questions. Provide one of \n")
    + "auto      \t\n"
    + "label     \t\n"
    + "<number>  \t\n"
    + "<number>% \t\n"
    )
   .c_str())
  ;
  
  return options;
}

OptionalError Evaluate::setup()
{
  return platform().loadCollection(*istreamWithName(variables()[oInputFile]));
}

OptionalError Evaluate::teardown()
{
  return Error::NO_ERROR;
}

OptionalError Evaluate::run1Classifier(const std::string& classifierName)
{
  auto optState = platform().collection().states()
    .optionalObjectWithName(classifierName);

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
  
  EvaluationParameters<Utterance> params;
  
  std::string reportFormatID =
  jerome::ir::evaluation::detail::HTMLReporterBase::IDENTIFIER;
  
  if (variables().count(oReportFormat)) {
    std::string format = variables()[oReportFormat].as<std::string>();
    if (format == "xml") {
      reportFormatID =
      jerome::ir::evaluation::detail::XMLReporterBase::IDENTIFIER;
    } else if (format == "html") {
      
    } else {
      return Error("Unknown report format: \"" + format + "\"");
    }
  }
  
  Record args(jerome::detail::FactoryConst::PROVIDER_IDENTIFIER_KEY,
              reportFormatID);
  
  params.stateName = classifierName;
  params.testQuestions = testTrainSplit.first;
  params.trainingQuestions = testTrainSplit.second;
  if (variables()[oReportFile].empty()) {
    params.report = Command::nullOStream();
  } else {
    params.report = Command::ostreamWithName(string_format(variables()[oReportFile].as<std::string>(),
                                                           variables()[oInputFile].as<std::string>().c_str(),
                                                           classifierName.c_str()));
  }
  params.reporterModel = args;
  
  auto acc_or_error = platform().evaluate(params);
  
  if (!acc_or_error) {
    return acc_or_error.error();
  }
  
  std::cout << acc_or_error.value() << std::endl;
  
  return Error::NO_ERROR;
}

std::string Evaluate::description() const
{
  return "evaluate a classifier";
}

std::string Evaluate::name() const
{
  return "evaluate";
}

