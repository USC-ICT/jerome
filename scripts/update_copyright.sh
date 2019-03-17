#!/bin/sh

#  update_copyright.sh
#  jerome
#
#  Created by Anton Leuski on 9/30/15.
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
PREFIX=`cat < $2`

# will replace everything starting from
# //  Copyright
# until the last comment line
# //
# with the content of file $2

update ()
{
  local dir=$1
  local f

  for f in `ls "${dir}"`
  do
    local file_path="${dir}/$f"
    if [ -d "${file_path}" ]
    then
      update "${file_path}"
    else
      echo ${file_path}
      perl -pi -e "\$prefix=\"$PREFIX\";" -e 'BEGIN{undef $/;} s/^((?:(?:\/\*.*?\*\/)|(?:\/\/.*?\n)|\s)*)\/\/  Copyright.*?\n(\/\/.*?\n)*/$1${prefix}\n/s;' ${file_path}
    fi
  done
}

update $1
