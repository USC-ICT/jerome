#! /bin/sh

src_dir=$1
dst_dir=$2

found_boost="YES"
for platform_name in iphoneos iphonesimulator macosx
do
	if [ ! -e "${dst_dir}/${platform_name}/Frameworks/boost.framework" ]
	then
		found_boost="NO"
	fi
done

if [ "${found_boost}" = "YES" ]
then
	echo "Found boost binary"
	exit
fi

make_directory () {
	local dst_platform_name="$1"

	if [ -e "${dst_dir}/${dst_platform_name}/Frameworks/boost.framework" ]
	then
		rm -rf "${dst_dir}/${dst_platform_name}/Frameworks/boost.framework"
	fi

	if [ ! -d "${dst_dir}/${dst_platform_name}/Frameworks/" ]
	then
		mkdir -p "${dst_dir}/${dst_platform_name}/Frameworks/"
	fi
}

pushd "${src_dir}"
	./boost.sh
	rm -rf build
	rm -rf src

	src_platform_name="osx"
	dst_platform_name="macosx"
	make_directory "${dst_platform_name}"

	mv -f "${src_platform_name}/framework/boost.framework" "${dst_dir}/${dst_platform_name}/Frameworks/"
	rm -rf "${src_platform_name}"

	src_platform_name="ios"
	dst_platform_name="iphoneos"
	make_directory "${dst_platform_name}"

	mv -f "${src_platform_name}/framework/boost.framework" "${dst_dir}/${dst_platform_name}/Frameworks/"
	rm -rf "${src_platform_name}"

	dst_platform_name="iphonesimulator"
	make_directory "${dst_platform_name}"

	pushd "${dst_dir}/${dst_platform_name}/Frameworks/"
	ln -sf ../../iphoneos/Frameworks/boost.framework
	popd

popd
