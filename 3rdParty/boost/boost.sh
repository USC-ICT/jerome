#===============================================================================
# Filename:  boost.sh
# Author:    Pete Goodliffe
# Copyright: (c) Copyright 2009 Pete Goodliffe
# Licence:   Please feel free to use this, with attribution
#===============================================================================
# Modified by Anton Leuski
#===============================================================================
# Builds a Boost framework for the iPhone, iPhone Simulator, and OSX.
# Creates a set of universal libraries that can be used on an iPhone and in the
# iPhone simulator. Then creates a pseudo-framework to make using boost in Xcode
# less painful.
#
# To configure the script, define:
#    BOOST_LIBS:        which libraries to build
#    IPHONE_SDKVERSION: iPhone SDK version (e.g. 8.1)
#    OSX_SDKVERSION: OSX SDK version (e.g. 10.10)
#
# Then go get the source tar.bz of the boost you want to build, shove it in the
# same directory as this script, and run "./boost.sh". Grab a cuppa. And voila.
#===============================================================================

TARGET_DIR="$1"

: ${THE_BOOST_VERSION:=59}
: ${BOOST_VERSION:=1.${THE_BOOST_VERSION}.0}
: ${BOOST_VERSION2:=1_${THE_BOOST_VERSION}_0}

: ${BOOST_LIBS:="thread filesystem program_options system date_time locale"}

: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}
: ${IPHONE_SDKVERSION:=`xcodebuild -showsdks | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
: ${OSX_SDKVERSION:=`xcodebuild -showsdks | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}

echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

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

: ${BUILD_UIKIT_FOR_MAC:=`echo "${IPHONE_SDKVERSION} >= 13.0 && ${UIKIT_DEPLOYMENT_TARGET} >= 10.15" | bc`}

: ${XCODE_ROOT:=`xcode-select -print-path`}
: ${EXTRA_CPPFLAGS:="-fvisibility=hidden -fvisibility-inlines-hidden -DBOOST_AC_USE_PTHREADS -DBOOST_SP_USE_PTHREADS -g -DNDEBUG -std=${CLANG_CXX_LANGUAGE_STANDARD} -stdlib=${CLANG_CXX_LIBRARY} -Os"}

# The EXTRA_CPPFLAGS definition works around a thread race issue in
# shared_ptr. I encountered this historically and have not verified that
# the fix is no longer required. Without using the posix thread primitives
# an invalid compare-and-swap ARM instruction (non-thread-safe) was used for the
# shared_ptr use count causing nasty and subtle bugs.
#
# Should perhaps also consider/use instead: -BOOST_SP_USE_PTHREADS

: ${TARBALLDIR:=`pwd`}
: ${SRCDIR:="${TARBALLDIR}/src"}
: ${BUILDDIR:="${TARBALLDIR}/build"}

: ${PREFIXDIR:="${TARGET_DIR}"}

: ${CPU_COUNT:="24"}

BOOST_TARBALL="$TARBALLDIR/boost_$BOOST_VERSION2.tar.bz2"
BOOST_SRC="$SRCDIR/boost_${BOOST_VERSION2}"
JAM_FILE="$BOOST_SRC/tools/build/src/user-config.jam"

#===============================================================================
# Functions
#===============================================================================

abort()
{
    echo
    echo "Aborted: $@"
    exit 1
}

doneSection()
{
    echo
    echo "================================================================="
    echo "Done"
    echo
}

#===============================================================================

cleanEverythingReadyToStart()
{
    echo Cleaning everything before we start to build...

    echo "    rm -rf \"${BUILDDIR}\" \"${SRCDIR}\""
    rm -rf "${BUILDDIR}" "${SRCDIR}"

    doneSection
}

#===============================================================================

downloadBoost()
{
    if [ ! -s $TARBALLDIR/boost_${BOOST_VERSION2}.tar.bz2 ]; then
        echo "Downloading boost ${BOOST_VERSION}"
        curl -L -o $TARBALLDIR/boost_${BOOST_VERSION2}.tar.bz2 http://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/boost_${BOOST_VERSION2}.tar.bz2/download
    fi

    doneSection
}

#===============================================================================

