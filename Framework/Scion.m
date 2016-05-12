//
//  Scion.m
//  jerome
//
//  Created by Anton Leuski on 5/10/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#import "Scion.h"

@interface NSString (xmlString)
@property (nonatomic, assign, readonly) const xmlChar* xmlString;
@end

@implementation NSString (xmlString)
- (const xmlChar*)xmlString
{
  return (xmlChar*)self.UTF8String;
}
@end


@interface ALXMLDoc : NSObject <ALXMLDocProtocol>
@property (nonatomic, assign) xmlDocPtr ptr;
@end

@interface ALXMLNode : NSObject
@property (nonatomic, assign) xmlNodePtr ptr;
@end

@implementation ALXMLNode
+ (instancetype)nodeWithPtr:(xmlNodePtr)ptr
{
  return [[ALXMLNode alloc] initWithPtr:ptr];
}

- (instancetype)initWithPtr:(xmlNodePtr)ptr
{
  if (self = [super init]) {
    self.ptr = ptr;
  }
  return self;
}
@end

@implementation ALXMLDoc

- (ALXMLNode*)documentElement
{
  return [ALXMLNode nodeWithPtr:xmlDocGetRootElement(self.ptr)];
}

- (instancetype)initWithContentsAtPath:(NSString*)path
{
  if (self = [super init]) {
    self.ptr	= xmlReadFile(path.fileSystemRepresentation, NULL, 0);
    if (self.ptr == NULL) {
      self = nil;
    }
  }
  return self;
}

+ (instancetype)documentWithContentsAtPath:(NSString*)path
{
  return [[ALXMLDoc alloc] initWithContentsAtPath:path];
}

- (instancetype)initWithData:(NSData*)data name:(NSString*)name
{
  if (self = [super init]) {
    if (!data) {
      return (self = nil);
    }
    self.ptr	= xmlReadMemory(data.bytes, (int)data.length,
                              name ? name.UTF8String : "noname.xml", NULL, 0);
    if (self.ptr == NULL) {
      self = nil;
    }
  }
  return self;
}

+ (instancetype)documentWithData:(NSData*)data name:(NSString*)name
{
  return [[ALXMLDoc alloc] initWithData:data name:name];
}

+ (instancetype)documentWithString:(NSString*)string name:(NSString*)name
{
  if (!string) return nil;
  NSData* data = [string dataUsingEncoding:NSUTF8StringEncoding];
  return [[ALXMLDoc alloc] initWithData:data name:name];
}

- (void)dealloc
{
  if (self.ptr) {
    xmlFreeDoc(self.ptr);
    self.ptr = NULL;
  }
}

@end


@interface ALURL : NSObject <ALURLProtocol>
@end

@implementation ALURL

- (NSString*)getPathFromUrl:(NSString*)url
{
  NSURL* theURL = [NSURL URLWithString:url];
  return [theURL path];
}

- (NSString*)changeUrlPath:(NSString*)url :(NSString*)newPath
{
  //  NSURL* theURL = [NSURL URLWithString:url];
  NSAssert(NO, @"changeUrlPath: not implemented");
  return nil;
}

@end

@interface ALDom : NSObject <ALDomProtocol>
@end

@implementation ALDom

- (NSArray*)getChildren:(ALXMLNode*)node
{
  NSMutableArray* result = [NSMutableArray new];
  for(xmlNodePtr aNode = node.ptr->children; aNode; aNode = aNode->next) {
    [result addObject:[ALXMLNode nodeWithPtr:aNode]];
  }
  return result;
}

- (NSString*)localName:(ALXMLNode*)node
{
  return node.ptr->name
    ? [NSString stringWithUTF8String:(const char*)node.ptr->name] : nil;
}

- (NSString*)getAttribute:(ALXMLNode*)node :(NSString*)attribute
{
  const char* value =
    (const char*)xmlGetProp(node.ptr, attribute.xmlString);
  return value ? [NSString stringWithUTF8String:value] : nil;
}

- (BOOL)hasAttribute:(ALXMLNode*)node :(NSString*)attribute
{
  return NULL != xmlGetProp(node.ptr, attribute.xmlString);
}

- (NSString*)namespaceURI:(ALXMLNode*)node
{
  const char* value = (const char*)(node.ptr->ns ? node.ptr->ns->href : NULL);
  return value ? [NSString stringWithUTF8String:value] : nil;
}

