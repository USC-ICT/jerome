//
//  ALPlatform.h
//  NPCEditor
//
//  Created by Anton Leuski on 10/20/15.
//  Copyright © 2015 Anton Leuski. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Jerome/model.h>

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALPlatform : NSObject
@property(nonatomic, strong, readonly) ALCollection* _Nullable colleciton;


- (BOOL)readCollectionFromURL:(NSURL* _Nonnull)url
                        error:(NSError * _Nullable * _Nullable)outError;

- (BOOL)writeCollectionToURL:(NSURL* _Nonnull)url
                       error:(NSError * _Nullable * _Nullable)outError;

- (ALUtterance* _Nullable)classifier:(NSString* _Nonnull)stateName
                           respondTo:(NSString* _Nonnull)question;

- (void)readDialogueManagerFromURL:(NSURL* _Nonnull)url
                  completionHandle:(ALDialoguScriptLoadedHandled _Nonnull)completionHandle;

- (void)postEvent:(NSString* _Nonnull)eventName
         withData:(NSDictionary<NSString*, NSString*>* _Nonnull)data
        toMachine:(NSString* _Nonnull)machine;

- (void)setEngineEventHandler:(ALEngineEventHandler _Nonnull)eventHandler;

- (ALUtterance* _Nullable)utteranceWithID:(NSString* _Nonnull)utteranceID;

- (void)collectionWasUpdated;
- (void)collectionWasUpdatedInState:(NSString* _Nonnull)stateName;

@end
