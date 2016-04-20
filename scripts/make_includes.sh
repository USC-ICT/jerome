#! /bin/sh

#  make_includes.sh
#  jerome
#
#  Created by Anton Leuski on 9/24/15.
#  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.

# prepare files for C++-like inclusion

FILES_TO_INCLUDE=(\
	"${SRCROOT}/jerome/scripting/startup.js" \
	"${SRCROOT}/jerome/scripting/utteranceCompiler.js" \
	"${SRCROOT}/3rdParty/SCION/dist/scion.js" \
	"${SRCROOT}/jerome/ir/report/HTMLReporter.xslt"\
	)

mkdir -p "${DERIVED_FILE_DIR}"

for f in "${FILES_TO_INCLUDE[@]}"
do
	newName=`basename "$f"`
	newName="${DERIVED_FILE_DIR}/$newName.inc"
	echo -n 'u8R"=====(' > "$newName"
	cat "$f" >> "$newName"
	echo -n ')====="' >> "$newName"
done





 