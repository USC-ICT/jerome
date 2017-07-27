#! /bin/sh

src_dir=$1
dst_dir=$2

found_eigen="YES"
for platform_name in iphoneos iphonesimulator macosx
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
	eigen_name="eigen-eigen-07105f7124f9"
	tar -xkf eigen-3.2.8.tar.bz2
	pushd "${eigen_name}"
		mkdir "build"
		pushd "build"
			cmake .. -DCMAKE_INSTALL_PREFIX="${dst_dir}/macosx"
			make install
		popd

		for platform_name in iphoneos iphonesimulator
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
