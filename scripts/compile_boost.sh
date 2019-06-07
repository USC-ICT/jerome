#! /bin/sh

src_dir=$1
dst_dir=$2

found_boost="YES"
for platform_name in iphoneos iphonesimulator macosx uikitformac
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

pushd "${src_dir}"
	./boost.sh "${dst_dir}"
popd
