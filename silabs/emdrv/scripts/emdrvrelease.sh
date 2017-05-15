#!/bin/bash
#
# EMDRV release package script
#
# k.andersen@energymicro.com
#
# This script needs to be run from super/platform/emdrv
# example: ~> cd super/platform/emdrv
#          ~> ./scripts/emdrvrelease.sh 1.0.0
#

RELEASE_NAME="gecko_sdk_release"
DRIVERS="nvm gpiointerrupt sleep rtcdrv spidrv uartdrv ustimer dmadrv ezradiodrv tempdrv"
EXTRAFILES="
common/inc/ecode.h
config/rtcdrv_config.h
config/spidrv_config.h
config/nvm_config.h
config/nvm_config.c
config/ustimer_config.h
config/uartdrv_config.h
config/dmadrv_config.h
config/tempdrv_config.h
config/ezradiodrv_config.h
"

function Help
{
  echo -e "\033[0;33;40mUsage: emdrvrelease.sh <VERSION>\033[0m"
  echo -e "\033[0;33;40m       Example: emdrvrelease.sh 2.4.1\033[0m"
  echo
  echo -e "\033[0;33;40m   or:\033[0m"
  echo -e "\033[0;33;40m       emdrvrelease.sh --test for testrun\033[0m"
  exit 1
}

#
# Verify repo is clean
#
function VerifyGitClean
{
  CLEAN=`git status | grep modified`
  if [ "${CLEAN}" != "" ]; then
    if [ "${TESTRUN}" == "1" ]; then
      echo -e "\033[1;31;40mWarning: Git repo is modified, ignored now...!!!\033[0m"
    else
      echo -e "\033[1;31;40mError: Git repo is modified, need clean repo to start\033[0m"
      exit 1
    fi
  fi
}

#
# Verify correct path for running script
#
function VerifyCurrentPath
{
  CWD=`pwd`
  if [ `basename ${CWD}` != "emdrv" ]; then
    echo -e "\033[1;31;40mError: This script must be run from \"emdrv\" as, ./scripts/emdrvrelease.sh <VERSION>\033[0m"
    exit 1
  fi
}

#
# Verify Revision and Changes file
#
function VerifyRevision
{
  CHANGES=`grep ${VERSION}: Changes-emdrv.txt`
  if [ "${TESTRUN}" == "0" ]; then
    if [ "${CHANGES}" == "" ]; then
      echo -e "\033[1;31;40mError: Missing information in Changes-emdrv.txt about version ${VERSION}\033[0m"
      echo -n -e "\033[1;31;40m       Latest version in Changes file is\033[0m "
      grep ":" Changes-emdrv.txt | head -1 | sed -e 's/://'
      exit 1
    fi
    if [ `git tag -l | grep ${RELEASE_NAME}_${VERSION}` ]; then
      echo -e "\033[1;31;40mError: ${RELEASE_NAME}_${VERSION} already tagged (and released?)\033[0m"
      exit 1
    fi
  fi
}

#
# Verify existence of readme file
#
function VerifyReadme
{
  if [ ! -f Readme-emdrv.txt ]; then
    echo -e "\033[1;31;40mError: No Readme-emdrv.txt file is present\033[0m"
      exit 1
  fi
}

#
# Make sure repo is Clean
#
function CleanRepo
{
  if [ "${TESTRUN}" == "0" ]; then
    echo -e "\033[0;33;40mCleaning repo\033[0m"
    git clean -dxf
    git checkout -- '*.*'
  fi
}

#
# Make autogen result directory
#
function MakeAutogenDir
{
  if [ ! -d autogen ]; then
    mkdir -p autogen
  fi
}