- (ALXMLNode*)createElementNS:(ALXMLDoc*)doc :(NSString*)ns :(NSString*)localName
{
  xmlNsPtr	theNs	= NULL;
  if (ns) {
    theNs	= xmlSearchNsByHref(doc.ptr, xmlDocGetRootElement(doc.ptr), (xmlChar*)ns.UTF8String);
    if (!theNs) {
      NSLog(@"Cannot find prefix for %@", ns);
      //						theNs	= xmlNewNs(xmlDocGetRootElement(theDoc), (xmlChar*)nsURL, NULL);
    }
  }
  return [ALXMLNode nodeWithPtr:xmlNewNode(theNs, localName.xmlString)];
}

- (void) setAttribute:(ALXMLNode*) node :(NSString*)name :(NSString*)value
{
  xmlNewProp(node.ptr, name.xmlString, value.xmlString);
}

- (ALXMLNode*) appendChild:(ALXMLNode*)parent :(ALXMLNode*)child
{
  xmlAddChild(parent.ptr, child.ptr);
  return child;
}

static int elementIndex(xmlNodePtr node) {
  int index = 1;
  for(xmlNodePtr n = node; n != node->parent->children; n = n->prev) {
				if (n != node && 0 == strcmp((const char*)node->name, (const char*)n->name))
          index++;
  }
  return index;
}

static NSString* xpathToNode(xmlNodePtr node) {
  if (!node->parent)
				return [NSString string];
  if (0 == strcmp((const char*)node->name, "scxml")) {
				return [NSString stringWithFormat:@"/scxml[@name='%s']",
                (const char*)xmlGetProp(node, (const xmlChar*)"name")];
  }
  return [NSString stringWithFormat:@"%@/%s[%d]", xpathToNode(node->parent),
          (const char*)node->name, elementIndex(node)];
}

- (NSString*)textContent:(ALXMLNode*)node :(NSString*)txt
{
  if ([JSContext currentArguments].count >= 2) {
    xmlNodeSetContent(node.ptr, (const xmlChar*)"");
    xmlNodeAddContent(node.ptr, txt.xmlString);
    return txt;
  } else {
    const char*	theContent	= (const char*)xmlNodeGetContent(node.ptr);
    NSString*	content = theContent
      ? [NSString stringWithUTF8String:theContent] : [NSString string];
    xmlFree((void*)theContent);
    
    if (0 == strcmp((const char*)node.ptr->name, "script")) {
      NSString* xpath = xpathToNode(node.ptr);
      content = [NSString stringWithFormat:@"\n/* %@/begin */\n%@\n/* %@/end */\n",
                 xpath, content, xpath];
    }
    return content;
  }
  
}

- (NSArray*)getElementChildren:(ALXMLNode*)node
{
  NSMutableArray* result = [NSMutableArray new];
  for(xmlNodePtr aNode = node.ptr->children; aNode; aNode = aNode->next) {
    if (aNode->type == XML_ELEMENT_NODE)
      [result addObject:[ALXMLNode nodeWithPtr:aNode]];
  }
  return result;

}

@end

@interface ALPath : NSObject <ALPathProtocol>
@end

@implementation ALPath

- (NSString*)join:(NSString*)path1 :(NSString*)path2
{
  return [path1 stringByAppendingPathComponent:path2];
}

- (NSString*)dirname:(NSString*)path
{
  return [path stringByDeletingLastPathComponent];
}

- (NSString*)basename:(NSString*)path :(NSString*)ext
{
  NSString* result = [path lastPathComponent];
  if (ext && [result hasSuffix:[NSString stringWithFormat:@".%@", ext]]) {
    result = [result stringByDeletingPathExtension];
  }
  return result;
}

- (NSString*)extname:(NSString*)path
{
  return [path pathExtension];
}

@end

@interface ALScionPlatform()
@property (nonatomic, strong) id<ALURLProtocol> url;
@property (nonatomic, strong) id<ALDomProtocol> dom;
@property (nonatomic, strong) id<ALPathProtocol> path;
@property (nonatomic, strong) dispatch_queue_t queue;
@property (nonatomic, strong) NSMutableDictionary* timers;
@property (nonatomic, assign) uint32_t timerIDCount;
@end

@implementation ALScionPlatform

