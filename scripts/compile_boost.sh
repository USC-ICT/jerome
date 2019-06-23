#! /bin/sh

src_dir=$1
dst_dir=$2

: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}
: ${IPHONE_SDKVERSION:=`xcodebuild -showsdks | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
: ${OSX_SDKVERSION:=`xcodebuild -showsdks | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}

echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

: ${BUILD_UIKIT_FOR_MAC:=`echo "${IPHONE_SDKVERSION} >= 13.0 && ${MACOSX_DEPLOYMENT_TARGET} >= 10.15" | bc`}

if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
then
  echo "build uikitformac on"
  platform_names="iphoneos iphonesimulator macosx uikitformac"
else
  echo "build uikitformac off"
  platform_names="iphoneos iphonesimulator macosx"
fi

found_boost="YES"
for platform_name in ${platform_names}
do
	if [ ! -e "${dst_dir}/${platform_name}/lib/libboost_system.a" ]
	then
		found_boost="NO"
	fi
done

if [ "${found_boost}" = "YES" ]
then
	echo "Found boost binary"
	exit
fi

export IPHONEOS_DEPLOYMENT_TARGET
export MACOSX_DEPLOYMENT_TARGET
export CLANG_CXX_LANGUAGE_STANDARD
export CLANG_CXX_LIBRARY
export BUILD_UIKIT_FOR_MAC

pushd "${src_dir}"
	./boost.sh "${dst_dir}"
popd
