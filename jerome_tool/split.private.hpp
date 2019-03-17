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

template <typename T>
static std::pair<UL, UL>
splitData(const po::variables_map& inVM,
          const String& inKey,
          const T& inRange,
          double inDefaultProportion,
          const String& inLabel)
{
  std::string propText = inVM[inKey].as<std::string>();
  if (propText == "label") {
    return jerome::split<UL, T>(inRange,
                                [&inLabel](const Utterance& u) {
                                  return u.get("train_test", "train") == inLabel;
                                });
  } else {
    double testProp = inDefaultProportion;
    if (propText == "auto") {
    } else if (hasSuffix(propText, "%")) {
      testProp = std::atof(propText.c_str()) / 100.0;
    } else {
      testProp = std::atof(propText.c_str());
    }
    return jerome::split<UL, T> (inRange, testProp);
  }
}

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
