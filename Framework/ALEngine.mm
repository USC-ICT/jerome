//
//  ALEngine.m
//  jerome
//
//  Created by Anton Leuski on 5/11/16.
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

#include <fstream>
#include <jerome/npc/engine.hpp>
#include <jerome/scripting/scripts.hpp>
#import "ObjectiveC++.h"
#import "model_private.h"
#import "Scion.h"
#import "ALEngine.h"

@protocol ALScriptingEngineProtocol <JSExport>
- (NSArray*)search:(NSString*)name :(JSValue*)query;
- (NSArray*)answers:(NSString*)name;
- (ALUtterance*)utteranceWithID:(NSString*)inUtteranceID;
- (void)receiveEvent:(NSString*)inName :(NSString*)inType :(NSString*)inTarget
                    :(NSString*)inOrigin :(NSDictionary*)inData;
- (void)didTransitionToState:(NSString* _Nonnull)stateName
                            :(NSDictionary* _Nonnull)data;
@end

@interface ALScriptingEngine : NSObject <ALScriptingEngineProtocol>
@property (nonatomic, weak) ALEngine* client;
@end

using namespace jerome::scripting;
using namespace jerome::npc;

@interface JSContext (AL)
- (JSValue *)evaluateScriptLiteral:(ScriptLiteral *)script;
@end

@implementation JSContext (AL)

- (JSValue *)evaluateScriptLiteral:(ScriptLiteral *)script
{
  NSString* name = [NSString stringWithUTF8String:script->name];
  return [self evaluateScript:[NSString stringWithUTF8String:script->source]
                withSourceURL:[NSURL fileURLWithPath:name]];
}

@end

@interface ALEngine ()
@property (nonatomic, assign) detail::Engine platformStorage;
@property (nonatomic, assign, readonly) detail::Engine& platform;
@property (nonatomic, strong) JSContext* context;
@property (nonatomic, strong) dispatch_queue_t queue;
@property (nonatomic, copy) ALEngineEventHandler _Nullable engineEventHandler;
@property (nonatomic, strong) ALScriptingEngine* scriptingEngine;
@end

@implementation ALEngine

+ (void)initialize
{
  if (self == [ALEngine class]) {
    detail::Engine::initialize();
  }
}

- (instancetype)init
{
  if (self = [super init]) {
    self.engineEventHandler = ^(ALPlatformEvent* event){
    };

    self.queue = dispatch_queue_create("edu.usc.ict.ScriptingEngine",
                                       DISPATCH_QUEUE_SERIAL);
    self.log = ^(NSArray<NSObject*>* array) {
      NSLog(@"%@", array);
    };
    
    self.platform.setUsingLookupTable(true);
  }
  return self;
}

- (detail::Engine&)platform
{
  return self->_platformStorage;
}

struct membuf : std::streambuf
{
  membuf(char* begin, char* end) {
    this->setg(begin, begin, end);
  }
};

- (BOOL)readCollectionFromURL:(NSURL*)url error:(NSError **)outError
{
  // we should be able load from remote URLs
  //  std::ifstream  stream(url.absoluteURL.fileSystemRepresentation);
  NSData* data = [NSData dataWithContentsOfURL:url 
                                       options:0 
                                         error:outError];
  if (!data) { 
    return NO;     
  }
  
  return [self readCollectionFromData:data error:outError];
}

- (BOOL)readCollectionFromData:(NSData*)data
                         error:(NSError**)outError
{
  membuf sbuf((char*)data.bytes, (char*)data.bytes + data.length);
  std::istream stream(&sbuf);
  auto result = self.platform.loadCollection(stream);
  if (outError) {
    *outError = [NSError errorWithOptionalError:result];
  }
  return !result;
}

- (BOOL)writeCollectionToURL:(NSURL*)url error:(NSError**)outError
{
 	std::ofstream	stream(url.absoluteURL.fileSystemRepresentation);
  auto result = self.platform.saveCollection(stream);
  if (outError) {
    *outError = [NSError errorWithOptionalError:result];
  }
  return !result;
}

