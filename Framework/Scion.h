//
//  Scion.h
//  jerome
//
//  Created by Anton Leuski on 5/10/16.
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

#import <Foundation/Foundation.h>
#import <JavaScriptCore/JavaScriptCore.h>
#include <libxml/tree.h>

@class ALXMLNode;
@class ALXMLDoc;

typedef void(^ALLogHandlerBlock)(NSArray<NSObject*>* _Nonnull);

@protocol ALXMLDocProtocol <JSExport>
@property (nonatomic, strong, readonly) ALXMLNode* _Nonnull documentElement;
@end

@protocol ALURLProtocol <JSExport>
- (NSString* _Nullable)getPathFromUrl:(NSString* _Nonnull)url;
- (NSString* _Nullable)changeUrlPath:(NSString* _Nonnull)url
                                    :(NSString* _Nonnull)newPath;
@end

@protocol ALDomProtocol <JSExport>
- (NSArray* _Nonnull)getChildren:(ALXMLNode* _Nonnull)node;
- (NSString* _Nullable)localName:(ALXMLNode* _Nonnull)node;
- (NSString* _Nullable)getAttribute:(ALXMLNode* _Nonnull)node
                                   :(NSString* _Nonnull)attribute;
- (BOOL)hasAttribute:(ALXMLNode* _Nonnull)node :(NSString* _Nonnull)attribute;
- (NSString* _Nullable)namespaceURI:(ALXMLNode* _Nonnull)node;
- (ALXMLNode* _Nonnull)createElementNS:(ALXMLDoc* _Nonnull)doc
                                      :(NSString* _Nullable)ns
                                      :(NSString* _Nonnull)localName;
- (void)setAttribute:(ALXMLNode* _Nonnull)node
                    :(NSString* _Nonnull)name
                    :(NSString* _Nonnull)value;
- (ALXMLNode* _Nonnull)appendChild:(ALXMLNode* _Nonnull)parent
                                  :(ALXMLNode* _Nonnull)child;
- (NSString* _Nullable)textContent:(ALXMLNode* _Nonnull)node
                                  :(NSString* _Nullable)txt;
- (NSArray* _Nonnull)getElementChildren:(ALXMLNode* _Nonnull)node;
@end

@protocol ALPathProtocol <JSExport>
- (NSString* _Nonnull)join:(NSString* _Nonnull)path1
                          :(NSString* _Nonnull)path2;
- (NSString* _Nonnull)dirname:(NSString* _Nonnull)path;
- (NSString* _Nonnull)basename:(NSString* _Nonnull)path
                              :(NSString* _Nullable)ext;
- (NSString* _Nullable)extname:(NSString* _Nonnull)path;
@end

@protocol ALScionPlatformProtocol <JSExport>
@property (nonatomic, strong, readonly) id<ALURLProtocol> _Nonnull url;
@property (nonatomic, strong, readonly) id<ALDomProtocol> _Nonnull dom;
@property (nonatomic, strong, readonly) id<ALPathProtocol> _Nonnull path;

- (void)getDocumentFromUrl:(NSString* _Nonnull)URL
                          :(JSValue* _Nonnull)cb;
- (ALXMLDoc* _Nonnull)parseDocumentFromString:(NSString* _Nonnull)content;
- (void)getDocumentFromFilesystem:(NSString* _Nonnull)path
                                 :(JSValue* _Nonnull)cb;
- (void)getResourceFromUrl:(NSString* _Nonnull)url
                          :(JSValue* _Nonnull)cb;
//- (void)postDataToUrl:(NSString*)url :(ALStringRsrcCallback)cb;
- (uint32_t)setTimeout:(JSValue* _Nonnull)cb :(double)timeout;
- (void)clearTimeout:(uint32_t)timeoutID;
- (void)log;
- (JSValue* _Nullable)eval:(NSString* _Nonnull)content
                          :(NSString* _Nonnull)name;
@end

@interface ALScionPlatform : NSObject <ALScionPlatformProtocol>
+ (instancetype _Nonnull)scionPlatformWithQueue:(dispatch_queue_t _Nonnull)queue
                                            log:(ALLogHandlerBlock _Nonnull)log;
@end
