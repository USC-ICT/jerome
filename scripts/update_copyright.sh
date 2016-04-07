#!/bin/sh

#  update_copyright.sh
#  jerome
#
#  Created by Anton Leuski on 9/30/15.
#  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.

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
