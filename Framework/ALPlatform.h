//
//  ALPlatform.h
//  NPCEditor
//
//  Created by Anton Leuski on 10/20/15.
//  Copyright © 2015 Anton Leuski. All rights reserved.
//

#import <Foundation/Foundation.h>

@class ALPlatform;

@interface ALPlatformEvent : NSObject
@property (nonatomic, copy, nonnull)  NSString* name;
@property (nonatomic, copy, nullable)  NSString* type;
@property (nonatomic, copy, nullable)  NSString* target;
@property (nonatomic, copy, nullable)  NSString* origin;
@property (nonatomic, copy, nonnull)  NSDictionary<NSString*, NSString*>* data;
@end

//@protocol ALPlatformDelegate <NSObject>
//@optional
//- (void)platform:(nonnull ALPlatform*)platform didReceiveEvent:(nonnull ALPlatformEvent*)event;
//- (void)platform:(nonnull ALPlatform*)platform didLoadDialogueManagerWithName:(nonnull NSString*)name;
//- (void)platform:(nonnull ALPlatform*)platform didFailToLoadDialogueManager:(nonnull NSError*)error;
//@end
//
//@class JSContext;

@interface ALTrainingState : NSObject
@end

@interface ALUtterance : NSObject
@end

@interface ALCollection : NSObject
@end

@interface ALPlatform : NSObject
@property(nonatomic, strong, readonly) ALCollection* _Nullable colleciton;


- (BOOL)readCollectionFromURL:(NSURL* _Nonnull)url error:(NSError * _Nullable * _Nullable)outError;
- (BOOL)writeCollectionToURL:(NSURL* _Nonnull)url error:(NSError * _Nullable * _Nullable)outError;

- (BOOL)trainClassifierAtState:(NSString* _Nonnull)stateName completionHandle:(void (^_Nonnull)(ALTrainingState * _Nonnull))completionHandle error:(NSError * _Nullable * _Nullable)outError;

- (ALUtterance* _Nullable)classifier:(NSString* _Nonnull)stateName respondTo:(NSString* _Nonnull)question;

- (void)readDialogueManagerFromURL:(NSURL* _Nonnull)url completionHandle:(void (^_Nonnull)(NSString * _Nullable, NSError * _Nullable))completionHandle;


- (void)postEvent:(NSString* _Nonnull)eventName withData:(NSDictionary<NSString*, NSString*>* _Nonnull)data toMachine:(NSString* _Nonnull)machine;

- (void)setEngineEventHandler:(void (^_Nonnull)(ALPlatformEvent * _Nonnull))eventHandler;

- (ALUtterance* _Nullable)utteranceWithID:(NSString* _Nonnull)utteranceID;

- (void)collectionWasUpdated;
- (void)collectionWasUpdatedInState:(NSString* _Nonnull)stateName;

@end
