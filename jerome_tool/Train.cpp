//
//  Train.cpp
//  jerome
//
//  Created by Anton Leuski on 9/5/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

#include <signal.h>

#include <fstream>
#include "Train.hpp"

#include <boost/algorithm/string/join.hpp>

#include <jerome/type/algorithm.hpp>
#include <jerome/npc/npc.hpp>
#include <jerome/npc/detail/ModelReader.hpp>
#include <jerome/npc/detail/ModelWriterText.hpp>
#include <jerome/npc/factories/AnalyzerFactory.hpp>
#include <jerome/npc/factories/TrainerFactory.hpp>
#include <jerome/npc/factories/RankerFactory.hpp>
#include <jerome/npc/factories/WeightingFactory.hpp>

#include <jerome/type/Factory.hpp>
#include <jerome/ir/report/HTMLReporter.hpp>
#include <jerome/ir/report/XMLReporter.hpp>

static const char* oOutputFile          = "output";
static const char* oReportFile          = "report";
static const char* oReportFormat        = "report-format";
static const char* oTestSplit           = "test-split";
static const char* oDevSplit            = "dev-split";
static const char* oMaxTime             = "max-time";
static const char* oQuestionAnalyzer    = "question-analyzer";
static const char* oAnswerAnalyzer      = "answer-analyzer";
static const char* oQuestionWeighting   = "question-weighting";
static const char* oAnswerWeighting     = "answer-weighting";
static const char* oTrainer             = "trainer";
static const char* oVerbosity           = "verbosity";
static const char* oHackThreshold       = "hack-threshold";

#include "split.private.hpp"

using namespace jerome;
using namespace jerome::npc;

typedef TrainerFactory::object_type TR;

template <typename F>
static std::pair<std::string, std::string>
modelNames()
{
  auto models = F::sharedInstance().predefinedModels();
  auto modelNames = keys(models);
  auto modelNamesString = boost::algorithm::join(modelNames, "\n  ");
  auto defaultModelName = F::sharedInstance().defaultModelKey();
  return std::make_pair(modelNamesString, defaultModelName);
}

static SplitMenu<const Domain::utterances_type&> testTrainSplitActions("test", 0.1);
static SplitMenu<const UL&> devTrainSplitActions("dev", 0.15);