#
# Update version information in files
#
function UpdateVersionNumbers
{
  if [ "${TESTRUN}" == "0" ]; then
    echo -e "\033[0;33;40mUpdating version information to ${VERSION}\033[0m"

    for x in ${DRIVERS}; do
      # Special treatment for "ezradiodrv"
      if [ "${x}" == "ezradiodrv" ]; then
        find ${x}/common -name \*.[ch] | xargs -n1  ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/config -name \*.[ch] | xargs -n1  ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/plugins -name \*.[ch] | xargs -n1 ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/si4455 -name \*.[ch] | xargs -n1  ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/si4460 -name \*.[ch] | xargs -n1  ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/si4461 -name \*.[ch] | xargs -n1  ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/si4463 -name \*.[ch] | xargs -n1  ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/si4468 -name \*.[ch] | xargs -n1  ../../tool/emtool/bin/fixversion.pl ${VERSION}
      else
        find ${x}/src -name \*.c | xargs -n1 ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/inc -name \*.h | xargs -n1 ../../tool/emtool/bin/fixversion.pl ${VERSION}
        find ${x}/config -name \*.h | xargs -n1 ../../tool/emtool/bin/fixversion.pl ${VERSION} || true
      fi
    done

    for x in ${EXTRAFILES}; do
      ../../tool/emtool/bin/fixversion.pl ${VERSION} ${x}
    done

  fi
}

#
# Pack ZIP file together
#
function MakeZIPFile
{
  echo
  echo -e "\033[0;33;40mPacking ZIP file\033[0m"
  # Remove old file
  rm -rf autogen/EMDRV_${VERSION}.zip
  # Generate new file
  (cd ../..; zip -qr platform/emdrv/autogen/EMDRV_${VERSION}.zip \
      platform/emdrv/Changes* \
      platform/emdrv/Readme*  \
      platform/emdrv/Silabs_License_Agreement* )

  for x in ${DRIVERS}; do
    # Special treatment for "ezradiodrv"
    if [ "${x}" == "ezradiodrv" ]; then
      (cd ../..; zip -ru platform/emdrv/autogen/EMDRV_${VERSION}.zip \
        platform/emdrv/${x}/common/* \
        platform/emdrv/${x}/config/* \
        platform/emdrv/${x}/plugins/* \
        platform/emdrv/${x}/si4455/* \
        platform/emdrv/${x}/si4460/* \
        platform/emdrv/${x}/si4461/* \
        platform/emdrv/${x}/si4463/* \
        platform/emdrv/${x}/si4468/* )
    else
      (cd ../..; zip -u platform/emdrv/autogen/EMDRV_${VERSION}.zip \
        platform/emdrv/${x}/config/*.* \
        platform/emdrv/${x}/inc/*.* \
        platform/emdrv/${x}/src/*.* )
    fi
  done

  for x in ${EXTRAFILES}; do
    (cd ../..; zip -u platform/emdrv/autogen/EMDRV_${VERSION}.zip platform/emdrv/${x} )
  done
  echo
}

function CleanUp
{
  :
}

#
# Main script
#
ARGS=$*
EXTRAARGS="0"
TESTRUN="0"

if [ $# -lt 1 ]; then
  Help
fi

#
# Parse command line arguments
#
for x in ${ARGS} ; do

  if [ "${x}" == "--test" ]; then
    TESTRUN="1"
    VERSION="9.9.9"
    echo -e "\033[0;32;40mTest building EMDRV Release Version ${VERSION}\033[0m"
    EXTRAARGS="1"
  fi

  if [ "${x}" == "--help" ]; then
    Help
  fi
done

if [ "${EXTRAARGS}" == "0" ]; then
  VERSION=$1
fi

VerifyCurrentPath
VerifyGitClean
VerifyRevision
VerifyReadme
CleanRepo
MakeAutogenDir
UpdateVersionNumbers
MakeZIPFile
CleanUp

#
# Instruct about manual procedure for tagging and releasing
#
echo -e "\033[0;32;40mNow perform the following manual procedure\033[0m"
echo -e "1) Move the \033[0;32;40mautogen/EMDRV_${VERSION}.zip\033[0m into T:\\\03 Research & Development\\\03050 EFM32 Software Support\\\030533 energyAware Drivers"
echo "2) Tag these files and this version:"
echo "   git commit -am \"EMDRV files updated to version ${VERSION}\""
echo "   git push"
echo "   git tag -a -m \"${RELEASE_NAME}_${VERSION} energyAware Drivers Package\" ${RELEASE_NAME}_${VERSION}"
echo "   git push --tags"
echo "   git checkout -- '*.*'"
