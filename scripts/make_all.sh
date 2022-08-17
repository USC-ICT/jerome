#! /bin/zsh

pushd $(dirname "${0}")
root_dir=$(pwd -L)
popd

thirdParty_dir="${root_dir}/../3rdParty"
build_dir="${thirdParty_dir}/build"

if [ ! -d "${build_dir}" ]
then
	mkdir -p "${build_dir}"
fi

if [ "macosx${SDK_VERSION}" = "${SDK_NAME}" ]
then
  OSX_SDKVERSION=${SDK_VERSION}
fi

if [ "${OSX_SDKVERSION}" = "13.0" ]
then
  IPHONE_SDKVERSION="16.0"
fi

echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "OSX_SDKVERSION = ${OSX_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

echo "compile ${root_dir} to ${build_dir}"

: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}

if [ -z "${OSX_SDKVERSION}" -o -z "${IPHONE_SDKVERSION}" ]
then
  # this is very expensive (~0.7 sec) with Xcode beta. 
  : ${SDKS:=`xcodebuild -showsdks`}
  : ${IPHONE_SDKVERSION:=`echo "${SDKS}" | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
  : ${OSX_SDKVERSION:=`echo "${SDKS}" | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
fi

echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "OSX_SDKVERSION = ${OSX_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

: ${BUILD_UIKIT_FOR_MAC:=`echo "${IPHONE_SDKVERSION} >= 13.0" | bc`}

if [ "x${PLATFORM_NAME}${EFFECTIVE_PLATFORM_NAME}" != "x" ]
then
  PLATFORM_NAMES="${PLATFORM_NAME}${EFFECTIVE_PLATFORM_NAME}"
else
  if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
  then
    echo "build macosx-maccatalyst on"
    PLATFORM_NAMES="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx macosx-maccatalyst"
  else
    echo "build macosx-maccatalyst off"
    PLATFORM_NAMES="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx"
  fi
fi

export IPHONEOS_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET
export CLANG_CXX_LANGUAGE_STANDARD
export CLANG_CXX_LIBRARY
export IPHONE_SDKVERSION
export OSX_SDKVERSION
export BUILD_UIKIT_FOR_MAC
export PLATFORM_NAMES

pushd "${root_dir}"
  time ./compile_boost.sh "${thirdParty_dir}/boost" "${build_dir}"
  time ./compile_nlopt.sh "${thirdParty_dir}" "${build_dir}"
  time ./compile_eigen.sh "${thirdParty_dir}" "${build_dir}"
popd
