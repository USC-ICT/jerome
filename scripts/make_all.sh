#! /bin/sh

pushd $(dirname "${0}")
root_dir=$(pwd -L)
popd

thirdParty_dir="${root_dir}/../3rdParty"
build_dir="${thirdParty_dir}/build"

if [ ! -d "${build_dir}" ]
then
	mkdir -p "${build_dir}"
fi

echo "compile ${root_dir} to ${build_dir}"

: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}
: ${IPHONE_SDKVERSION:=`xcodebuild -showsdks | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
: ${OSX_SDKVERSION:=`xcodebuild -showsdks | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}

echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

: ${BUILD_UIKIT_FOR_MAC:=`echo "${IPHONE_SDKVERSION} >= 13.0 && ${MACOSX_DEPLOYMENT_TARGET} >= 10.15" | bc`}

export IPHONEOS_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET
export CLANG_CXX_LANGUAGE_STANDARD
export CLANG_CXX_LIBRARY
export BUILD_UIKIT_FOR_MAC

pushd "${root_dir}"
	./compile_boost.sh "${thirdParty_dir}/boost" "${build_dir}"
	./compile_nlopt.sh "${thirdParty_dir}" "${build_dir}"
	./compile_eigen.sh "${thirdParty_dir}" "${build_dir}"
popd