+ (instancetype)scionPlatformWithQueue:(dispatch_queue_t)queue
{
  return [[self alloc] initWithQueue:queue];
}

- (instancetype)initWithQueue:(dispatch_queue_t)queue
{
  if (self = [super init]) {
    self.timers = [NSMutableDictionary new];
    self.timerIDCount = 0;
    self.queue = queue;
    self.url = [ALURL new];
    self.dom = [ALDom new];
    self.path = [ALPath new];
  }
  return self;
}

//typedef void (^ALXMLDocCallback)(NSString* error, ALXMLDoc* doc);
//typedef void (^ALStringRsrcCallback)(NSString* error, NSString* doc);
//typedef void (^ALTimerCallback)();

- (void)getDocumentFromUrl:(NSString*)URL :(JSValue*)cb
{
  NSURL* theURL = [NSURL URLWithString:URL];
  NSData* data = [NSData dataWithContentsOfURL:theURL];
  ALXMLDoc* doc = [ALXMLDoc documentWithData:data name:nil];
  if (doc) {
    [cb callWithArguments:@[[NSNull null], doc]];
  } else {
    [cb callWithArguments:@[@"failed to parse XML document", [NSNull null]]];
  }
}

- (ALXMLDoc*)parseDocumentFromString:(NSString*)content
{
  return [ALXMLDoc documentWithString:content name:nil];
}

- (void)getDocumentFromFilesystem:(NSString*)path :(JSValue*)cb
{
  ALXMLDoc* doc = [ALXMLDoc documentWithContentsAtPath:path];
  if (doc) {
    [cb callWithArguments:@[[NSNull null], doc]];
  } else {
    [cb callWithArguments:@[@"failed to read and parse XML document", [NSNull null]]];
  }
}

- (void)getResourceFromUrl:(NSString*)url :(JSValue*)cb
{
  NSURL* actualURL = [NSURL URLWithString:url];
  NSStringEncoding enc;
  NSError* error;
  NSString* result = [NSString stringWithContentsOfURL:actualURL
                                          usedEncoding:&enc
                                                 error:&error];
  if (result) {
    [cb callWithArguments:@[[NSNull null], result]];
  } else {
    [cb callWithArguments:@[error.localizedDescription
                            ? error.localizedDescription
                            : @"failed to load XML resource", [NSNull null]]];
  }
}

//- (void)postDataToUrl:(NSString*)url :(ALStringRsrcCallback)cb;

- (void)_clearTimeout:(uint32_t)timeoutID
{
  dispatch_source_t timer = self.timers[@(timeoutID)];
  if (timer) {
    [self.timers removeObjectForKey:@(timeoutID)];
    dispatch_source_cancel(timer);
  }
}

- (uint32_t)setTimeout:(JSValue*)cb :(double)timeoutMilliSeconds
{
  dispatch_source_t timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER,
                                                   0, 0, self.queue);

  uint32_t identifier;

  while (YES) {
    if (self.timerIDCount == UINT32_MAX) {
      self.timerIDCount = 0;
    } else {
      self.timerIDCount +=1;
    }
    identifier = self.timerIDCount;
    if (!self.timers[@(identifier)]) {
      self.timers[@(identifier)] = timer;
      break;
    }
  }
  
  int64_t interval = timeoutMilliSeconds * NSEC_PER_MSEC;
  
  dispatch_source_set_timer(timer,
                            dispatch_time(DISPATCH_TIME_NOW, interval),
                            interval, 0);
  
  __weak ALScionPlatform* weakSelf = self;
  JSManagedValue* retainedCB = [JSManagedValue managedValueWithValue:cb];
  
  dispatch_source_set_event_handler(timer, ^{
    NSLog(@"calling timer callback");
    [retainedCB.value callWithArguments:[NSArray array]];
    [weakSelf _clearTimeout:identifier];
  });
  
  dispatch_resume(timer);

  return identifier;
}

- (void)clearTimeout:(uint32_t)timeoutID
{
  [self _clearTimeout:timeoutID];
}

- (void)log
{
  NSLog(@"%@", [JSContext currentArguments]);
}

- (JSValue*)eval:(NSString*)content :(NSString*)name
{
  NSString* src = [NSString stringWithFormat:@"(function(){\nreturn %@;})();", content];
  return [[JSContext currentContext] evaluateScript:src
                                      withSourceURL:[NSURL fileURLWithPath:name]];
}

@end