unpackBoost()
{
    [ -f "$BOOST_TARBALL" ] || abort "Source tarball missing."

    echo Unpacking boost into $SRCDIR...

    [ -d $SRCDIR ]    || mkdir -p $SRCDIR
    [ -d $BOOST_SRC ] || ( cd $SRCDIR; tar xfj $BOOST_TARBALL )
    [ -d $BOOST_SRC ] && echo "    ...unpacked as $BOOST_SRC"

    echo "patching matrix.hpp"
    pushd $SRCDIR
    patch -p1 < ../matrix-${THE_BOOST_VERSION}.patch
#    patch -p1 < ../boost.patch
    popd

    doneSection
}

#===============================================================================

restoreBoost()
{
  if [ -f "${JAM_FILE}-bk" ]
  then
    mv "${JAM_FILE}-bk" "${JAM_FILE}"
  else
    rm "${JAM_FILE}"
  fi
}

#===============================================================================

updateBoost()
{
  echo Updating boost into $BOOST_SRC...

    doneSection
}

#===============================================================================

inventMissingHeaders()
{
    # These files are missing in the ARM iPhoneOS SDK, but they are in the simulator.
    # They are supported on the device, so we copy them from x86 SDK to a staging area
    # to use them on ARM, too.
    echo Invent missing headers

    cp $XCODE_ROOT/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator${IPHONE_SDKVERSION}.sdk/usr/include/{crt_externs,bzlib}.h $BOOST_SRC
}

#===============================================================================

bootstrapBoost()
{
    pushd $BOOST_SRC

    BOOST_LIBS_COMMA=$(echo $BOOST_LIBS | sed -e "s/ /,/g")
    echo "Bootstrapping (with libs $BOOST_LIBS_COMMA)"
    ./bootstrap.sh --with-libraries=$BOOST_LIBS_COMMA

    popd

    doneSection
}

#===============================================================================

runBuildBoost()
{
  local full_platform_name=$1 # one of 
    # iphoneos-iphoneos 
    # iphonesimulator-iphonesimulator 
    # macosx 
    # macosx-maccatalyst
  local architecture=$2
  local command=$3
  local target_sys=""
  local macosx_version=""
  local target_os=""
  local sdk_platform=${full_platform_name}
  local root=""
  local clang_arg=""
  local clang="$XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++"

  local clang_arch="arm64"
  if [ $architecture = "x86" ]
  then
    clang_arch="x86_64"
  fi

  case ${full_platform_name} in
  "iphoneos-iphoneos")
    sdk_platform="iphoneos"
    target_sys="ios${IPHONEOS_DEPLOYMENT_TARGET}"
    macosx_version="iphone-${IPHONE_SDKVERSION}"
    target_os="iphone"
    root="$XCODE_ROOT/Platforms/iPhoneOS.platform/Developer"
    clang_arg="-mios-version-min=$IPHONEOS_DEPLOYMENT_TARGET"
    ;;
  "iphonesimulator-iphonesimulator")
    sdk_platform="iphonesimulator"
    target_sys="ios${IPHONEOS_DEPLOYMENT_TARGET}"
    macosx_version="iphonesim-${IPHONE_SDKVERSION}"
    target_os="iphone"
    root="$XCODE_ROOT/Platforms/iPhoneSimulator.platform/Developer"
    clang_arg="-mios-version-min=$IPHONEOS_DEPLOYMENT_TARGET"
    ;;
  "macosx")
    target_sys="macos${MACOSX_DEPLOYMENT_TARGET}"
    macosx_version="${OSX_SDKVERSION}"
    target_os="darwin"
    root="$XCODE_ROOT/Platforms/MacOSX.platform/Developer"
    clang_arg="-mmacosx-version-min=$MACOSX_DEPLOYMENT_TARGET"
    ;;
  "macosx-maccatalyst")
    target_sys="ios${UIKIT_IOS_DEPLOYMENT_TARGET}-macabi"
    macosx_version="${OSX_SDKVERSION}"
    target_os="darwin"
    sdk_platform="macosx"
    root="$XCODE_ROOT/Platforms/MacOSX.platform/Developer"
    clang_arg=""
    ;;
  "*")
    echo "unknown platform ${full_platform_name}"
    exit -1
  esac

  local ICONV_PATH=$(dirname $(find $(xcrun --sdk ${sdk_platform} --show-sdk-platform-path) -name "libiconv*.tbd" | tail -1 ) | rev | cut -d/ -f2- | rev)  

  echo "ICONV_PATH=$ICONV_PATH"

  if [ -f "${JAM_FILE}" ]
  then
    rm "${JAM_FILE}"
  fi

  cat > "${JAM_FILE}" <<EOF
