#! /bin/sh

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
