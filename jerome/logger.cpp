//
//  logger.cpp
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

#include <iostream>
#include <fstream>
#include <future>
#include <memory>
#include "logger.hpp"

namespace jerome {
namespace log {

static const char* string4level(Logger::Level level) {
  switch (level) {
  case Logger::Level::info: return "Info";
  case Logger::Level::error: return "Error";
  }
}

struct LogTarget {
  // output string to the target buffer on a separate thread
  // syncronize the output using a mutex.
  struct Implementation {
    virtual std::ostream& stream() = 0;
    virtual ~Implementation() {}
    void print(const std::string& inString) {
      std::async(std::launch::async, [=] {
        std::lock_guard<std::mutex> guard(this->semaphore);
        this->stream() << inString;
      });
    }
    std::mutex semaphore;
  };

  struct File: Implementation {
    File(const char* inPath)
    : streamStorage(inPath) {}

    std::ofstream  streamStorage;
    std::ostream& stream() { return streamStorage; }
  };

  struct Stream: Implementation {
    Stream(std::ostream& inStream)
    : streamStorage(inStream) {}

    std::ostream&  streamStorage;
    std::ostream& stream() { return streamStorage; }
  };

  LogTarget(const char* inPath)
  : implementaion(std::make_shared<File>(inPath))
  {}

  LogTarget(std::ostream& inStream)
  : implementaion(std::make_shared<Stream>(inStream))
  {}

  void print(const std::string& inString) {
    implementaion->print(inString);
  }

private:
  std::shared_ptr<Implementation> implementaion;
};

static LogTarget cerr(std::cerr);
static LogTarget cout(std::cout);

Logger::Logger(LogTarget& inStream, Level inLevel)
: target(inStream)
, level(inLevel)
, isDirty(false)
{
}

Logger::~Logger() {
  flush();
}

void Logger::flush() {
  if (!isDirty) { return; }
  auto size = oss.str().size();
  if (size == 0 || oss.str()[size-1] != '\n') {
    oss << std::endl;
  }
  auto output = std::string("[") + string4level(level) + "] " + oss.str();
  oss.str("");
  isDirty = false;
  target.print(output);
}

Logger info() {
  return Logger(cerr, Logger::Level::info);
}

Logger error() {
  return Logger(cerr, Logger::Level::error);
}

Logger& flush(Logger& l) {
  l.flush();
  return l;
}
}
}

