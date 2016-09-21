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
#include "split.private.hpp"

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

static Result<String> parseFormat(const po::variables_map& inVM)
{
  std::string format = inVM[oReportFormat].as<std::string>();
  if (format == "xml") {
    return String(jerome::ir::evaluation::detail::XMLReporterBase::IDENTIFIER);
  } else if (format == "html") {
    return String(jerome::ir::evaluation::detail::HTMLReporterBase::IDENTIFIER);
  } else {
    return Error("Unknown report format: \"" + format + "\"");
  }
}

static ostream_ptr parseReportStream(const std::string& classifierName,
                                     const po::variables_map& inVM)
{
  if (inVM[oReportFile].empty()) {
    return Command::nullOStream();
  } else {
    auto name = string_format(inVM[oReportFile].as<std::string>(),
                              inVM[oInputFile].as<std::string>().c_str(),
                              classifierName.c_str());
    return Command::ostreamWithName(name);
  }
}

OptionalError Evaluate::run1Classifier(const std::string& classifierName)
{
  auto optState = platform().collection().states()
    .optionalObjectWithName(classifierName);

  if (!optState) {
    return Error(std::string("Invalid classifier name: ") + classifierName);
  }

  auto format_or_error = parseFormat(variables());
  if (!format_or_error) return format_or_error.error();
  
  std::pair<UL, UL>   testTrainSplit =
  splitData<const Domain::utterances_type&>(variables(), oTestSplit,
                                            optState->questions().utterances(),
                                            0.1, "test");
  
  Record args(jerome::detail::FactoryConst::PROVIDER_KEY,
              format_or_error.value());

  EvaluationParameters<Utterance> params;
  
  params.stateName = classifierName;
  params.testQuestions = testTrainSplit.first;
  params.trainingQuestions = testTrainSplit.second;
  params.report = parseReportStream(classifierName, variables());
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