using darwin : al_toolset
: ${clang} ${clang_arg}
: <striper> <root>${root}
: <architecture>${architecture} <target-os>${target_os}
;
EOF


  ./b2 -j${CPU_COUNT} \
    --build-dir="${BUILDDIR}/${full_platform_name}/${architecture}" \
    --stagedir="${BUILDDIR}/${full_platform_name}/${architecture}/stage" \
    --prefix="${PREFIXDIR}/${full_platform_name}" \
    toolset="darwin-al_toolset" \
    address-model=64 \
    architecture=${architecture} \
    -sICONV_PATH="${ICONV_PATH}" \
    cxxflags="-target ${clang_arch}-apple-${target_sys} ${EXTRA_CPPFLAGS}" \
    macosx-version=${macosx_version} \
    target-os=${target_os} \
    variant=release \
    link=static \
    $command

  doneSection
}

#===============================================================================

buildBoostForPlatform() 
{
  local platform=$1
  local archs=("arm" "x86")
  for arch in "${archs[@]}"
  do
    runBuildBoost ${platform} $arch stage
  done

  mkdir -p "${BUILDDIR}/${platform}/stage/lib"

  local libs=$(cd "${BUILDDIR}/${platform}/${archs[0]}/stage/lib"; ls *.a)
  for lib in $libs
  do
    lib_files=()
    for arch in "${archs[@]}"
    do
      lib_files+=("${BUILDDIR}/${platform}/${arch}/stage/lib/${lib}")
    done
    lipo -create "${lib_files[@]}" -o "${BUILDDIR}/${platform}/stage/lib/${lib}" || abort "Lipo $lib failed"
  done
  
  copyLibs "${BUILDDIR}/${platform}/stage/" ${platform}
}

#===============================================================================

buildBoost()
{
    pushd $BOOST_SRC

    echo Building Boost for iPhone
    
    runBuildBoost iphoneos-iphoneos arm stage
    runBuildBoost iphoneos-iphoneos arm install
    
    buildBoostForPlatform iphonesimulator-iphonesimulator
    buildBoostForPlatform macosx
    if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
    then
      buildBoostForPlatform macosx-maccatalyst
    fi
    
#   exit 0

    popd
}

#===============================================================================

copyLibs() 
{
  local SRC_DIR="$1"
  local DST_DIR="$2"
  
    mkdir -p "${PREFIXDIR}/${DST_DIR}/lib/"
    pushd "${PREFIXDIR}/${DST_DIR}/lib/"
    rm libboost_*.a
    cp "${SRC_DIR}/lib/"* ./
    popd
  mkdir -p "${PREFIXDIR}/${DST_DIR}/include/"
  pushd "${PREFIXDIR}/${DST_DIR}/include/"
  ln -fs "../../iphoneos-iphoneos/include/boost"
  popd
}

#===============================================================================
cleanAfterBuild()
{
  echo "Removing directories"
  echo "rm -rf \"$BUILDDIR\""
  rm -rf "${BUILDDIR}"
  echo "rm -rf \"$SRCDIR\""
  rm -rf "${SRCDIR}"

    doneSection
}

#===============================================================================
# Execution starts here
#===============================================================================

# cleanEverythingReadyToStart #may want to comment if repeatedly running during dev
# restoreBoost
# 
# echo "BOOST_VERSION:              $BOOST_VERSION"
# echo "BOOST_LIBS:                 $BOOST_LIBS"
# echo "BOOST_SRC:                  $BOOST_SRC"
# echo "PREFIXDIR:                  $PREFIXDIR"
# echo "IPHONE_SDKVERSION:          $IPHONE_SDKVERSION"
# echo "OSX_SDKVERSION:             $OSX_SDKVERSION"
# echo "IPHONEOS_DEPLOYMENT_TARGET: $IPHONEOS_DEPLOYMENT_TARGET"
# echo "MACOSX_DEPLOYMENT_TARGET:   $MACOSX_DEPLOYMENT_TARGET"
# echo "BUILD_UIKIT_FOR_MAC:        $BUILD_UIKIT_FOR_MAC"
# echo "XCODE_ROOT:                 $XCODE_ROOT"
# echo
# 
# cleanAfterBuild
# 
# downloadBoost
# unpackBoost
# inventMissingHeaders
# bootstrapBoost
# updateBoost
rm -rf "${BUILDDIR}"
buildBoost
#cleanAfterBuild
# restoreBoost

echo "Completed successfully"

#===============================================================================