- (void)doReadDialogueManagerFromSource:(NSString*)source
                       completionHandle:(ALDialogueScriptLoadedHandle
                                         _Nonnull)completionHandle
{
  [self initializeScripting];
  [self.context[@"initStateMachineWithString"]
   callWithArguments:@[ source, ^(JSValue* error, NSDictionary* data) {
    if (data) {
      id name = data[@"name"];
      if (name == nil || ![name isKindOfClass:[NSString class]]) {
        completionHandle(nil,
                         [NSError errorWithDomain:@"JS"
                                             code:0
                                         userInfo:@{ NSLocalizedDescriptionKey
                                                     : @"missing name attribute in DM scxml tag" }]);
        return;
      }
      ALDialogueManagerMetadata* metadata = [ALDialogueManagerMetadata new];
      metadata.name = name;
      NSDictionary* datamodel = data[@"datamodel"];
      if (datamodel && [datamodel isKindOfClass:[NSDictionary class]]) {
        NSDictionary* hasStagesEntry = datamodel[@"hasStages"];
        if (hasStagesEntry && [hasStagesEntry isKindOfClass:[NSDictionary class]]) {
          NSString* hasStagesString = hasStagesEntry[@"content"];
          if (hasStagesString && [hasStagesString isKindOfClass:[NSString class]]) {
            metadata.hasStages = [[hasStagesString lowercaseString] isEqualToString:@"true"];
          }
        }
      }
      completionHandle(metadata, nil);
    } else {
      completionHandle(nil,
                       [NSError errorWithDomain:@"JS"
                                           code:0
                                       userInfo:@{ NSLocalizedDescriptionKey
                                                   : error.description }]);
    }
  }]];
}

- (void)readDialogueManagerFromURL:(NSURL*)url
                  completionHandle:(ALDialogueScriptLoadedHandle
                                    _Nonnull)completionHandle
{
  NSError* error;
  NSString* source = [NSString stringWithContentsOfURL:url
                                          usedEncoding:nil
                                                 error:&error];
  if (!source) {
    completionHandle(nil, error);
    return;
  }
  
  __weak ALEngine* weakSelf = self;
  dispatch_async(self.queue, ^{
    ALEngine* strongSelf = weakSelf;
    [strongSelf doReadDialogueManagerFromSource:source
                               completionHandle:completionHandle];
  });
}

- (ALDialogueManagerMetadata*)readDialogueManagerFromURL:(NSURL*)url
                                                   error:(NSError**)outError
{
  NSString* source = [NSString stringWithContentsOfURL:url
                                          usedEncoding:nil
                                                 error:outError];
  if (!source) {
    return nil;
  }
  
  __block NSError* error = nil;
  __block ALDialogueManagerMetadata* metadata = nil;
  
  dispatch_sync(self.queue, ^{
    [self doReadDialogueManagerFromSource:source
                         completionHandle:^(ALDialogueManagerMetadata* inMetadata,
                                            NSError* inError)
    {
      metadata = inMetadata;
      error = inError;
    }];
  });
  
  if (outError) *outError = error;
  return metadata;
}

- (ALDialogueManagerMetadata*)readDialogueManagerFromData:(NSData*)data
                                                    error:(NSError**)outError
{
  NSString* source = [[NSString alloc] initWithData:data 
                                           encoding:NSUTF8StringEncoding];
  if (!source) {
    if (outError) {
      *outError = [NSError errorWithDomain:@"jerome" 
                                      code:-1 
                                  userInfo:@{NSLocalizedDescriptionKey: @"Cannot interpret data as UTF8"}];
    }
    return nil;
  }
  
  __block NSError* error = nil;
  __block ALDialogueManagerMetadata* metadata = nil;
  
  dispatch_sync(self.queue, ^{
    [self doReadDialogueManagerFromSource:source
                         completionHandle:^(ALDialogueManagerMetadata* inMetadata,
                                            NSError* inError)
     {
       metadata = inMetadata;
       error = inError;
     }];
  });
  
  if (outError) *outError = error;
  return metadata;
}

- (ALUtterance* _Nullable)do_classifier:(NSString* _Nonnull)stateName
                              respondTo:(NSString* _Nonnull)question
                                  error:(NSError**)outError
{
  [self initializeScripting];
  try {
    auto rl = self.platform.search(stateName.cppString,
                detail::Engine::queryForString(question.cppString));
    if (outError) *outError = nil;
    if (rl.size()) {
      return [ALUtterance utteranceWithUtterance:rl[0]];
    } else {
      return nil;
    }
  } catch (const jerome::Error& e) {
    if (outError)
      *outError = [NSError errorWithError:e];
    return nil;
  }
}

- (ALUtterance* _Nullable)classifier:(NSString* _Nonnull)stateName
                           respondTo:(NSString* _Nonnull)question
                               error:(NSError* _Nullable __autoreleasing * _Nullable)outError
{
  __block ALUtterance*  result = nil;
  dispatch_sync(self.queue, ^{
    result = [self do_classifier:stateName respondTo:question error:outError];
  });
  return result;
}

- (void)classifier:(NSString* _Nonnull)stateName
         respondTo:(NSString* _Nonnull)question
  completionHandle:(void(^)(ALUtterance* _Nullable,
                            NSError* _Nullable))handle
{
  __weak ALEngine* weakSelf = self;
  dispatch_async(self.queue, ^{
    ALEngine* strongSelf = weakSelf;
    if (!strongSelf) return;
    NSError* error = nil;
    ALUtterance* result = [weakSelf do_classifier:stateName
                                        respondTo:question error:&error];
    handle(result, error);
  });
}

