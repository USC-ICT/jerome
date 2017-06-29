#===============================================================================
# Filename:  boost.sh
# Author:    Pete Goodliffe
# Copyright: (c) Copyright 2009 Pete Goodliffe
# Licence:   Please feel free to use this, with attribution
#===============================================================================
# Modified by Anton Leuski
#===============================================================================
# Builds a Boost framework for the iPhone, iPhone Simulator, and OSX.
# Creates a set of universal libraries that can be used on an iPhone and in the
# iPhone simulator. Then creates a pseudo-framework to make using boost in Xcode
# less painful.
#
# To configure the script, define:
#    BOOST_LIBS:        which libraries to build
#    IPHONE_SDKVERSION: iPhone SDK version (e.g. 8.1)
#    OSX_SDKVERSION: OSX SDK version (e.g. 10.10)
#
# Then go get the source tar.bz of the boost you want to build, shove it in the
# same directory as this script, and run "./boost.sh". Grab a cuppa. And voila.
#===============================================================================

: ${THE_BOOST_VERSION:=59}
: ${BOOST_VERSION:=1.${THE_BOOST_VERSION}.0}
: ${BOOST_VERSION2:=1_${THE_BOOST_VERSION}_0}

: ${BOOST_LIBS:="thread signals filesystem program_options system date_time"}

