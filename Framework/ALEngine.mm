//
//  ALEngine.m
//  jerome
//
//  Created by Anton Leuski on 5/11/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
@end

@interface ALScriptingEngine : NSObject <ALScriptingEngineProtocol>
@property (nonatomic, weak) ALEngine* client;
@end

using namespace jerome::scripting;
using namespace jerome::npc;


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
    self.queue = dispatch_queue_create("edu.usc.ict.ScriptingEngine",
                                       DISPATCH_QUEUE_SERIAL);
  }
  return self;
}

- (detail::Engine&)platform
{
  return self->_platformStorage;
}

- (BOOL)readCollectionFromURL:(NSURL*)url error:(NSError **)outError
{
  std::ifstream	stream(url.absoluteURL.fileSystemRepresentation);
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

- (void)readDialogueManagerFromURL:(NSURL*)url
                  completionHandle:(ALDialoguScriptLoadedHandled _Nonnull)completionHandle
{
  NSError* error;
  NSString* source = [NSString stringWithContentsOfURL:url usedEncoding:nil error:&error];
  if (!source) {
    completionHandle(nil, error);
    return;
  }
  
  __weak ALEngine* weakSelf = self;
  dispatch_async(self.queue, ^{
    ALEngine* strongSelf = weakSelf;
    if (!strongSelf) return;

    [strongSelf initializeScripting];
    [strongSelf.context[@"initStateMachineWithString"]
      callWithArguments:@[ source, ^(JSValue* error, NSString* name) {
      if (name) {
        completionHandle(name, nil);
      } else {
        completionHandle(nil,
         [NSError errorWithDomain:@"JS"
                                  code:0
                              userInfo:@{ NSLocalizedDescriptionKey : error.description }]);
      }
      }]];
  });
}

- (ALUtterance* _Nullable)classifier:(NSString* _Nonnull)stateName
                           respondTo:(NSString* _Nonnull)question
{
  auto rl = self.platform.search(stateName.cppString,
              detail::Engine::queryForString(question.cppString));
  if (rl.size()) {
    return [ALUtterance utteranceWithUtterance:rl[0]];
  } else {
    return nil;
  }
}

- (void)postEvent:(NSString* _Nonnull)eventName
         withData:(NSDictionary<NSString*, NSString*>* _Nonnull)data
        toMachine:(NSString* _Nonnull)machine
{
  __weak ALEngine* weakSelf = self;
  dispatch_async(self.queue, ^{
    ALEngine* strongSelf = weakSelf;
    if (!strongSelf) return;

    [strongSelf initializeScripting];
    [strongSelf.context[@"postEventToStateMachine"]
     callWithArguments:@[ machine, eventName, data ]];
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
  
  self.context[@"__scionPlatform"] = [ALScionPlatform scionPlatformWithQueue:self.queue];
  
  [self.context evaluateScript:[NSString stringWithUTF8String:SCION_SCRIPT.source]
                 withSourceURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:SCION_SCRIPT.name]]];
  
  [self.context evaluateScript:[NSString stringWithUTF8String:JEROME_STARTUP_SCRIPT.source]
                 withSourceURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:JEROME_STARTUP_SCRIPT.name]]];
  
  [self.context evaluateScript:[NSString stringWithUTF8String:JEROME_UTTERANCE_COMPILER_SCRIPT.source]
                 withSourceURL:[NSURL fileURLWithPath:[NSString stringWithUTF8String:JEROME_UTTERANCE_COMPILER_SCRIPT.name]]];

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

