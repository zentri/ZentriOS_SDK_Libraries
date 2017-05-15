#!/bin/bash
#
# EM DRV release package script
#
# This script needs to be run from s025_sw/emdrv
# example: ~> cd s025_sw/emdrv
#          ~> ./scripts/gendoc.sh
#

PATH=${PATH}:../utils/bin/dot

#
# Generate doxygen documentation
#
function MakeDoxygenDocs
{
  echo -e "\033[0;33;40mGenerating doxygen docs\033[0m"

  # Prepare list of INPUT source files
  REPL=$(printf "%s" "INPUT = $DRIVER_MEMBERS")
  # Replace / with \
  REPL=$(echo $REPL | sed -e 's,/,\\,g')
  # Replace \ with \\
  REPL=$(echo $REPL | sed -e 's,\\,\\\\,g')

  rm -rf emdrvdoc
  mkdir emdrvdoc
  cat ./scripts/doxygen_template.config \
      | sed -e "s/INPUT                  =/$REPL/" \
      | sed -e 's/PROJECT_NUMBER         =/PROJECT_NUMBER\ \ =\ '$1'-'$2' /' \
      > ./scripts/doxygen.config
  if [ ! -f ./scripts/doxygen.config ]; then
    echo -e "\033[1;31;40mMissing doxygen configuration file\033[0m"
    exit 1
  fi

  ../utils/bin/doxygen-1.6.3.exe ./scripts/doxygen.config
  if [ $? -ne 0 ]; then
    echo -e "\033[1;31;40mDoxygen failure\033[0m"
    exit 1
  fi
  cp -r ../utils/doxyhtml/* emdrvdoc/html/

  #
  # Create Javascript to check if file exists
  #
  echo "var EMDRV = true;" > emdrvdoc/html/__existence__.js 2>&1
  rm ./scripts/doxygen.config
}

DRIVER_NAME=$1
shift
DRIVER_REVISION=$1
shift
DRIVER_MEMBERS=""

while [ "$1" ]
do
  DRIVER_MEMBERS="$DRIVER_MEMBERS $1"
  shift
done

MakeDoxygenDocs $DRIVER_NAME $DRIVER_REVISION
