#! /bin/sh

src_dir=$1
dst_dir=$2

found_boost="YES"
for platform_name in ios osx
do
	if [ ! -d "${dst_dir}/${platform_name}/Frameworks/boost.framework" ]
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
	./boost.sh
	rm -rf build
	rm -rf src

	for platform_name in ios osx
	do
		if [ -d "${dst_dir}/${platform_name}/Frameworks/boost.framework" ]
		then
			rm -rf "${dst_dir}/${platform_name}/Frameworks/boost.framework"
		fi

		if [ ! -d "${dst_dir}/${platform_name}/Frameworks/" ]
		then
			mkdir -p "${dst_dir}/${platform_name}/Frameworks/"
		fi

		mv -f "${platform_name}/framework/boost.framework" "${dst_dir}/${platform_name}/Frameworks/"
		rm -rf "${platform_name}"
	done
popd