- (void)doPostEvent:(NSString* _Nonnull)eventName
         withData:(NSDictionary<NSString*, id>* _Nonnull)data
        toMachine:(NSString* _Nonnull)machine
{
  [self initializeScripting];
  [self.context[@"postEventToStateMachine"]
   callWithArguments:@[ machine, eventName, data ]];
}

- (void)postEventWithName:(NSString* _Nonnull)eventName
                     data:(NSDictionary<NSString*, id>* _Nonnull)data
                toMachine:(NSString* _Nonnull)machine
{
  __weak ALEngine* weakSelf = self;
  dispatch_async(self.queue, ^{
    ALEngine* strongSelf = weakSelf;
    [strongSelf doPostEvent:eventName withData:data toMachine:machine];
  });
}

- (ALUtterance* _Nullable)utteranceWithID:(NSString* _Nonnull)utteranceID
{
  return [ALUtterance utteranceWithOptionalUtterance:
          self.platform.utteranceWithID(utteranceID.cppString)];
}

- (void)collectionWasUpdated
{
  self.platform.collectionWasUpdated(jerome::OptionalString());
}

- (void)collectionWasUpdatedInState:(NSString* _Nonnull)stateName
{
  self.platform.collectionWasUpdated(stateName.cppString);
}

- (void)initializeScripting
{
  if (self.context) return;
  
  self.context = [JSContext new];
  self.context.exceptionHandler = ^(JSContext *context, JSValue *exception){
    NSLog(@"Exception in JavaScript: %@", exception);
  };
  
  self.context[@"__scionPlatform"]
    = [ALScionPlatform scionPlatformWithQueue:self.queue log:self.log];
  
  [self.context evaluateScriptLiteral:&SCION_SCRIPT];
  [self.context evaluateScriptLiteral:&JEROME_STARTUP_SCRIPT];
  [self.context evaluateScriptLiteral:&JEROME_UTTERANCE_COMPILER_SCRIPT];

  // I decided to retain the scriptingEngine
  self.scriptingEngine = [ALScriptingEngine new];
  self.scriptingEngine.client = self;
  
  self.context[@"classifier"] = self.scriptingEngine;
}

@end

@implementation ALScriptingEngine

static NSArray* utteranceVectorToArray(const std::vector<Utterance>& utterances)
{
  NSMutableArray* array = [NSMutableArray new];
  for(const auto& u : utterances) {
    [array addObject:[ALUtterance utteranceWithUtterance:u]];
  }
  return array;
}

- (NSArray*)search:(NSString*)name :(JSValue*)query
{
  ALEngine* engine = self.client;
  if (!engine) return [NSArray array];
  
  if ([query isString]) {
    Utterance q(detail::Engine::queryForString([[query toString] UTF8String]));
    return utteranceVectorToArray(engine.platform.search(name.cppString, q));
  } else if ([query isObject]) {
    NSDictionary* object = [query toDictionary];
    cpp::ObjectFactory   of;
    Utterance q(of.newUtterance());
    for (NSString* key in object) {
      id v = object[key]; // already converted to a ObjC type
      if ([v isKindOfClass:[NSString class]]
          || [v isKindOfClass:[NSNumber class]]) {
        q.put(key.cppString, [v description].cppString);
      }
    }
    return utteranceVectorToArray(engine.platform.search(name.cppString, q));
    
  } else {
    return [NSArray array];
  }
}

- (NSArray*)answers:(NSString*)name
{
  ALEngine* engine = self.client;
  if (!engine) return [NSArray array];

  if ([JSContext currentArguments].count > 0) {
    return utteranceVectorToArray(engine.platform.answers(name.cppString));
  }
  return utteranceVectorToArray(engine.platform.answers());
}

- (ALUtterance*)utteranceWithID:(NSString*)inUtteranceID
{
  ALEngine* engine = self.client;
  if (!engine) return nil;

  return [ALUtterance utteranceWithOptionalUtterance:
          engine.platform.utteranceWithID(inUtteranceID.cppString)];
}

- (void)didTransitionToState:(NSString*)stateName
                            :(NSDictionary*)data
{
  [self receiveEvent:@"didTransitionToState" :nil :stateName :nil :data];
}

- (void)receiveEvent:(NSString*)inName :(NSString*)inType :(NSString*)inTarget
                    :(NSString*)inOrigin :(NSDictionary*)inData
{
  ALEngine* engine = self.client;
  if (!engine) return;

  ALPlatformEvent* event = [ALPlatformEvent new];
  event.name = inName;
  event.type = inType;
  event.target = inTarget;
  event.origin = inOrigin;
  event.data = inData;
  
  engine.engineEventHandler(event);
}

@end

