#! /bin/sh

src_dir=$1
dst_dir=$2

: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}
: ${IPHONE_SDKVERSION:=`xcodebuild -showsdks | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
: ${OSX_SDKVERSION:=`xcodebuild -showsdks | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}

abort()
{
    echo
    echo "Aborted:" "$@"
    exit 1
}


echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

: ${BUILD_UIKIT_FOR_MAC:=`echo "${IPHONE_SDKVERSION} >= 13.0" | bc`}

if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
then
  echo "build macosx-maccatalyst on"
  platform_names="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx macosx-maccatalyst"
else
  echo "build macosx-maccatalyst off"
  platform_names="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx"
fi

found_nlopt="YES"
for platform_name in ${platform_names}
do
  if [ ! -f "${dst_dir}/${platform_name}/lib/libnlopt_cxx.a" ]
  then
    found_nlopt="NO"
  fi
done

if [ "${found_nlopt}" = "YES" ]
then
  echo "Found nlopt binary"
  exit
fi

pushd "${src_dir}"

tar -xzf nlopt-2.3.tar.gz

: ${XCODE_ROOT:=`xcode-select -print-path`}
: ${EXTRA_CFLAGS:="-fvisibility=hidden -fvisibility-inlines-hidden -w -g -DNDEBUG -Os"}
: ${EXTRA_CPPFLAGS:="${EXTRA_CFLAGS} -std=${CLANG_CXX_LANGUAGE_STANDARD} -stdlib=${CLANG_CXX_LIBRARY}"}

BASE_PREFIX="${dst_dir}"
TOOLCHAIN="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain"

configure_and_make () {

  PREFIX="$1"
  ARCHS_LIST="$2"
  PLATFORM="$3"
  SDK="$4"
  EXTRA_FLAGS="$5"
  EXTRA_CONFIG="$6"

  SYSRT="$XCODE_ROOT/Platforms/${PLATFORM}.platform/Developer/SDKs/${PLATFORM}${SDK}.sdk"

  export CC="$TOOLCHAIN/usr/bin/clang"
  export CXX="$TOOLCHAIN/usr/bin/clang++"
  export CPP="$CC -E"
  export CFLAGS=" -Os ${ARCHS_LIST} -isysroot $SYSRT ${EXTRA_FLAGS} $EXTRA_CFLAGS"
  export CXXFLAGS="$CFLAGS -std=${CLANG_CXX_LANGUAGE_STANDARD} -stdlib=${CLANG_CXX_LIBRARY}"
  
  echo "CXXFLAGS=${CXXFLAGS}"
  
cat <<EOF
./configure --prefix="$PREFIX" ${EXTRA_CONFIG} 
--disable-shared --enable-static --with-cxx --without-guile --without-python 
--without-octave --without-matlab
EOF

  ./configure --prefix="$PREFIX" ${EXTRA_CONFIG} \
    --disable-shared --enable-static --with-cxx --without-guile --without-python \
    --without-octave --without-matlab || abort "cofigure failed"

  make clean
  make -j 16 || abort "make failed"
  make install || abort "make install failed"
  make clean

}

pushd nlopt-2.3

if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
then

if [ `echo "${MACOSX_DEPLOYMENT_TARGET} >= 10.15" | bc` -eq 1 ]
then
  UIKIT_DEPLOYMENT_TARGET="${MACOSX_DEPLOYMENT_TARGET}"
else
  UIKIT_DEPLOYMENT_TARGET="10.15"
fi

if [ `echo "${IPHONEOS_DEPLOYMENT_TARGET} >= 13.0" | bc` -eq 1 ]
then
  UIKIT_IOS_DEPLOYMENT_TARGET="${IPHONEOS_DEPLOYMENT_TARGET}"
else
  UIKIT_IOS_DEPLOYMENT_TARGET="13.0"
fi

configure_and_make "${BASE_PREFIX}/macosx-maccatalyst" "-arch arm64 -target arm64-apple-ios${UIKIT_IOS_DEPLOYMENT_TARGET}-macabi" \
  "MacOSX" ${OSX_SDKVERSION} " " \
  "--host=arm-apple-darwin"
  
configure_and_make "${BASE_PREFIX}/macosx-maccatalyst/x86" "-arch x86_64 -target x86_64-apple-ios${UIKIT_IOS_DEPLOYMENT_TARGET}-macabi" \
  "MacOSX" ${OSX_SDKVERSION} " " \
  "--host=x86-apple-darwin"

mv "${BASE_PREFIX}/macosx-maccatalyst/lib/libnlopt_cxx.a"   "${BASE_PREFIX}/macosx-maccatalyst/lib/libnlopt_cxx_arm.a"
lipo -create "${BASE_PREFIX}/macosx-maccatalyst/lib/libnlopt_cxx_arm.a" \
   "${BASE_PREFIX}/macosx-maccatalyst/x86/lib/libnlopt_cxx.a"   \
   -o "${BASE_PREFIX}/macosx-maccatalyst/lib/libnlopt_cxx.a"
rm -rf "${BASE_PREFIX}/macosx-maccatalyst/x86"
rm -f "${BASE_PREFIX}/macosx-maccatalyst/lib/libnlopt_cxx_arm.a"
fi

configure_and_make "${BASE_PREFIX}/iphoneos-iphoneos" "-arch arm64" \
  "iPhoneOS" ${IPHONE_SDKVERSION} "-mios-version-min=${IPHONEOS_DEPLOYMENT_TARGET}" \
  "--host=arm-apple-darwin"

configure_and_make "${BASE_PREFIX}/iphonesimulator-iphonesimulator" "-arch arm64 -arch x86_64" \
  "iPhoneSimulator" ${IPHONE_SDKVERSION} "-mios-version-min=${IPHONEOS_DEPLOYMENT_TARGET}" \
  "--host=arm-apple-darwin"

configure_and_make "${BASE_PREFIX}/macosx" "-arch x86_64 -arch arm64" \
  "MacOSX" ${OSX_SDKVERSION} "-mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}" \
  ""

popd
rm -rf nlopt-2.3

popd

#
# configure_and_make "x86" "-arch x86_64" \
#   "iPhoneSimulator" ${IPHONE_SDKVERSION} "-mios-version-min=${IPHONE_SDKVERSION}" \
#   ""

# cp $BASE_PREFIX/arm/include/* $BASE_PREFIX/../include/
# $TOOLCHAIN/usr/bin/lipo -create -output $BASE_PREFIX/../lib/libnlopt_cxx.a $BASE_PREFIX/arm/lib/libnlopt_cxx.a $BASE_PREFIX/x86/lib/libnlopt_cxx.a
