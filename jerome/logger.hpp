//
//  logger.hpp
//
//  Created by Anton Leuski on 11/23/19.
//  Copyright © 2019 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef logger_hpp
#define logger_hpp

#include <sstream>

namespace jerome {
namespace log {

struct LogTarget;

// the assumption is that you use a Logger object in one thread only
// you cannot copy it and you can barely pass it around.

struct Logger {
  ~Logger();

  template <typename T>
  Logger& operator <<(const T& data);

  void flush();

  enum Level {
    info,
    error
  };

  Logger(Logger&&) = default;

private:

  Logger(const Logger&) = delete;
  Logger& operator = (const Logger&) = delete;

  std::ostringstream oss;
  LogTarget& target;
  Level level;
  bool isDirty;

  Logger() = delete;
  Logger(LogTarget& inTarget, Level inLevel);

  friend Logger& flush(Logger& l);
  friend Logger info();
  friend Logger error();
};

Logger info();
Logger error();

Logger& flush(Logger& l);

template <typename T>
inline Logger& Logger::operator <<(const T& data) {
  isDirty = true;
  oss << data;
  return *this;
}

}
}

#endif /* logger_hpp */
