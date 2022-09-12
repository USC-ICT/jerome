//
//  ALPlatform.m
//  NPCEditor
//
//  Created by Anton Leuski on 10/20/15.
//  Copyright © 2015 Anton Leuski. All rights reserved.
//

#import <JavaScriptCore/JavaScript.h>
#include <fstream>
#include <jerome/npc/platform.hpp>
#import "ObjectiveC++.h"
#import "model_private.h"
#import "ALPlatform.h"

using namespace jerome;

@interface ALPlatform ()
@property (nonatomic, assign) jerome::npc::Platform platform;
@property (nonatomic, copy) ALEngineEventHandler storedEngineEventHandler;
@end

@implementation ALPlatform

+ (void)initialize
{
  if (self == [ALPlatform class]) {
    jerome::npc::Platform::initialize();
  }
}

- (instancetype)init
{
	if (self = [super init]) {
    self.storedEngineEventHandler = nil;
//		self.engine = std::make_shared<npc::detail::Engine>();
//		self.context = [JSContext contextWithJSGlobalContextRef:(JSGlobalContextRef)self->_engine->context()];
//		self.engine->setEngineEventHandler([self](const npc::EngineEvent& event) {
//			id<ALPlatformDelegate> del = self.delegate;
//			if (!del || ![del respondsToSelector:@selector(platform:didReceiveEvent:)]) return;
//			ALPlatformEvent* e = [ALPlatformEvent new];
//			e.name = [NSString stringWithCPPString:event.name()];
//			e.type = [NSString stringWithOptionalCPPString:event.type()];
//			e.target = [NSString stringWithOptionalCPPString:event.target()];
//			e.origin = [NSString stringWithOptionalCPPString:event.origin()];
//			e.data = [NSDictionary dictionaryWithStringMap:event.data()];
//			[del platform:self didReceiveEvent:e];
//		});
	}
	return self;
}

- (void)dealloc {
  self->_platform.setEngineEventHandler(
    [](const jerome::scripting::EngineEvent& event) {});
  ALEngineEventHandler handler = self.storedEngineEventHandler;
  if (handler) {
    handler([ALPlatformEvent eventWithName:ALPlatformEventNameEndOfStream]);
  }
}

- (BOOL)readCollectionFromURL:(NSURL*)url error:(NSError **)outError
{
	std::ifstream	stream(url.absoluteURL.fileSystemRepresentation);
	auto result = self->_platform.loadCollection(stream);
	if (outError) {
		*outError = [NSError errorWithOptionalError:result];
	}
	return !result;
}

- (BOOL)writeCollectionToURL:(NSURL*)url error:(NSError**)outError
{
 	std::ofstream	stream(url.absoluteURL.fileSystemRepresentation);
  auto result = self->_platform.saveCollection(stream);
  if (outError) {
    *outError = [NSError errorWithOptionalError:result];
  }
  return !result;
 
}

- (void)readDialogueManagerFromURL:(NSURL*)url
                  completionHandle:(ALDialogueScriptLoadedHandle)completionHandle
{
	std::ifstream	inp(url.absoluteURL.fileSystemRepresentation);
	self->_platform.loadDialogueManager(inp,
    [self, completionHandle](const Result<scripting::DialogueManagerMetadata>& result)
  {
		if (result) {
      ALDialogueManagerMetadata* metadata = [ALDialogueManagerMetadata new];
      metadata.name = [NSString stringWithCPPString:result.value().name];
      metadata.hasStages = result.value().hasStages;
      completionHandle(metadata, nil);
		} else {
      completionHandle(nil, [NSError errorWithError:result.error()]);
    }
	});
}

- (ALUtterance* _Nullable)classifier:(NSString* _Nonnull)stateName
                           respondTo:(NSString* _Nonnull)question
{
  return [ALUtterance utteranceWithOptionalUtterance:self->_platform.respond(stateName.cppString, question.cppString)];
}

- (void)postEvent:(NSString* _Nonnull)eventName
         withData:(NSDictionary<NSString*, NSString*>* _Nonnull)data
        toMachine:(NSString* _Nonnull)machine
{
  self->_platform.postEvent(eventName.cppString, data.stringMap, machine.cppString);
}

- (void)setEngineEventHandler:(ALEngineEventHandler)eventHandler
{
  self.storedEngineEventHandler = eventHandler;
  self->_platform.setEngineEventHandler(
  [self, eventHandler](const jerome::scripting::EngineEvent& event)
  {
    ALPlatformEvent* e = [ALPlatformEvent new];
    e.name = [NSString stringWithCPPString:event.name()];
    e.type = [NSString stringWithOptionalCPPString:event.type()];
    e.target = [NSString stringWithOptionalCPPString:event.target()];
    e.origin = [NSString stringWithOptionalCPPString:event.origin()];
    e.data = [NSDictionary dictionaryWithStringMap:event.data()];
    eventHandler(e);
  });
}

- (ALUtterance* _Nullable)utteranceWithID:(NSString* _Nonnull)utteranceID
{
  return [ALUtterance utteranceWithOptionalUtterance:self->_platform.utteranceWithID(utteranceID.cppString)];
}

- (void)collectionWasUpdated
{
  self->_platform.collectionWasUpdated();
}

- (void)collectionWasUpdatedInState:(NSString* _Nonnull)stateName
{
  self->_platform.collectionWasUpdated(stateName.cppString);
}



@end
