#! /bin/sh

src_dir=$1
dst_dir=$2

: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}
: ${SDKS:=`xcodebuild -showsdks`}
: ${IPHONE_SDKVERSION:=`echo "${SDKS}" | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
: ${OSX_SDKVERSION:=`echo "${SDKS}" | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}

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

  local PREFIX="$1"
  local ARCHS_LIST="$2"
  local PLATFORM="$3"
  local SDK="$4"
  local EXTRA_FLAGS="$5"
  local EXTRA_CONFIG="$6"

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

  make clean

  ./configure --prefix="$PREFIX" ${EXTRA_CONFIG} \
    --disable-shared --enable-static --with-cxx --without-guile --without-python \
    --without-octave --without-matlab || abort "cofigure failed"

  make -j 16 || abort "make failed"
  make install || abort "make install failed"

}

configure_and_make_fat() {
  local PREFIX="$1"
  local SYS="$2"
  local PLATFORM="$3"
  local SDK="$4"
  local EXTRA_FLAGS="$5"
  local SECONDARY_PREFIX="${PREFIX}/arm"
  local tmp_FILE="${PREFIX}/lib/libnlopt_cxx_tmp.a"
  
  configure_and_make "${SECONDARY_PREFIX}" "-arch arm64 -target arm64-apple-${SYS}" \
    ${PLATFORM} ${SDK} "${EXTRA_FLAGS}" \
    "--host=arm-apple-darwin"
  
  configure_and_make "${PREFIX}" "-arch x86_64 -target x86_64-apple-${SYS}" \
    ${PLATFORM} ${SDK} "${EXTRA_FLAGS}" \
    "--host=x86-apple-darwin"

  mv "${PREFIX}/lib/libnlopt_cxx.a" "${tmp_FILE}"
  lipo -create "${tmp_FILE}" "${SECONDARY_PREFIX}/lib/libnlopt_cxx.a"   \
     -o "${PREFIX}/lib/libnlopt_cxx.a"
  rm -rf "${SECONDARY_PREFIX}"
  rm -f "${tmp_FILE}"
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

configure_and_make_fat "${BASE_PREFIX}/macosx-maccatalyst" "ios${UIKIT_IOS_DEPLOYMENT_TARGET}-macabi" \
  "MacOSX" ${OSX_SDKVERSION} " " 
fi

configure_and_make "${BASE_PREFIX}/iphoneos-iphoneos" "-arch arm64" \
  "iPhoneOS" ${IPHONE_SDKVERSION} "-mios-version-min=${IPHONEOS_DEPLOYMENT_TARGET}" \
  "--host=arm-apple-darwin"

configure_and_make_fat "${BASE_PREFIX}/iphonesimulator-iphonesimulator" "ios${IPHONEOS_DEPLOYMENT_TARGET}-simulator" \
  "iPhoneSimulator" ${IPHONE_SDKVERSION} "-mios-version-min=${IPHONEOS_DEPLOYMENT_TARGET}" 

configure_and_make_fat "${BASE_PREFIX}/macosx" "macos${MACOSX_DEPLOYMENT_TARGET}" \
  "MacOSX" ${OSX_SDKVERSION} "-mmacosx-version-min=${MACOSX_DEPLOYMENT_TARGET}" 

popd
rm -rf nlopt-2.3

popd

#
# configure_and_make "x86" "-arch x86_64" \
#   "iPhoneSimulator" ${IPHONE_SDKVERSION} "-mios-version-min=${IPHONE_SDKVERSION}" \
#   ""

# cp $BASE_PREFIX/arm/include/* $BASE_PREFIX/../include/
# $TOOLCHAIN/usr/bin/lipo -create -output $BASE_PREFIX/../lib/libnlopt_cxx.a $BASE_PREFIX/arm/lib/libnlopt_cxx.a $BASE_PREFIX/x86/lib/libnlopt_cxx.a
