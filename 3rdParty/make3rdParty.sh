#! /bin/sh

pushd $(dirname "${0}")
root_dir=$(pwd -L)
popd

build_dir="${root_dir}/build"

mkdir -p "${build_dir}"

for platform_name in ios osx
do
	for dir_name in Frameworks lib include
	do
		mkdir -p "${build_dir}/${platform_name}/${dir_name}"
	done
done

## build boost

pushd "${root_dir}/boost"
	./boost.sh
	rm -rf build
	rm -rf src

	for platform_name in ios osx
	do
		if [ -d "${build_dir}/${platform_name}/Frameworks/boost.framework" ]
		then
			rm -rf "${build_dir}/${platform_name}/Frameworks/boost.framework"
		fi

		mv -f "${root_dir}/boost/${platform_name}/framework/boost.framework" "${build_dir}/${platform_name}/Frameworks/"
		rm -rf "${platform_name}"
	done
popd

## build eigen

eigen_name="eigen-eigen-07105f7124f9"
tar -xkf eigen-3.2.8.tar.bz2
pushd "${eigen_name}"
	mkdir "build"
	pushd "build"
		cmake .. -DCMAKE_INSTALL_PREFIX="${build_dir}/osx"
		make install
	popd
	ln -sf "${build_dir}/osx/include/eigen3" "${build_dir}/ios/include/eigen3"
popd
rm -rf "${eigen_name}"

## build nlopt

pushd "${root_dir}"
	./compile_nlopt.sh
popd

