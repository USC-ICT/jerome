#! /bin/sh

#  make_includes.sh
#  jerome
#
#  Created by Anton Leuski on 9/24/15.
#  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
#
#  This file is part of Jerome.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

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
	printf 'u8R"=====(' > "$newName"
	cat "$f" >> "$newName"
	printf ')====="' >> "$newName"
done





 
