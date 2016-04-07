//
//  ALChatEngine.m
//  Twins
//
//  Created by Anton Leuski on 2/9/13.
//  Copyright (c) 2013 Anton Leuski. All rights reserved.
//

#import "ALChatEngine.h"

#include <alib/npc/classifier.h>
#include <alib/npc/model_cpp.h>
#include <alib/scripting/engine.h>

using namespace alib;
using namespace alib::npc;

static NSDictionary* dictionaryFromUtterance(const Utterance& inUtterance)
{
	NSMutableDictionary*	result = [NSMutableDictionary new];
	for(NSString* key in @[ @"speaker", @"text", @"id" ]) {
		if (inUtterance.has([key UTF8String]))
			result[key] = [NSString stringWithUTF8String:inUtterance.get([key UTF8String]).c_str()];
	}
	return result;
}

struct MyEngine : public alib::scripting::Engine {
	MyEngine(const alib::npc::Collection& inCollection, ALChatEngine* inChatEngine)
	: alib::scripting::Engine(inCollection) {
		mChatEngine	= inChatEngine;
	}
	
	~MyEngine() {
		mChatEngine	= nil;
	}
	
	void answerQuestion(const char* inQuestion) {
		StringStringMap	data;
		data["text"]	= inQuestion;
		postEvent("vrSpeech_asr_complete", data, "dm");
	}
	
	virtual void send(const StringStringMap& inEvent) {
	
		if (inEvent.find("utteranceID") == inEvent.end() && inEvent.find("command") == inEvent.end())
			return;
		
		String			eventName	= inEvent.at("complete");
		String			machineName	= inEvent.at("machineName");
		
		ALSendUtteranceCompletionBlock	_completionBlock	= ^{
			try {
				this->postEvent(eventName, StringStringMap(), machineName);
			} catch (std::exception& ex) {
				std::cerr << ex.what() << std::endl; // TODO handle error
			}
		};
		
		ALSendUtteranceCompletionBlock	completionBlock		= ^{
			dispatch_async(dispatch_get_main_queue(), _completionBlock);
		};
		
		if (inEvent.find("utteranceID") == inEvent.end()) {
			completionBlock();
			return;
		}
		
		Utterance		utterance	= utteranceWithID(inEvent.at("utteranceID"));
		
		ALChatEngine*					chatEngine	= mChatEngine;
		id<ALChatEngineDisplayDelegate>	del			= chatEngine.displayDelegate;
		
		if (del && [del respondsToSelector:@selector(chatEngine:didSendUtterance:completionHandler:)]) {
			[del chatEngine:chatEngine didSendUtterance:dictionaryFromUtterance(utterance) completionHandler:completionBlock];
		} else {
			completionBlock();
		}
		
	}
	
private:
	__weak ALChatEngine*	mChatEngine;
	
};

@interface ALChatEngine ()
@property (nonatomic, assign) MyEngine*	engine;
@end

@implementation ALChatEngine

- (void)dealloc
{
	delete self.engine;
	self.engine	= nil;
}

- (void)AL_fireStatusUpdate:(NSString*)status
{
	id<ALChatEngineLoadingDelegate>	del = self.loadingDelegate;
	if (del && [del respondsToSelector:@selector(chatEngine:loadingStatusDidChange:)]) {
		dispatch_async(dispatch_get_main_queue(), ^{
			[del chatEngine:self loadingStatusDidChange:status];
		});
	}
}

- (void)AL_evaluateScriptWithName:(NSString*)scriptName
{
	NSURL*		scriptURL	= [[NSBundle mainBundle] URLForResource:scriptName withExtension:@"js"];
	NSString*	scriptText	= [NSString stringWithContentsOfURL:scriptURL encoding:NSUTF8StringEncoding error:nil];
	self.engine->evaluateScript([scriptText UTF8String], [[scriptURL absoluteString] UTF8String], 0, true);
}

- (void)AL_load
{
	[self AL_fireStatusUpdate:@"Loading language data..."];
	
	NSString*	path	= [[NSBundle mainBundle] pathForResource:@"irregular_verbs" ofType:@"txt"];
	configure([path UTF8String], [[[path stringByDeletingLastPathComponent] stringByAppendingPathComponent:@"kstem-lexicon-beta"] UTF8String]);
	
	[self AL_fireStatusUpdate:@"Loading character data..."];

	alib::npc::cpp::ObjectFactory	of;
	alib::npc::Collection			collection 	= loadCollection(of, [[[NSBundle mainBundle] pathForResource:@"twins.plist" ofType:@"csxml"] UTF8String]);

	[self AL_fireStatusUpdate:@"Making classifier..."];

	self.engine		= new MyEngine(collection, self);
	self.engine->ranker("Anyone - The Twins");

	[self AL_fireStatusUpdate:@"Initializing dialogue engine..."];

	[self AL_evaluateScriptWithName:@"scion"];
	[self AL_evaluateScriptWithName:@"startup"];
	[self AL_evaluateScriptWithName:@"utteranceCompiler"];

	NSURL*		dmURL	= [[NSBundle mainBundle] URLForResource:@"dm" withExtension:@"scxml"];
	self.engine->loadDialogueManager([[dmURL absoluteString] UTF8String]);

	[self AL_fireStatusUpdate:@"Done"];
}

- (void)load
{
	dispatch_async(dispatch_queue_create(NULL, NULL), ^{
		NSError*	error = nil;
		@try {
			try {
				[self AL_load];
			} catch (...) {
				// TODO handle error
			}
		} @catch (...) {
			// TODO handle error
		}
		id<ALChatEngineLoadingDelegate>	del = self.loadingDelegate;
		if (del && [del respondsToSelector:@selector(chatEngine:loadDidCompleteWithError:)]) {
			dispatch_async(dispatch_get_main_queue(), ^{
				[del chatEngine:self loadDidCompleteWithError:error];
			});
		}
	});
}

- (void)answerUtterance:(NSString*)text
{
	try {
		NSLog(@"answer %@", text);
		self.engine->answerQuestion([text UTF8String]);
	} catch (...) {
		// TODO handle error		
	}
}

@end
