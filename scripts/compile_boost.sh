#! /bin/sh

: ${src_dir:=$1}
: ${dst_dir:=$2}

echo "compile_boost ${src_dir} ${dst_dir}"

: ${IPHONEOS_DEPLOYMENT_TARGET:=12.0}
: ${MACOSX_DEPLOYMENT_TARGET:=10.13}
: ${CLANG_CXX_LANGUAGE_STANDARD:=gnu++14}
: ${CLANG_CXX_LIBRARY:=libc++}
: ${SDKS:=`xcodebuild -showsdks`}
: ${IPHONE_SDKVERSION:=`echo "${SDKS}" | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
: ${OSX_SDKVERSION:=`echo "${SDKS}" | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}

echo "IPHONE_SDKVERSION = ${IPHONE_SDKVERSION}"
echo "MACOSX_DEPLOYMENT_TARGET = ${MACOSX_DEPLOYMENT_TARGET}"

: ${BUILD_UIKIT_FOR_MAC:=`echo "${IPHONE_SDKVERSION} >= 13.0" | bc`}

if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
then
  echo "build macosx-maccatalyst on"
  : ${PLATFORM_NAMES:="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx macosx-maccatalyst"}
else
  echo "build macosx-maccatalyst off"
  : ${PLATFORM_NAMES:="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx"}
fi

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
  export IPHONEOS_DEPLOYMENT_TARGET
  export MACOSX_DEPLOYMENT_TARGET
  export CLANG_CXX_LANGUAGE_STANDARD
  export CLANG_CXX_LIBRARY
  export IPHONE_SDKVERSION
  export OSX_SDKVERSION
  export BUILD_UIKIT_FOR_MAC

  pushd "${src_dir}"
    ./boost.sh "${dst_dir}"
  popd
fi

