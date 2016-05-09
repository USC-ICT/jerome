//
//  jerome_osx_Tests.m
//  jerome.osx Tests
//
//  Created by Anton Leuski on 8/2/14.
//  Copyright (c) 2014 Anton Leuski & ICT/USC. All rights reserved.
//

#if defined(__Check)
#error "check"
#endif

#include <jerome/scripting/js_engine.hpp>
#include <jerome/npc/model_cpp.hpp>

#import <XCTest/XCTest.h>

@interface ScionTest : XCTestCase
@property (nonatomic, copy)	NSString*	name;
@property (nonatomic, copy)	NSString*	scxmlPath;
@property (nonatomic, copy)	NSString*	jsonPath;
@property (nonatomic, copy)	NSString*	scionPath;
@property (nonatomic, copy)	NSString*	jeromeSrcPath;
@end

@implementation ScionTest
@synthesize name = _name;

+ (XCTestSuite*)defaultTestSuite
{
	XCTestSuite*		testSuite		= [[XCTestSuite alloc] initWithName:NSStringFromClass(self)];
	
	NSString*			baseFolderPath = [[NSFileManager defaultManager] currentDirectoryPath];
//	NSString*			baseFolderPath = @"~/Documents/Projects/jerome/jerome";
//	baseFolderPath = [baseFolderPath stringByStandardizingPath];
	
	NSString*			scionFolderPath	= [[[baseFolderPath stringByDeletingLastPathComponent]
																	 stringByAppendingPathComponent:@"3rd_party"]
																	 stringByAppendingPathComponent:@"SCION"];
	NSLog(@"%@", scionFolderPath);
	
	NSString*			jeromeSrcFolderPath	= [[baseFolderPath stringByAppendingPathComponent:@"jerome"]
																		 stringByAppendingPathComponent:@"scripting"];
	NSLog(@"%@", jeromeSrcFolderPath);
	
	NSString*			testsFolderPath	= [[scionFolderPath stringByAppendingPathComponent:@"scxml-test-framework-master"]
																	 stringByAppendingPathComponent:@"test"];
	NSLog(@"%@", testsFolderPath);
	
	
	for(NSString* testFolderName in [[NSFileManager defaultManager] contentsOfDirectoryAtPath:testsFolderPath error:nil]) {
		BOOL		isdir;
		NSString*	testFolderPath	= [testsFolderPath stringByAppendingPathComponent:testFolderName];
		if (![[NSFileManager defaultManager] fileExistsAtPath:testFolderPath isDirectory:&isdir] || !isdir) continue;
		for(NSString* testScriptName in [[NSFileManager defaultManager] contentsOfDirectoryAtPath:testFolderPath error:nil]) {
			if (![testScriptName hasSuffix:@".scxml"]) continue;
			NSString*	scxmlPath	= [testFolderPath stringByAppendingPathComponent:testScriptName];
			NSString*	jsonPath	= [testFolderPath stringByAppendingPathComponent:[[testScriptName stringByDeletingPathExtension] stringByAppendingPathExtension:@"json"]];
			if (![[NSFileManager defaultManager] fileExistsAtPath:jsonPath]) continue;
			
			NSString*	name = [[scxmlPath substringFromIndex:testsFolderPath.length+1] stringByDeletingPathExtension];

			ScionTest*	test = [[ScionTest alloc] initWithSelector:@selector(test)];
			test.name = name;
			test.scxmlPath = [[NSURL fileURLWithPath:scxmlPath] absoluteString];
			test.jsonPath = [[NSURL fileURLWithPath:jsonPath] absoluteString];
			test.jeromeSrcPath = [[NSURL fileURLWithPath:jeromeSrcFolderPath] absoluteString];
			test.scionPath = [[NSURL fileURLWithPath:scionFolderPath] absoluteString];

			[testSuite addTest:test];
		}
	}
	
	return testSuite;
}

namespace js = jerome::javascript;

