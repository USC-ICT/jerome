#! /bin/sh

: ${src_dir:=$1}
: ${dst_dir:=$2}

echo "compile_boost ${src_dir} ${dst_dir}"

: ${VISIONOS_DEPLOYMENT_TARGET:=1.0}
: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}

if [ -z "${OSX_SDKVERSION}" -o -z "${IPHONE_SDKVERSION}" -o -z "${VISION_SDKVERSION}" ]
then
  : ${SDKS:="$(xcodebuild -showsdks | egrep '\-sdk (\w|\.)+' -o)"}
  : ${VISION_SDKVERSION:=$(echo "${SDKS}" | grep xros     | egrep "[[:digit:]]+\.[[:digit:]]+" -o | head -n 1)}
  : ${IPHONE_SDKVERSION:=$(echo "${SDKS}" | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | head -n 1)}
  : ${OSX_SDKVERSION:=$(echo "${SDKS}" | grep macosx   | egrep "[[:digit:]]+\.[[:digit:]]+" -o | head -n 1)}
fi

echo "VISION_SDKVERSION = ${VISION_SDKVERSION}"
echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

: ${BUILD_UIKIT_FOR_MAC:=`echo "${IPHONE_SDKVERSION} >= 13.0" | bc`}
: ${BUILD_VISION:=`echo "${IPHONE_SDKVERSION} >= 17.0" | bc`}

base_names="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx"

if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
then
  echo "build macosx-maccatalyst on"
  base_names="${base_names} macosx-maccatalyst"
else
  echo "build macosx-maccatalyst off"
fi

if [ ${BUILD_VISION} -eq 1 ]
then
  echo "build vision on"
  base_names="${base_names} xros-xros xrsimulator-xrsimulator"
else
  echo "build vision off"
fi

: ${PLATFORM_NAMES:=${base_names}}

found_boost="YES"
for platform_name in ${PLATFORM_NAMES}
do
  echo "${dst_dir}/${platform_name}/lib/libboost_system.a"
	if [ ! -e "${dst_dir}/${platform_name}/lib/libboost_system.a" ]
	then
		found_boost="NO"
	fi
done

if [ "${found_boost}" = "YES" ]
then
	echo "Found boost binary"
else
  export VISIONOS_DEPLOYMENT_TARGET
  export IPHONEOS_DEPLOYMENT_TARGET
  export MACOSX_DEPLOYMENT_TARGET
  export CLANG_CXX_LANGUAGE_STANDARD
  export CLANG_CXX_LIBRARY
  export VISION_SDKVERSION
  export IPHONE_SDKVERSION
  export OSX_SDKVERSION
  export BUILD_UIKIT_FOR_MAC
  export PLATFORM_NAMES

  pushd "${src_dir}"
    ./boost.sh "${dst_dir}"
  popd
fi

