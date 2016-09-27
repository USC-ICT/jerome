//
//  split.private.hpp
//  jerome
//
//  Created by Anton Leuski on 9/20/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
                  const std::string& suffix = "")
{
  if (inVM[oReportFile].empty()) {
    return Command::nullOStream();
  } else {
    auto name = string_format(inVM[oReportFile].as<std::string>(),
                              inVM[oInputFile].as<std::string>().c_str(),
                              classifierName.c_str(),
                              suffix.c_str());
    return Command::ostreamWithName(name);
  }
}


#endif /* split_private_h */
