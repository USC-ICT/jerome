#!/bin/sh

#  copy_3rdParty_libs.sh
#  jerome
#
#  Created by Anton Leuski on 6/6/19.
#  Copyright © 2019 Anton Leuski & ICT/USC. All rights reserved.

name=${EFFECTIVE_PLATFORM_NAME}
name=${name#"-"}
echo ">> $name"
echo ">> ${TARGET_BUILD_DIR}"

if [ "${name}" = "" ]
then
  name="macosx"
fi

cp -R "${SRCROOT}/3rdParty/build/${name}/lib/"* "${TARGET_BUILD_DIR}/"
