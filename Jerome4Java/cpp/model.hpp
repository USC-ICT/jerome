//
//  model.hpp
//
//  Created by Anton Leuski on 9/4/19.
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

#ifndef model_hpp
#define model_hpp

#ifdef __APPLE__
# include<JavaVM/jni.h>
#else
# include<jni.h>
#endif

#include <jerome/npc/platform.hpp>

using namespace jerome;
using namespace jerome::npc;

struct UtteranceCallback;

struct Model : enable_shared_from_this<Model> {
  Platform platform;
  OptionalString mainMachineName;
  StringMap<shared_ptr<UtteranceCallback>> callbacks;
  
  void resetDialogue();
  void tossDialogueToStateWithName(const String& inName);
  void respondTo(JNIEnv * inEnv, jobject inCallback, const String& inText);
  void installEngineHandler();
  
  jobject utterance(JNIEnv * inEnv, const String& inID);
  jobject completion(JNIEnv * inEnv, const StringStringMap& inData);
};

struct Completion {
  weak_ptr<Model> jerome;
  StringStringMap eventData;
  
  Completion(const shared_ptr<Model>& inModel, 
             const StringStringMap& inEventData);
  
  void send(const String& inEvent);
};


#endif /* model_hpp */