# Current iPhone SDK
: ${IPHONE_SDKVERSION:=`xcodebuild -showsdks | grep iphoneos | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
# Specific iPhone SDK
: ${MIN_IOS_VERSION:=8.1}

# Current OSX SDK
: ${OSX_SDKVERSION:=`xcodebuild -showsdks | grep macosx | egrep "[[:digit:]]+\.[[:digit:]]+" -o | tail -1`}
# Specific OSX SDK
: ${MIN_OSX_VERSION:=10.10}

: ${XCODE_ROOT:=`xcode-select -print-path`}
: ${EXTRA_CPPFLAGS:="-fvisibility=hidden -fvisibility-inlines-hidden -DBOOST_AC_USE_PTHREADS -DBOOST_SP_USE_PTHREADS -g -DNDEBUG -std=c++11 -stdlib=libc++ -Os"}

# The EXTRA_CPPFLAGS definition works around a thread race issue in
# shared_ptr. I encountered this historically and have not verified that
# the fix is no longer required. Without using the posix thread primitives
# an invalid compare-and-swap ARM instruction (non-thread-safe) was used for the
# shared_ptr use count causing nasty and subtle bugs.
#
# Should perhaps also consider/use instead: -BOOST_SP_USE_PTHREADS

: ${TARBALLDIR:=`pwd`}
: ${SRCDIR:="${TARBALLDIR}/src"}
: ${IOSDIR:="${TARBALLDIR}/ios"}
: ${OSXDIR:="${TARBALLDIR}/osx"}
: ${BUILDDIR:="${TARBALLDIR}/build"}

: ${IOS_TARGET_DIR:="${BUILDDIR}/ios"}
: ${ARM_B2_DIR:="${IOS_TARGET_DIR}/iphone"}
: ${SIM_B2_DIR:="${IOS_TARGET_DIR}/phonesim"}
: ${IOS_BUILD_DIR:="${IOS_TARGET_DIR}/build"}

: ${OSX_TARGET_DIR:="${BUILDDIR}/osx"}
: ${OSX_BUILD_DIR:="${OSX_TARGET_DIR}/build"}
: ${OSX_B2_DIR:="${OSX_TARGET_DIR}/osx"}
: ${PREFIXDIR:="${BUILDDIR}/prefix"}

: ${IOSFRAMEWORKDIR:="${IOSDIR}/framework"}
: ${OSXFRAMEWORKDIR:="${OSXDIR}/framework"}
: ${IOS_ARCHS:="armv7 armv7s arm64"}
: ${OSX_ARCHS:="x86_64"}

OSX_ARCHS_OPTIONS=""
for a in $OSX_ARCHS
do
	OSX_ARCHS_OPTIONS="$OSX_ARCHS_OPTIONS -arch $a"
done

IOS_ARCHS_OPTIONS=""
for a in $IOS_ARCHS
do
	IOS_ARCHS_OPTIONS="$IOS_ARCHS_OPTIONS -arch $a"
done

BOOST_TARBALL="$TARBALLDIR/boost_$BOOST_VERSION2.tar.bz2"
BOOST_SRC="$SRCDIR/boost_${BOOST_VERSION2}"
JAM_FILE="$BOOST_SRC/tools/build/src/user-config.jam"

#===============================================================================
ARM_DEV_CMD="xcrun --sdk iphoneos"
SIM_DEV_CMD="xcrun --sdk iphonesimulator"
OSX_DEV_CMD="xcrun --sdk macosx"

#===============================================================================
# Functions
#===============================================================================

abort()
{
    echo
    echo "Aborted: $@"
    exit 1
}

doneSection()
{
    echo
    echo "================================================================="
    echo "Done"
    echo
}

#===============================================================================

cleanEverythingReadyToStart()
{
    echo Cleaning everything before we start to build...

		echo "    rm -rf \"${BUILDDIR}\" \"${IOSDIR}\" \"${OSXDIR}\" \"${SRCDIR}\""
    rm -rf "${BUILDDIR}" "${IOSDIR}" "${OSXDIR}" "${SRCDIR}"

    doneSection
}

#===============================================================================

downloadBoost()
{
    if [ ! -s $TARBALLDIR/boost_${BOOST_VERSION2}.tar.bz2 ]; then
        echo "Downloading boost ${BOOST_VERSION}"
        curl -L -o $TARBALLDIR/boost_${BOOST_VERSION2}.tar.bz2 http://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/boost_${BOOST_VERSION2}.tar.bz2/download
    fi

    doneSection
}

#===============================================================================

unpackBoost()
{
    [ -f "$BOOST_TARBALL" ] || abort "Source tarball missing."

    echo Unpacking boost into $SRCDIR...

    [ -d $SRCDIR ]    || mkdir -p $SRCDIR
    [ -d $BOOST_SRC ] || ( cd $SRCDIR; tar xfj $BOOST_TARBALL )
    [ -d $BOOST_SRC ] && echo "    ...unpacked as $BOOST_SRC"

    echo "patching matrix.hpp"
    pushd $SRCDIR
    patch -p1 < ../matrix.patch
    popd

    doneSection
}

#===============================================================================

restoreBoost()
{
	if [ -f "${JAM_FILE}-bk" ]
	then
    mv "${JAM_FILE}-bk" "${JAM_FILE}"
  else
  	rm "${JAM_FILE}"
  fi
}

#===============================================================================

updateBoost()
{
    echo Updating boost into $BOOST_SRC...

		if [ -f "${JAM_FILE}" ]
		then
	    cp "${JAM_FILE}" "${JAM_FILE}-bk"
		fi

    cat >> "${JAM_FILE}" <<EOF
using darwin : ${IPHONE_SDKVERSION}~iphone
: $XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ ${IOS_ARCHS_OPTIONS} -mios-version-min=$MIN_IOS_VERSION $EXTRA_CPPFLAGS
: <striper> <root>$XCODE_ROOT/Platforms/iPhoneOS.platform/Developer
: <architecture>arm <target-os>iphone
;
using darwin : ${IPHONE_SDKVERSION}~iphonesim
: $XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ ${OSX_ARCHS_OPTIONS} -mios-version-min=$MIN_IOS_VERSION $EXTRA_CPPFLAGS
: <striper> <root>$XCODE_ROOT/Platforms/iPhoneSimulator.platform/Developer
: <architecture>x86 <target-os>iphone
;
using darwin : ${OSX_SDKVERSION}
: $XCODE_ROOT/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++ ${OSX_ARCHS_OPTIONS} -mmacosx-version-min=$MIN_OSX_VERSION $EXTRA_CPPFLAGS
: <striper> <root>$XCODE_ROOT/Platforms/MacOSX.platform/Developer
: <architecture>x86 <target-os>darwin
;
EOF

    doneSection
}

#===============================================================================

inventMissingHeaders()
{
    # These files are missing in the ARM iPhoneOS SDK, but they are in the simulator.
    # They are supported on the device, so we copy them from x86 SDK to a staging area
    # to use them on ARM, too.
    echo Invent missing headers

    cp $XCODE_ROOT/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator${IPHONE_SDKVERSION}.sdk/usr/include/{crt_externs,bzlib}.h $BOOST_SRC
}

#===============================================================================

bootstrapBoost()
{
    pushd $BOOST_SRC

    BOOST_LIBS_COMMA=$(echo $BOOST_LIBS | sed -e "s/ /,/g")
    echo "Bootstrapping (with libs $BOOST_LIBS_COMMA)"
    ./bootstrap.sh --with-libraries=$BOOST_LIBS_COMMA

		popd

    doneSection
}

#===============================================================================

buildBoost()
{
    pushd $BOOST_SRC

    echo Building Boost for iPhone
    ./b2 -j16 --build-dir="${ARM_B2_DIR}" --stagedir="${ARM_B2_DIR}/stage" --prefix="$PREFIXDIR" \
    	toolset=darwin cxxflags="${EXTRA_CPPFLAGS}" architecture=arm \
    	target-os=iphone macosx-version=iphone-${IPHONE_SDKVERSION} define=_LITTLE_ENDIAN link=static stage
    doneSection

    echo Installing Boost Headers
    # Install this one so we can copy the includes for the frameworks...
    ./b2 -j16 --build-dir="${ARM_B2_DIR}" --stagedir="${ARM_B2_DIR}/stage" --prefix=$PREFIXDIR toolset=darwin cxxflags="${EXTRA_CPPFLAGS}" architecture=arm target-os=iphone macosx-version=iphone-${IPHONE_SDKVERSION} define=_LITTLE_ENDIAN link=static install
    doneSection

    echo Building Boost for iPhoneSimulator
    ./b2 -j16 --build-dir="${SIM_B2_DIR}" --stagedir="${SIM_B2_DIR}/stage" toolset=darwin-${IPHONE_SDKVERSION}~iphonesim cxxflags="${EXTRA_CPPFLAGS}" architecture=x86 target-os=iphone macosx-version=iphonesim-${IPHONE_SDKVERSION} link=static stage
    doneSection

    echo building Boost for OSX
    ./b2 -j16 --build-dir="${OSX_B2_DIR}" --stagedir="${OSX_B2_DIR}/stage" toolset=darwin-${OSX_SDKVERSION} cxxflags="${EXTRA_CPPFLAGS} ${OSX_ARCHS_OPTIONS}" architecture=x86 linkflags="-stdlib=libc++" link=static threading=multi macosx-version=${OSX_SDKVERSION} stage
    doneSection

    popd
}

#===============================================================================

unpackArchive()
{
    local BUILDDIR=$1
    local LIBNAME=$2

    echo "Unpacking $LIBNAME"

    mkdir -p $BUILDDIR/$LIBNAME
    rm $BUILDDIR/$LIBNAME/*.o
    rm $BUILDDIR/$LIBNAME/*.SYMDEF*

    (
        cd $BUILDDIR/$NAME; ar -x ../../libboost_$NAME.a;
        for FILE in *.o; do
            NEW_FILE="${NAME}_${FILE}"
            mv $FILE $NEW_FILE
        done
    )
}

scrunchOneLib()
{
	DEV_CMD="$1"
	BUILD_DIR="$2"
	ARCH="$3"
	TARGET_DIR="$4"
	$NAME="$5"

	echo ${ARCH}
	$DEV_CMD lipo "${BUILD_DIR}/stage/lib/libboost_$NAME.a" -thin ${ARCH} -o "${TARGET_DIR}/${ARCH}/libboost_$NAME.a"
	unpackArchive "${TARGET_DIR}/${ARCH}/obj" $NAME
	(cd ${TARGET_DIR}/${ARCH}; $DEV_CMD ar crus libboost.a obj/$NAME/*.o; )
}

scrunchAllLibsTogetherInOneLibPerPlatform()
{
    pushd $BOOST_SRC

    rm "$IOS_BUILD_DIR/*/libboost.a"
    rm "$OSX_BUILD_DIR/*/libboost.a"

    # iOS Device
	for ARCH in ${IOS_ARCHS}; do
	    mkdir -p $IOS_BUILD_DIR/${ARCH}/obj
	done

    # iOS Simulator
	for ARCH in ${OSX_ARCHS}; do
    	mkdir -p $IOS_BUILD_DIR/${ARCH}/obj
	done

    # OSX
	for ARCH in ${OSX_ARCHS}; do
    	mkdir -p $OSX_BUILD_DIR/${ARCH}/obj
	done

    ALL_LIBS=""

    echo Splitting all existing fat binaries...

    for NAME in $BOOST_LIBS; do
        if [ "$NAME" == "test" ]; then
            NAME="unit_test_framework"
        fi

        ALL_LIBS="$ALL_LIBS libboost_$NAME.a"

		for ARCH in ${IOS_ARCHS}; do
			scrunchOneLib "$ARM_DEV_CMD" "${ARM_B2_DIR}" "${ARCH}" "$IOS_BUILD_DIR" "$NAME"
	    done

		for ARCH in ${OSX_ARCHS}; do
			scrunchOneLib "$SIM_DEV_CMD" "${SIM_B2_DIR}" "${ARCH}" "$IOS_BUILD_DIR" "$NAME"
		done

		for ARCH in ${OSX_ARCHS}; do
			scrunchOneLib "$OSX_DEV_CMD" "${OSX_B2_DIR}" "${ARCH}" "$OSX_BUILD_DIR" "$NAME"
		done

    done

    popd
}

#===============================================================================
buildFramework()
{
    : ${1:?}
    local FRAMEWORKDIR=$1
    local BUILDDIR=$2

    VERSION_TYPE=Alpha
    FRAMEWORK_NAME=boost
    FRAMEWORK_VERSION=A

    FRAMEWORK_CURRENT_VERSION=$BOOST_VERSION
    FRAMEWORK_COMPATIBILITY_VERSION=$BOOST_VERSION

    FRAMEWORK_BUNDLE=$FRAMEWORKDIR/$FRAMEWORK_NAME.framework
    echo "Framework: Building $FRAMEWORK_BUNDLE from $BUILDDIR..."

    rm -rf $FRAMEWORK_BUNDLE

    echo "Framework: Setting up directories..."
    mkdir -p $FRAMEWORK_BUNDLE
    mkdir -p $FRAMEWORK_BUNDLE/Versions
    mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION
    mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/Resources
    mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/Headers
    mkdir -p $FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/Documentation

    echo "Framework: Creating symlinks..."
    ln -s $FRAMEWORK_VERSION               $FRAMEWORK_BUNDLE/Versions/Current
    ln -s Versions/Current/Headers         $FRAMEWORK_BUNDLE/Headers
    ln -s Versions/Current/Resources       $FRAMEWORK_BUNDLE/Resources
    ln -s Versions/Current/Documentation   $FRAMEWORK_BUNDLE/Documentation
    ln -s Versions/Current/$FRAMEWORK_NAME $FRAMEWORK_BUNDLE/$FRAMEWORK_NAME

    FRAMEWORK_INSTALL_NAME=$FRAMEWORK_BUNDLE/Versions/$FRAMEWORK_VERSION/$FRAMEWORK_NAME

    echo "Lipoing library into $FRAMEWORK_INSTALL_NAME..."
    $ARM_DEV_CMD lipo -create $BUILDDIR/*/libboost.a -o "$FRAMEWORK_INSTALL_NAME" || abort "Lipo $1 failed"

    echo "Framework: Copying includes..."
    cp -r $PREFIXDIR/include/boost/*  $FRAMEWORK_BUNDLE/Headers/

    echo "Framework: Creating plist..."
    cat > $FRAMEWORK_BUNDLE/Resources/Info.plist <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
<key>CFBundleDevelopmentRegion</key>
<string>English</string>
<key>CFBundleExecutable</key>
<string>${FRAMEWORK_NAME}</string>
<key>CFBundleIdentifier</key>
<string>org.boost</string>
<key>CFBundleInfoDictionaryVersion</key>
<string>6.0</string>
<key>CFBundlePackageType</key>
<string>FMWK</string>
<key>CFBundleSignature</key>
<string>????</string>
<key>CFBundleVersion</key>
<string>${FRAMEWORK_CURRENT_VERSION}</string>
</dict>
</plist>
EOF

    doneSection
}

#===============================================================================
cleanAfterBuild()
{
	echo "Removing directories"
	echo "rm -rf \"$PREFIXDIR\""
	rm -rf "$PREFIXDIR"
	echo "rm -rf \"$BUILDDIR\""
	rm -rf "${BUILDDIR}"
	echo "rm -rf \"$SRCDIR\""
	rm -rf "${SRCDIR}"

    doneSection
}

#===============================================================================
# Execution starts here
#===============================================================================

mkdir -p $IOS_TARGET_DIR

cleanEverythingReadyToStart #may want to comment if repeatedly running during dev
restoreBoost

echo "BOOST_VERSION:     $BOOST_VERSION"
echo "BOOST_LIBS:        $BOOST_LIBS"
echo "BOOST_SRC:         $BOOST_SRC"
echo "IOS_BUILD_DIR:       $IOS_BUILD_DIR"
echo "OSX_BUILD_DIR:       $OSX_BUILD_DIR"
echo "PREFIXDIR:         $PREFIXDIR"
echo "IOSFRAMEWORKDIR:   $IOSFRAMEWORKDIR"
echo "OSXFRAMEWORKDIR:   $OSXFRAMEWORKDIR"
echo "IPHONE_SDKVERSION: $IPHONE_SDKVERSION"
echo "OSX_SDKVERSION:    $OSX_SDKVERSION"
echo "XCODE_ROOT:        $XCODE_ROOT"
echo

downloadBoost
unpackBoost
inventMissingHeaders
bootstrapBoost
updateBoost
buildBoost
scrunchAllLibsTogetherInOneLibPerPlatform
buildFramework $IOSFRAMEWORKDIR $IOS_BUILD_DIR
buildFramework $OSXFRAMEWORKDIR $OSX_BUILD_DIR
cleanAfterBuild
restoreBoost

echo "Completed successfully"

#===============================================================================
