//
//  ALChatEngine.h
//  Twins
//
//  Created by Anton Leuski on 2/9/13.
//  Copyright (c) 2013 Anton Leuski. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol ALChatEngineLoadingDelegate;
@protocol ALChatEngineDisplayDelegate;

@interface ALChatEngine : NSObject
@property (nonatomic, weak) id<ALChatEngineLoadingDelegate>	loadingDelegate;
@property (nonatomic, weak) id<ALChatEngineDisplayDelegate>	displayDelegate;
- (void)load;
- (void)answerUtterance:(NSString*)text;
@end

@protocol ALChatEngineLoadingDelegate <NSObject>
- (void)chatEngine:(ALChatEngine*)chatEngine loadingStatusDidChange:(NSString*)status;
- (void)chatEngine:(ALChatEngine*)chatEngine loadDidCompleteWithError:(NSError*)error;
@end

typedef void(^ALSendUtteranceCompletionBlock)(void);

@protocol ALChatEngineDisplayDelegate <NSObject>
- (void)chatEngine:(ALChatEngine*)chatEngine speaker:(NSString*)speaker says:(NSString*)text;
- (void)chatEngine:(ALChatEngine*)chatEngine didSendUtterance:(NSDictionary*)utterance completionHandler:(ALSendUtteranceCompletionBlock)completionHandler;
@end
