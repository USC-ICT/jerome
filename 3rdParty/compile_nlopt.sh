#! /bin/sh

# Current iPhone SDK
: ${IPHONE_SDKVERSION:=`xcodebuild -showsdks | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
# Specific iPhone SDK
# : ${IPHONE_SDKVERSION:=8.1}

# Current OSX SDK
: ${OSX_SDKVERSION:=`xcodebuild -showsdks | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
# Specific OSX SDK
# : ${OSX_SDKVERSION:=10.10}

: ${XCODE_ROOT:=`xcode-select -print-path`}
: ${EXTRA_CFLAGS:="-fvisibility=hidden -fvisibility-inlines-hidden  -g -DNDEBUG -Os"}
: ${EXTRA_CPPFLAGS:="${EXTRA_CFLAGS} -std=c++11 -stdlib=libc++"}


BASE_PREFIX=`pwd`
BASE_PREFIX="$BASE_PREFIX/../../build/ios/tmp/"
TOOLCHAIN="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain"

cd nlopt-2.3

configure_and_make () {

	BUILD_DIR_NAME="$1"
	ARCHS_LIST="$2"
	PLATFORM="$3"
	SDK="$4"
	EXTRA_FLAGS="$5"
	EXTRA_CONFIG="$6"

	SYSRT="$XCODE_ROOT/Platforms/${PLATFORM}.platform/Developer/SDKs/${PLATFORM}${SDK}.sdk"

	export CC="$TOOLCHAIN/usr/bin/clang"
	export CXX="$TOOLCHAIN/usr/bin/clang++"
	export CPP="$CC -E"
	export CFLAGS="$ARCHS_LIST -isysroot $SYSRT ${EXTRA_FLAGS} $EXTRA_CFLAGS"
	export CXXFLAGS="$CFLAGS -std=c++11 -stdlib=libc++"

	PREFIX="$BASE_PREFIX/$BUILD_DIR_NAME"

	./configure --prefix="$PREFIX" ${EXTRA_CONFIG} \
		--disable-shared --enable-static --with-cxx --without-guile --without-python \
		--without-octave --without-matlab 

	make clean
	make -j 16
	make install

}

# configure_and_make "arm" "-arch armv7 -arch armv7s -arch arm64" \
# 	"iPhoneOS" ${IPHONE_SDKVERSION} "-miphoneos-version-min=${IPHONE_SDKVERSION}" \
# 	"--host=arm-apple-darwin --target=arm-apple-darwin"
# 
# configure_and_make "x86" "-arch x86_64" \
# 	"iPhoneSimulator" ${IPHONE_SDKVERSION} "-miphoneos-version-min=${IPHONE_SDKVERSION}" \
# 	""

cp $BASE_PREFIX/arm/include/* $BASE_PREFIX/../include/
$TOOLCHAIN/usr/bin/lipo -create -output $BASE_PREFIX/../lib/libnlopt_cxx.a $BASE_PREFIX/arm/lib/libnlopt_cxx.a $BASE_PREFIX/x86/lib/libnlopt_cxx.a