po::options_description Train::options(po::options_description inOptions) const
{
  po::options_description options(parent_type::options(inOptions));
  
  auto analyzerModels = modelNames<AnalyzerFactory>();
  auto qwModels = modelNames<QuestionWeightingFactory>();
  auto awModels = modelNames<AnswerWeightingFactory>();
  auto trainerModels = modelNames<TrainerFactory>();

  appendInputOptions(options);
  
  const char* IDENT = "\n  ";
  const char* CHOICE = "\nProvide one of";
  
  options.add_options()
  (oVerbosity, 	po::value<int>()->default_value(int(0)),
   "verbosity level")
  (oOutputFile, po::value<std::string>(),
   "output file (default: none)")
  (oReportFile, po::value<std::string>(),
   "report file name format string (default: none), e.g., \"%s-%s-%s.xml\". "\
   "The file will be named by replacing the first argument in the format " \
   "with with input file name, the second with the classifier name, and" \
   "the third is a suffix: one of \"before\" and \"after\".")
  (oReportFormat, po::value<std::string>()->default_value("html"),
   "report file format. One of xml or html.")
  (oMaxTime, po::value<std::string>(),
   "maximum training time. Can be specified as <number><unit>, where unit is "\
   "one of 's' (seconds), 'm' (minutes), or 'h' (hours). If the unit is "\
   "omitted, we assume seconds.")
  (oTestSplit,  po::value<std::string>()->default_value("label"),
   (std::string("How to select test questions.")
    + CHOICE
    + testTrainSplitActions.description() + "\n"
    )
   .c_str())
  (oDevSplit,   po::value<std::string>()->default_value("label"),
   (std::string("How to select development questions.")
    + CHOICE
    + devTrainSplitActions.description() + "\n"
    )
   .c_str())
  (oHackThreshold, po::value<bool>()->default_value(false),
   "If you do not have enough training data to extract and use a "\
   "meaningful dev and test sets, you can train and test on the "\
   "whole dataset by specifying 'all' as the question selection "\
   "options. However, in that case the final classifier will be "\
   "overtrained. It will generalize poorly -- it will be very "\
   "sensitive to the question text. To combat this, we can make "\
   "the classifier less sensitive by decreasing the threshold "\
   "by hand. It's very hack-ish, but necessary during initial "\
   "data collection. Set this flag to true if you want to "\
   "make the classifier less sensitive."
   )
  (oQuestionAnalyzer, po::value<std::string>()
    ->default_value(analyzerModels.second),
   (std::string("question analyzer.")
    + CHOICE + IDENT + analyzerModels.first).c_str())
  (oAnswerAnalyzer, po::value<std::string>()
    ->default_value(analyzerModels.second),
   (std::string("answer analyzer.")
    + CHOICE + IDENT + analyzerModels.first).c_str())
  (oQuestionWeighting, po::value<std::string>()
   ->default_value(qwModels.second),
   (std::string("question weighting.")
    + CHOICE + IDENT + qwModels.first).c_str())
  (oAnswerWeighting, po::value<std::string>()
   ->default_value(awModels.second),
   (std::string("answer weighting.")
    + CHOICE + IDENT + awModels.first).c_str())
  (oTrainer, po::value<std::string>()
   ->default_value(trainerModels.second),
   (std::string("training measure.")
    + CHOICE + IDENT + trainerModels.first).c_str())
  (TR::G_ALGORITHM, po::value<std::string>()
   ->default_value("GN_DIRECT_L_RAND"),
   (std::string("global optimization algorithm.")
    + CHOICE + IDENT 
    + boost::algorithm::join(TR::algorithms(TR::AlgorithmKind::ALL), IDENT)
    ).c_str())
  (TR::L_ALGORITHM, po::value<std::string>(),
   (std::string("some global optimization algorithms, i.e., ")
    + boost::algorithm::join(TR::algorithms(TR::AlgorithmKind::GLOBAL_REQUIRING_LOCAL), ", ")
    + " require a local optimizer. Try LN_BOBYQA first."
    + CHOICE + IDENT 
    + boost::algorithm::join(TR::algorithms(TR::AlgorithmKind::LOCAL), IDENT)
    ).c_str())
  ;
  
  po::options_description stopping("Stopping criteria");
  stopping.add_options()
  (TR::G_FTOL_REL, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "the objective function value by less than this value multiplied by the "\
   "absolute value of the function value. (If there is any chance that your "\
   "optimum function value is close to zero, you might want to set an "\
   "absolute tolerance as well.) As a first take, set this value to something "\
   "like 0.0001.")
  (TR::G_FTOL_ABS, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "the function value by less than this value.")
  (TR::G_XTOL_REL, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "every parameter by less than this value multiplied by the absolute value "\
   "of the parameter. (If there is any chance that an optimal parameter is "\
   "close to zero, you might want to set an absolute tolerance as well.)")
  (TR::G_XTOL_ABS, po::value<double>(),
   "stop when an optimization step (or an estimate of the optimum) changes "\
   "every parameter value by less than this value.")
  (TR::L_FTOL_REL, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes the objective function value by less than this value multiplied "\
   "by the absolute value of the function value. (If there is any chance that "\
   "your optimum function value is close to zero, you might want to set an "\
   "absolute tolerance as well.)")
  (TR::L_FTOL_ABS, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes the function value by less than this value.")
  (TR::L_XTOL_REL, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes every parameter by less than this value multiplied by the "\
   "absolute value of the parameter. (If there is any chance that an optimal "\
   "parameter is close to zero, you might want to set an absolute tolerance "\
   "as well.)")
  (TR::L_XTOL_ABS, po::value<double>(),
   "stop when a local optimization step (or an estimate of the optimum) "\
   "changes every parameter value by less than this value.")
  ;
  
  options.add(stopping);
  
  return options;
}

// ----------------------------------------------------------------------------


template <typename T>
static void recordEmplace(Record& ioRecord,
                          const String& key, const po::variables_map& vm)
{
  if (!vm[key].empty())
    ioRecord.emplace(key, vm[key].as<T>());
}

static Result<TrainerFactory::object_type>
makeTrainer(const po::variables_map& inVM)
{
  auto trainerMeasure = inVM[oTrainer].as<std::string>();
  auto trainerModel = TrainerFactory::sharedInstance()
  .modelAt(trainerMeasure);
  if (!trainerModel) {
    return Error("No trainer measure with id " + trainerMeasure);
  }
  
  auto model = *trainerModel;
  
  recordEmplace<String>(model, TR::G_ALGORITHM, inVM);
  recordEmplace<String>(model, TR::L_ALGORITHM, inVM);
  recordEmplace<double>(model, TR::G_FTOL_REL, inVM);
  recordEmplace<double>(model, TR::G_FTOL_ABS, inVM);
  recordEmplace<double>(model, TR::G_XTOL_REL, inVM);
  recordEmplace<double>(model, TR::G_XTOL_ABS, inVM);
  recordEmplace<double>(model, TR::L_FTOL_REL, inVM);
  recordEmplace<double>(model, TR::L_FTOL_ABS, inVM);
  recordEmplace<double>(model, TR::L_XTOL_REL, inVM);
  recordEmplace<double>(model, TR::L_XTOL_ABS, inVM);

  return TrainerFactory::sharedInstance().make(model);
}

#define COPY_RECORD(F, K, A) \
{ \
  auto key = inVM[K].as<std::string>(); \
  auto optional_v = F::sharedInstance().modelAt(key); \
  if (!optional_v) { \
    return Error(String("Unknown model ") + key + " " + K); \
  } \
  model.emplace(UtteranceCLRankerModel::A, *optional_v);\
}

static
Result<Record> makeRankerModel(const po::variables_map& inVM)
{
  Record model = Record {
    RankerFactory::PROVIDER_KEY, UtteranceCLRankerModel::IDENTIFIER };

  COPY_RECORD(AnswerWeightingFactory, oAnswerWeighting, ANSWER_WEIGHTING_KEY);
  COPY_RECORD(QuestionWeightingFactory, oQuestionWeighting, QUESTION_WEIGHTING_KEY);
  COPY_RECORD(AnalyzerFactory, oAnswerAnalyzer, ANSWER_ANALYZER_KEY);
  COPY_RECORD(AnalyzerFactory, oQuestionAnalyzer, QUESTION_ANALYZER_KEY);
  
  return model;
}

static int
parseTime(const std::string& inTime)
{
  char* end;
  double value = std::strtod(inTime.c_str(), &end);
  switch (*end) {
    case 'h': return (int)(value * 60 * 60);
    case 'm': return (int)(value * 60);
    default: return (int)value;
  }
}

// ----------------------------------------------------------------------------

static bool sCaughtSIGINT = false;
static void my_SIGINT_handler(int s)
{
  sCaughtSIGINT = true;
}

static void setup_SIGINT_handler()
{
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_SIGINT_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
}

// ----------------------------------------------------------------------------

struct Verbosity {
  Verbosity(int inValue)
  : value(inValue)
  {}
  
  bool isPrintingTestTrainSizes() const { return value > 0; }
  bool isPrintingFinalScore() const { return value > 0; }
  bool isPrintingInitialScore() const { return value > 1; }
  bool isPrintingProgress() const { return value > 2; }
  bool isPrintingFunctionValues() const { return value > 3; }
private:
  int value;
};

OptionalError Train::run1Classifier(const std::string& classifierName)
{
  auto optState = platform().collection().states()
  .optionalObjectWithName(classifierName);
  if (!optState) {
    return Error(std::string("Invalid classifier name: ") + classifierName);
  }
  
  auto   testTrainSplit = testTrainSplitActions
    .split(optState->questions().utterances(),
           variables()[oTestSplit].as<String>());
  if (!testTrainSplit) {
    return testTrainSplit.error();
  }
  
  auto   devTrainSplit = devTrainSplitActions
    .split(testTrainSplit.value().second,
           variables()[oDevSplit].as<String>());
  if (!devTrainSplit) {
    return devTrainSplit.error();
  }
  
  auto trainer_or_error = makeTrainer(variables());
  if (!trainer_or_error) {
    return trainer_or_error.error();
  }

  auto ranker_or_error = makeRankerModel(variables());
  if (!ranker_or_error) {
    return ranker_or_error.error();
  }

  auto format_or_error = parseFormat(variables());
  if (!format_or_error) return format_or_error.error();

  int maxTime = 0;
  if (!variables()[oMaxTime].empty())
    maxTime = parseTime(variables()[oMaxTime].as<std::string>());
  
  double bestValue = 0;
  bool newline = true;
  Verbosity verbosity(variables()[oVerbosity].as<int>());
  
  TrainingParameters<Utterance> params;

  setup_SIGINT_handler();
  
  if (verbosity.isPrintingTestTrainSizes()) {
    std::cout 
      << "question count: " << optState->questions().utterances().size() << std::endl 
      << "  answer count: " << optState->answers().utterances().size() << std::endl 
      << "    link count: " << optState->links().links().size() << std::endl 
      << "train set size: " << devTrainSplit.value().second.size() << std::endl 
      << "  dev set size: " << devTrainSplit.value().first.size() << std::endl 
      << " test set size: " << testTrainSplit.value().first.size() << std::endl 
      << std::flush;
  }
  
  params.stateName = classifierName;
  params.callback = [&](TrainingState& state) {
    if (sCaughtSIGINT) {
      if (verbosity.isPrintingFunctionValues()) {
        std::cout << std::endl << "Caught SIGINT. Exiting... " << std::flush;
      }
      state.stop();
      return;
    }

    if (maxTime > 0 && state.elapsedTimeInSeconds() > maxTime) {
      if (verbosity.isPrintingFunctionValues()) {
        std::cout << std::endl << "Timer expired. Exiting... " << std::flush;
      }
      state.stop();
      return;
    }
    
    if (verbosity.isPrintingFunctionValues()) {

      if (newline) {
        std::cout << std::endl;
        newline = false;
      } else {
        std::cout << "\r";
      }

      std::cout
      << state.name() << ": "
      << std::setw(10) << std::setprecision(7) << std::fixed << state.lastValue()
      << " "
      << state.lastArgument()
      << std::flush;
      
      if (state.lastValue() > bestValue) {
        bestValue = state.lastValue();
        newline = true;
      }

    } else if (verbosity.isPrintingProgress()) {
      std::cout << "." << std::flush;
    }
  };
  params.developmentQuestions = devTrainSplit.value().first;
  params.trainingQuestions = devTrainSplit.value().second;
  params.trainer = trainer_or_error.value();
  params.rankerModel = ranker_or_error.value();
  
  EvaluationParameters<Utterance> eparams;
  
  Record eargs(jerome::detail::FactoryConst::PROVIDER_KEY,
              format_or_error.value());
  
  eparams.stateName = classifierName;
  eparams.testQuestions = testTrainSplit.value().first;
  eparams.trainingQuestions = testTrainSplit.value().second;
  eparams.report = parseReportStream(classifierName, variables(), 
                                     inputFileName(variables()), "before");
  eparams.reporterModel = eargs;
  
  if (verbosity.isPrintingInitialScore()) {
    auto acc_or_error_before = platform().evaluate(eparams);
    if (!acc_or_error_before) return acc_or_error_before.error();
    std::cout << acc_or_error_before.value() << std::flush;
  }
  
  auto error = platform().train(params);
  if (error) return error;

  eparams.report = parseReportStream(classifierName, variables(), 
                                     inputFileName(variables()), "after");
  auto acc_or_error_after = platform().evaluate(eparams);
  if (!acc_or_error_after) return acc_or_error_after.error();

  if (verbosity.isPrintingFinalScore()) {
    if (verbosity.isPrintingFunctionValues()) {
      std::cout << std::endl;
    } else if (verbosity.isPrintingInitialScore()
               || verbosity.isPrintingProgress()) {
      std::cout << "\t";
    }
    std::cout << acc_or_error_after.value() << std::endl;
  }

  return Error::NO_ERROR;
}

OptionalError Train::setup()
{
  return loadCollection();
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


