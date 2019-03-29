//
//  split.private.hpp
//  jerome
//
//  Created by Anton Leuski on 9/20/16.
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

#ifndef split_private_h
#define split_private_h

typedef List<Utterance> UL;

struct SplitActionBase {
  virtual ~SplitActionBase() {}
  virtual String description() const = 0;
};

template <typename T>
struct SplitAction: public SplitActionBase {
  virtual optional<std::pair<UL, UL>> 
  split(const T& inRange, const String& value) const = 0;
};

template <typename T>
struct SplitActionLabel: public SplitAction<T> {
  static constexpr const char* FIELD_NAME = "train_test";
  static constexpr const char* TRIGGER = "label";

  String mLabel;
  SplitActionLabel(const String& inLabel) : mLabel(inLabel) {}  
  String description() const override {
    return String(TRIGGER) + "     \tuse the value '" + mLabel
      + "' of the '" 
      + FIELD_NAME + "' field as the selection criterion.";
  }
  optional<std::pair<UL, UL>> 
  split(const T& inRange, const String& value) const override 
  {
    if (value != TRIGGER) { return optional<std::pair<UL, UL>>(); }
    return jerome::split<UL, T>(inRange,
                                [this](const Utterance& u) {
                                  return u.get(FIELD_NAME, "train") == 
                                    this->mLabel;
                                });
  }
};

template <typename T>
struct SplitActionProportion: public SplitAction<T> {
  optional<std::pair<UL, UL>> 
  split(const T& inRange, const String& inValue) const override 
  {
    auto value = proportion(inValue);
    if (!value) { return optional<std::pair<UL, UL>>(); }
    return jerome::split<UL, T> (inRange, value.value());
  }
  virtual optional<double> proportion(const String& value) const = 0;
};

template <typename T>
struct SplitActionAuto: public SplitActionProportion<T> {
  static constexpr const char* TRIGGER = "auto";
  double mProportion;
  SplitActionAuto(double inProportion) : mProportion(inProportion) {}  
  String description() const override {
    String prop = mProportion < 0 
      ? "100%"
      : mProportion < 1 
      ? std::to_string((int)round(mProportion * 100)) + "%"
      : std::to_string((int)round(mProportion));
    return String(TRIGGER) 
    + "      \tuse the default value. Currently set at '" 
    + prop + "'.";
  }
  optional<double> proportion(const String& value) const override 
  {
    if (value != TRIGGER) { return optional<double>(); }
    return mProportion;
  }
};

template <typename T>
struct SplitActionAll: public SplitActionProportion<T> {
  static constexpr const char* TRIGGER = "all";
  String description() const override {
    return String(TRIGGER) + "       \tuse all values.";
  }
  optional<double> proportion(const String& value) const override 
  {
    if (value != TRIGGER) { return optional<double>(); }
    return -1;
  }
};

template <typename T>
struct SplitActionPercent: public SplitActionProportion<T> {
  static constexpr const char* TRIGGER = "<number>%";
  String description() const override {
    return String(TRIGGER) + " \tuse a proportion of total questions.";
  }
  optional<double> proportion(const String& value) const override 
  {
    if (!hasSuffix(value, "%")) { return optional<double>(); }
    auto testProp = std::atof(value.c_str()) / 100.0;
    // special case. 100%. Take all. Or, do not split.
    return (testProp == 1) ? -1 : testProp;
  }
};

template <typename T>
struct SplitActionCount: public SplitActionProportion<T> {
  static constexpr const char* TRIGGER = "<number>";
  String description() const override {
    return String(TRIGGER) + "  \tuse the given number of questions.";
  }
  optional<double> proportion(const String& value) const override 
  {
    if (hasSuffix(value, "%")) { return optional<double>(); }
    return std::atof(value.c_str());
  }
};

template <typename T>
struct SplitMenu {
  std::vector<shared_ptr<SplitAction<T>>> items;
  
  SplitMenu(const String& inLabel, const double inDefault) 
  : items({ 
    std::make_shared<SplitActionLabel<T>>(inLabel),
    std::make_shared<SplitActionAuto<T>>(inDefault),
    std::make_shared<SplitActionAll<T>>(),
    std::make_shared<SplitActionCount<T>>(),
    std::make_shared<SplitActionPercent<T>>()
  })
  {
  }
  
  String description() const {
    const char* SEP = "\n  ";
    std::vector<String> descriptions(items.size());
    std::transform(items.begin(), items.end(), descriptions.begin(), 
                   [](const std::shared_ptr<SplitActionBase>& item) 
                   { return item->description(); });
    return SEP + boost::algorithm::join(descriptions, SEP);
  }
  
  Result<std::pair<UL, UL>> split(const T& inRange, const String& value) const {
    for(const auto& action: items) {
      auto split = action->split(inRange, value);
      if (split) { return split.value(); }
    }
    return Error("Unknown split action: '" + value + "'.");
  }
};

static Result<String>
parseFormat(const po::variables_map& inVM)
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

static ostream_ptr
parseReportStream(const std::string& classifierName,
                  const po::variables_map& inVM,
                  const std::string& inInputFileName, 
                  const std::string& suffix = "")
{
  if (inVM[oReportFile].empty()) {
    return Command::nullOStream();
  } else {
    auto name = string_format(inVM[oReportFile].as<std::string>(),
                              inInputFileName.c_str(),
                              classifierName.c_str(),
                              suffix.c_str());
    return Command::ostreamWithName(name);
  }
}


#endif /* split_private_h */
