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
splitData(const po::variables_map& inVM, const String& inKey, const T& inRange,
          double inDefaultProportion, const String& inLabel)
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

#endif /* split_private_h */
