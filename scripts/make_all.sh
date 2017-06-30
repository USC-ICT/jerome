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

pushd "${root_dir}"
	./compile_nlopt.sh "${thirdParty_dir}" "${build_dir}"
	./compile_eigen.sh "${thirdParty_dir}" "${build_dir}"
	./compile_boost.sh "${thirdParty_dir}/boost" "${build_dir}"
popd
