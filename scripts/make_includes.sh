#! /bin/sh

#  make_includes.sh
#  jerome
#
#  Created by Anton Leuski on 9/24/15.
#  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.

# prepare files for C++-like inclusion

src_dir=${SRCROOT:=$1}
dst_dir=${DERIVED_FILE_DIR:=$2}

echo "src_dir = ${src_dir}"
echo "dst_dir = ${dst_dir}"

FILES_TO_INCLUDE=(\
	"${src_dir}/jerome/scripting/startup.js" \
	"${src_dir}/jerome/scripting/utteranceCompiler.js" \
	"${src_dir}/3rdParty/SCION/dist/scion.js" \
	"${src_dir}/jerome/ir/report/HTMLReporter.xslt"\
	)

mkdir -p "${dst_dir}"

for f in "${FILES_TO_INCLUDE[@]}"
do
	newName=`basename "$f"`
	newName="${dst_dir}/$newName.inc"
	echo -n 'u8R"=====(' > "$newName"
	cat "$f" >> "$newName"
	echo -n ')====="' >> "$newName"
done





 