- (void)test
{
	jerome::scripting::Engine	engine((jerome::npc::Collection()));
	
	bool testsAreDone = NO;
	js::Context&	context(engine.context());
	
	engine.performBlock([&] {
		context.exceptionHandler() = [&](const js::Value& exception, const js::String& inExceptionContext) {
			testsAreDone = YES;
			XCTFail(@"JS exception %@ while %@", @((const char*)exception), @(inExceptionContext.c_str()));
		};
		
		context["scionTestsAreDone"] = [&] {
			testsAreDone = YES;
		};
	});
	
	NSString* scionMainScriptFilePath = [[self.scionPath stringByAppendingPathComponent:@"dist"]
																			 stringByAppendingPathComponent:@"scion.js"];
	NSString* startupScriptFilePath = [self.jeromeSrcPath stringByAppendingPathComponent:@"startup.js"];
	NSString* scionTestScriptFilePath = [self.jeromeSrcPath stringByAppendingPathComponent:@"scion.tests.js"];
	
	engine.evaluateScriptWithFilePath([scionMainScriptFilePath fileSystemRepresentation]);
	engine.evaluateScriptWithFilePath([startupScriptFilePath fileSystemRepresentation]);
	engine.evaluateScriptWithFilePath([scionTestScriptFilePath fileSystemRepresentation]);
	
	engine.performBlock([&] {
		
		engine.postEvent("foo", {{"one", "two"}, {"foo", "bar"} }, "");
		
		//		function testOneScionCase(name, scxmlFileName, eventsFileName,
		//								  failed, errored, passed, log)
		
		auto failed = [&] {
			XCTFail(@"failed: %@", @(jerome::scripting::Engine::argumentsDescription().c_str()));
			testsAreDone = true;
		};
		
		auto errored = [&] {
			XCTFail(@"errored: %@", @(jerome::scripting::Engine::argumentsDescription().c_str()));
			testsAreDone = true;
		};
		
		context["testOneScionCase"]([self.name UTF8String],
										  [self.scxmlPath UTF8String],
										  [self.jsonPath UTF8String],
										  failed,
										  errored,
										  [&] {testsAreDone = true;},
										  context["__scionPlatform"]["log"]
										  );
	});
	
	while (!testsAreDone) {
		[[NSRunLoop currentRunLoop] runUntilDate:[NSDate dateWithTimeIntervalSinceNow:0.01]];
	}
	
}

@end

@interface jerome_osx_Tests : XCTestCase

@end

@implementation jerome_osx_Tests

- (void)setUp
{
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown
{
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

struct A {
	void f() {}
	void initialize(JSContextRef ctx, JSObjectRef object);
	static void hasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName);
	static JSValueRef getProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName);
	static bool setProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception);
};

void A::initialize(JSContextRef ctx, JSObjectRef object)
{
	
}
void A::hasProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName)
{
	
}
JSValueRef A::getProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName)
{
	return nullptr;
}
bool A::setProperty(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exception)
{
	return false;
}

struct B{};

namespace jerome { namespace javascript {
	template <>
	struct ClassTraits<A> : public DefaultClassTraits<A> {};
}}

- (void)testConversionForClassWithTraits
{
	jerome::javascript::Context ctx;
	jerome::javascript::detail::to_value<A>::convert(ctx, A());
}

- (void)testCallable
{
	std::cout << jerome::is_callable<decltype(&A::f)>::value << std::endl;
	std::cout << jerome::is_callable<void(void)>::value << std::endl;
	std::cout << jerome::is_callable<void(*)(void)>::value << std::endl;
	std::cout << jerome::is_callable<void*>::value << std::endl;
	std::cout << jerome::is_callable<int>::value << std::endl;
	auto x = [](int x) -> bool { return true; };
	std::cout << jerome::is_callable<decltype(x)>::value << std::endl;
}

- (void)testParameterizedClass
{
	jerome::javascript::Context	ctx;
	jerome::javascript::Value		value = ctx.newObject();
	value.prototype() = ctx.newObject();
	value["a"] = ctx.newObject();
	value[0] = ctx.newObject();
	jerome::javascript::Value		a = value.prototype();
	a = value["a"];
	
	const jerome::javascript::Value	cvalue(value);
	a = cvalue["a"];

	a = value[0];
	a = value;
	a = 5;
	a = 0;
	a = 3.14;
	a = nullptr;
	a = "hello";
	value["a"] = "a";
	value["a"] = nullptr;
	value["a"] = [](int x) -> bool { return true; };
	value["a"] = [&](int x) -> bool { return true; };
	value["a"] = (bool (*)(int))([](int x) -> bool { return true; });
	value["a"] = (std::function<bool(int)>)([](int x) -> bool { return true; });
}

