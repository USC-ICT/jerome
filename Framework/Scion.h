//
//  Scion.h
//  jerome
//
//  Created by Anton Leuski on 5/10/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <JavaScriptCore/JavaScriptCore.h>
#include <libxml/tree.h>

@class ALXMLNode;
@class ALXMLDoc;

@protocol ALXMLDocProtocol <JSExport>
@property (nonatomic, strong, readonly) ALXMLNode* documentElement;
@end

@protocol ALURLProtocol <JSExport>
- (NSString*)getPathFromUrl:(NSString*)url;
- (NSString*)changeUrlPath:(NSString*)url :(NSString*)newPath;
@end

@protocol ALDomProtocol <JSExport>
- (NSArray*)getChildren:(ALXMLNode*)node;
- (NSString*)localName:(ALXMLNode*)node;
- (NSString*)getAttribute:(ALXMLNode*)node :(NSString*)attribute;
- (BOOL)hasAttribute:(ALXMLNode*)node :(NSString*)attribute;
- (NSString*)namespaceURI:(ALXMLNode*)node;
- (ALXMLNode*)createElementNS:(ALXMLDoc*)doc :(NSString*)ns :(NSString*)localName;
- (void) setAttribute:(ALXMLNode*) node :(NSString*)name :(NSString*)value;
- (ALXMLNode*) appendChild:(ALXMLNode*)parent :(ALXMLNode*)child;
- (NSString*)textContent:(ALXMLNode*)node :(NSString*)txt;
- (NSArray*)getElementChildren:(ALXMLNode*)node;
@end

@protocol ALPathProtocol <JSExport>
- (NSString*)join:(NSString*)path1 :(NSString*)path2;
- (NSString*)dirname:(NSString*)path;
- (NSString*)basename:(NSString*)path :(NSString*)ext;
- (NSString*)extname:(NSString*)path;
@end

@protocol ALScionPlatformProtocol <JSExport>
@property (nonatomic, strong, readonly) id<ALURLProtocol> url;
@property (nonatomic, strong, readonly) id<ALDomProtocol> dom;
@property (nonatomic, strong, readonly) id<ALPathProtocol> path;

- (void)getDocumentFromUrl:(NSString*)URL :(JSValue*)cb;
- (ALXMLDoc*)parseDocumentFromString:(NSString*)content;
- (void)getDocumentFromFilesystem:(NSString*)path :(JSValue*)cb;
- (void)getResourceFromUrl:(NSString*)url :(JSValue*)cb;
//- (void)postDataToUrl:(NSString*)url :(ALStringRsrcCallback)cb;
- (uint32_t)setTimeout:(JSValue*)cb :(double)timeout;
- (void)clearTimeout:(uint32_t)timeoutID;
- (void)log;
- (JSValue*)eval:(NSString*)content :(NSString*)name;
@end

@interface ALScionPlatform : NSObject <ALScionPlatformProtocol>
+ (instancetype)scionPlatformWithQueue:(dispatch_queue_t)queue;
@end
