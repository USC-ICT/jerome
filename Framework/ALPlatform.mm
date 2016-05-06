//
//  ALPlatform.m
//  NPCEditor
//
//  Created by Anton Leuski on 10/20/15.
//  Copyright © 2015 Anton Leuski. All rights reserved.
//

#import <JavaScriptCore/JavaScriptCore.h>
#include <fstream>
#include <jerome/npc/platform.hpp>
#import "ObjectiveC++.h"
#import "ALPlatform.h"

using namespace jerome;


@implementation ALUtterance

+ (ALUtterance* _Nonnull)utteranceWithUtterance:(const npc::Utterance&)utterance
{
  return nil;
}

+ (ALUtterance* _Nullable)utteranceWithOptionalUtterance:(const optional<npc::Utterance>&)utterance
{
  return utterance ? [self utteranceWithUtterance:*utterance] : nil;
}

@end

@implementation ALPlatformEvent
@end

@interface ALPlatform ()
@property (nonatomic, assign) jerome::npc::Platform platform;
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

- (void)readDialogueManagerFromURL:(NSURL*)url completionHandle:(void (^_Nonnull)(NSString *, NSError *))completionHandle
{
	std::ifstream	inp(url.absoluteURL.fileSystemRepresentation);
	self->_platform.loadDialogueManager(inp,
    [self, completionHandle](const Result<String>& result)
  {
		if (result) {
      completionHandle([NSString stringWithCPPString:result.value()], nil);
		} else {
      completionHandle(nil, [NSError errorWithError:result.error()]);
    }
	});
}

- (BOOL)trainClassifierAtState:(NSString* _Nonnull)stateName completionHandle:(void (^_Nonnull)(ALTrainingState * _Nonnull))completionHandle error:(NSError * _Nullable * _Nullable)outError
{
  return false;
}

- (ALUtterance* _Nullable)classifier:(NSString* _Nonnull)stateName respondTo:(NSString* _Nonnull)question
{
  return [ALUtterance utteranceWithOptionalUtterance:self->_platform.respond(stateName.cppString, question.cppString)];
}

- (void)postEvent:(NSString* _Nonnull)eventName withData:(NSDictionary<NSString*, NSString*>* _Nonnull)data toMachine:(NSString* _Nonnull)machine
{
  self->_platform.postEvent(eventName.cppString, data.stringMap, machine.cppString);
}

- (void)setEngineEventHandler:(void (^_Nonnull)(ALPlatformEvent * _Nonnull))eventHandler
{
  self->_platform.setEngineEventHandler(
  [self, eventHandler](const npc::EngineEvent& event)
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
