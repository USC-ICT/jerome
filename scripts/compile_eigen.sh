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

found_eigen="YES"
for platform_name in ${platform_names}
do
	if [ ! -e "${dst_dir}/${platform_name}/include/eigen3" ]
	then
		found_eigen="NO"
	fi
done

if [ "${found_eigen}" = "YES" ]
then
	echo "Found eigen binary"
	exit
fi

pushd "${src_dir}"
#	eigen_name="eigen-eigen-07105f7124f9"
	eigen_name="eigen-eigen-b3f3d4950030"
#	tar -xkf eigen-3.2.8.tar.bz2
	tar -xkf eigen-3.3.5.tar.bz2
	pushd "${eigen_name}"
		mkdir "build"
		pushd "build"
			cmake .. -DCMAKE_INSTALL_PREFIX="${dst_dir}/macosx"
			make install
		popd

    if [ ${BUILD_UIKIT_FOR_MAC} -eq 1 ]
    then
      link_platform_names="iphoneos-iphoneos iphonesimulator-iphonesimulator macosx-maccatalyst"
    else
      link_platform_names="iphoneos-iphoneos iphonesimulator-iphonesimulator"
    fi

		for platform_name in ${link_platform_names}
		do
			if [ ! -d "${dst_dir}/${platform_name}/include/" ]
			then
				mkdir -p "${dst_dir}/${platform_name}/include/"
			fi

			pushd "${dst_dir}/${platform_name}/include/"
			ln -sf "../../macosx/include/eigen3"
			popd
		done

	popd
	rm -rf "${eigen_name}"
popd
