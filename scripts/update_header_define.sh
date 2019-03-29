#! /bin/sh

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

SOURCE_DIR="$1"
PREFIX="$2"

update ()
{
  local dir=$1
  local prefix=$2
  local f
  
  for f in `ls "${dir}"`
  do
    local file_path="${dir}/$f"
    if [ -d "${file_path}" ]
    then
      update "${file_path}" "${prefix}_$f"
    else
      local name=`basename -s .hpp $f`
      if [ $name != $f ]
      then
        file_prefix="${prefix}_${name}_hpp__"
        
        perl -pi -e "\$prefix=\"$file_prefix\";" -e 'BEGIN{undef $/;} s/^((?:(?:\/\*.*?\*\/)|(?:\/\/.*?\n)|\s)*)#ifndef\s+(?:\w+)\s*#define.*?\n(.*)#endif(?:(?:(?:\/\*.*?\*\/)|(?:\/\/.*?\n)|\s)*)$/$1#ifndef ${prefix}\n#define ${prefix}\n$2#endif \/\/ defined ${prefix}\n/sg;' ${file_path}
      fi
    fi
  done
}

update $1 $2