//- (void)testExample
//{
//	
//	using namespace jerome;
//	using namespace jerome::npc;
//
//	jerome::npc::cpp::ObjectFactory	of;
//	jerome::npc::Collection collection;
//	
//	NSURL*		srcRootURL	= [NSURL fileURLWithPath:@"/Users/leuski/Documents/Projects/jerome/jerome/jerome/scripting"];
//	NSURL*		scionRootURL= [NSURL fileURLWithPath:@"/Users/leuski/Documents/Projects/jerome/3rd_party/SCION"];
//	
//	NSURL*		scionURL	= [scionRootURL URLByAppendingPathComponent:@"dist/scion.js"];
//	NSString*	scionStr	= [NSString stringWithContentsOfURL:scionURL encoding:NSUTF8StringEncoding error:nil];
//	
//	static jerome::shared_ptr<jerome::scripting::Engine> theEngine(new jerome::scripting::Engine(collection));
//	theEngine->evaluateScript([scionStr UTF8String], [[scionURL absoluteString] UTF8String]);
//	
//	NSURL*		scion1URL	= [srcRootURL URLByAppendingPathComponent:@"startup.js"];
//	NSString*	scion1Str	= [NSString stringWithContentsOfURL:scion1URL encoding:NSUTF8StringEncoding error:nil];
//	
//	theEngine->evaluateScript([scion1Str UTF8String], [[scion1URL absoluteString] UTF8String]);
//	
//	NSURL*		scion2URL	= [srcRootURL URLByAppendingPathComponent:@"scion.tests.js"];
//	NSString*	scion2Str	= [NSString stringWithContentsOfURL:scion2URL encoding:NSUTF8StringEncoding error:nil];
//	
//	theEngine->evaluateScript([scion2Str UTF8String], [[scion2URL absoluteString] UTF8String]);
//	
//	//	NSURL*		testURL	= [NSURL URLWithString:@"file:///Users/leuski/Documents/Projects/mVH/3rd_party/SCION/test/surface-test.js"];
//	//	NSString*	testStr	= [NSString stringWithContentsOfURL:testURL encoding:NSUTF8StringEncoding error:nil];
//	//	theEngine.context().evaluateScript([testStr UTF8String], [[testURL absoluteString] UTF8String]);
//	
//	NSString*			testsFolderPath	= [[scionRootURL URLByAppendingPathComponent:@"scxml-test-framework-master/test"] path];
//	NSMutableString*	code	= [NSMutableString stringWithString:@"test(["];
//	int count = 0;
//	for(NSString* testFolderName in [[NSFileManager defaultManager] contentsOfDirectoryAtPath:testsFolderPath error:nil]) {
//		BOOL		isdir;
//		NSString*	testFolderPath	= [testsFolderPath stringByAppendingPathComponent:testFolderName];
//		if (![[NSFileManager defaultManager] fileExistsAtPath:testFolderPath isDirectory:&isdir] || !isdir) continue;
//		for(NSString* testScriptName in [[NSFileManager defaultManager] contentsOfDirectoryAtPath:testFolderPath error:nil]) {
//			if (![testScriptName hasSuffix:@".scxml"]) continue;
//			NSString*	scxmlPath	= [testFolderPath stringByAppendingPathComponent:testScriptName];
//			NSString*	jsonPath	= [testFolderPath stringByAppendingPathComponent:[[testScriptName stringByDeletingPathExtension] stringByAppendingPathExtension:@"json"]];
//			if (![[NSFileManager defaultManager] fileExistsAtPath:jsonPath]) continue;
//			if (count) {
//				[code appendString:@","];
//			}
//			[code appendFormat:@"[\"%@\", \"%@\"]", [[NSURL fileURLWithPath:scxmlPath] absoluteString], [[NSURL fileURLWithPath:jsonPath] absoluteString]]; count++;
//			//			NSLog(@"%@ and %@", [scxmlPath lastPathComponent], [jsonPath lastPathComponent]);
//		}
//	}
//	[code appendString:@"]);"];
//	
//	String	script([code UTF8String]);
//	theEngine->evaluateScript(script);
////	jerome::async::sharedInstance().strand().dispatch(boost::bind(&jerome::scripting::Engine::evaluateScript, theEngine, script, String(), 0, true));
//	
//	std::cout << "test started" << std::endl;
//	while (!testIsDone)
//		usleep(1000);
//	std::cout << "test done" << std::endl;
//
////    XCTFail(@"No implementation for \"%s\"", __PRETTY_FUNCTION__);
//}
//

@end
