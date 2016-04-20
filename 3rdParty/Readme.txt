to compile nlopt:

./configure --prefix=jerome/3rdParty/nlopt-2.3/build \
	--enable-shared --with-cxx --without-guile --without-python --without-octave --without-matlab \
	CXX=c++ CXXFLAGS=-std=c++11 -stdlib=libc++ -Os CFLAGS=-Os CC=clang
