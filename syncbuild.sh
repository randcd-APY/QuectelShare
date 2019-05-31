#!/bin/bash
#
# Build Linux Base release
#

set -o errexit

# Configurable system utilities.
CURL="/usr/bin/curl"
REPO_DIST="https://gerrit.googlesource.com/git-repo"
REPO_PATH="${HOME}/bin"
REPO="${REPO_PATH}/repo"
BUILDID="$1"
if [ -z "$CAF_URL" ]; then
  if [ ${BUILDID:0:11} == "LE.UM.2.4.1" ]; then
    CAF_URL="ssh://git@git.codeaurora.org:9222/external/private_CAF_2.4.1/le/manifest.git"
  else
    CAF_URL="git://codeaurora.org/quic/le/le/manifest.git"
  fi
fi
URL="$CAF_URL"

if [ -z "$BRANCH" ]; then
  BRANCH="release"
fi
TARGET="$2"
JOB=8

ERR_MSG="A list of valid IDs can be found at https://source.codeaurora.org/quic/le/le/manifest/log/?h=release"

# Use BUILDID to set the versioned manifest.xml.
if [ -z "${BUILDID}" ]; then
  echo "Please supply a Build ID."
  echo "${ERR_MSG}"
  echo "Example Usage: ./syncbuild.sh IOT.LE.1.0-04802-8x53"
  exit 1
else
  MANIFEST="${BUILDID}.xml"
fi

# Install Repo.
mkdir -p "${REPO_PATH}"
if [ ! -f "${REPO}" ]; then
  ${CURL} "${REPO_DIST}" > "${REPO}"
fi
chmod +x "${REPO}"

if [ ! -d .repo ]; then
  ${REPO} init -u ${URL} -b ${BRANCH} -m ${MANIFEST}
fi

# Exit script if the manifest is not found.
if [ ! -f ".repo/manifests/${MANIFEST}" ]; then
  echo "ERROR: An Invalid Build ID was supplied."
  echo "${ERR_MSG}"
  exit 2
fi

${REPO} sync --no-tags -j ${JOB}

# Empty all the input arguments, this makes oe build fail otherwise.
while (( "$#" )); do
  shift
done

# Run build steps now.
if [ -f poky/build/conf/set_bb_env.sh ]; then
  cd poky
  . build/conf/set_bb_env.sh
fi

# Determine TARGET using BUILDID.
if [ ${BUILDID:18:4} == "8x53" ]; then
  TARGET=apq8053
elif [[ ${BUILDID} == *"9x07"* ]]; then
  TARGET=mdm9607
elif [[ ${BUILDID} == *"9x50"* ]]; then
  TARGET=mdm9650
elif [[ ${BUILDID} == *"SDX20"* ]]; then
  TARGET=sdx20
elif [[ ${BUILDID} == *"SDX24"* ]]; then
  TARGET=sdxpoorwills
elif [ ${BUILDID:18:4} == "8x96" ] && [ ${BUILDID:0:11} == "LV.HB.1.1.1" ]; then
  TARGET=8x96auto
elif [ ${BUILDID:18:4} == "8x96" ] && [ ${BUILDID:0:11} == "LV.HB.1.1.2" ]; then
  TARGET=8x96autofusion
elif [ ${BUILDID:18:9} == "8x96_cv2x" ] && [ ${BUILDID:0:11} == "LV.HB.0.0.1" ]; then
  TARGET=8x96autocv2x
elif [ ${BUILDID:18:4} == "8x96" ] && [ ${BUILDID:0:11} == "LE.UM.1.1.6" ]; then
  TARGET=msm8096-drone
elif [ ${BUILDID:18:4} == "8x96" ] && [ ${BUILDID:0:6} == "LE.UM." ]; then
  TARGET=apq8096
elif [ ${BUILDID:19:4} == "8x09" ] && [ ${BUILDID:0:12} == "LE.UM.1.1.10" ]; then
  TARGET=apq8009-drone
elif [ ${BUILDID:19:4} == "8x09" ] && [ ${BUILDID:0:10} == "IOT.LE.1.0" ]; then
  TARGET=msm8909-poky
elif [ ${BUILDID:21:4} == "8x09" ] && [ ${BUILDID:0:11} == "LE.UM.1.1.4" ]; then
  TARGET=msm8909
elif [ ${BUILDID:21:4} == "8x17" ] && [ ${BUILDID:0:11} == "LE.UM.1.2.8" ]; then
  TARGET=apq8017-qsap
elif [ ${BUILDID:18:4} == "8x09" ] && [ ${BUILDID:0:11} == "LE.UM.1.4.1" ]; then
  TARGET=apq8009-qsap
elif [ ${BUILDID:18:4} == "8x09" ] && [ ${BUILDID:0:11} == "LE.UM.2.3.1" ] || [ ${BUILDID:0:11} == "LE.UM.2.3.1.r1" ]; then
  TARGET=msm8909-quec-smart
elif [ ${BUILDID:19:4} == "8x98" ] && [ ${BUILDID:0:11} == "LE.UM.2.1.2" ]; then
  TARGET=apq8098
elif [ ${BUILDID:21:6} == "QCS605" ] && [ ${BUILDID:0:11} == "LE.UM.2.3.3" ]; then
  TARGET=qcs605-32-concam
elif [ ${BUILDID:18:6} == "qcs405" ] && [ ${BUILDID:0:11} == "LE.UM.2.4.1" ]; then
  TARGET=qcs405-som1-qsap
elif [ ${BUILDID:21:6} == "qcs405" ] && [ ${BUILDID:0:14} == "LE.UM.2.4.1.r1" ]; then
  TARGET=qcs405-som1-qsap
elif [ ${BUILDID:19:4} == "8x53" ] && [ ${BUILDID:0:12} == "LE.UM.1.3.r2" ] || [ ${BUILDID:0:12} == "LE.UM.1.3.c1" ]; then
  TARGET=apq8053-32
elif [ "${BUILDID}" == "caf_versioned" ]; then
  # If BUILDID is caf_versioned, then TARGET must be explicitly specified.
  if [ -z "${TARGET}" ]; then
    echo "ERROR: TARGET must be specified when BUILDID is caf_versioned"
    echo "ex. ./syncbuild.sh caf_versioned apq8053"
    exit 1
  fi
else
  echo "ERROR: Unknown Build ID"
  echo "${ERR_MSG}"
  exit 1
fi

# Set build command.
# Ex. if TARGET is apq8053, BUILD_CMD is build-8053-image.

if [ ${BUILDID:18:4} == "8x96" ] && [ ${BUILDID:0:6} == "LV.HB." ]; then
  BUILD_CMD="build-${TARGET}-image"
  echo "Build command: ${BUILD_CMD}"
else
  BUILD_CMD="build-"$(echo ${TARGET} | sed -r 's#(mdm|msm|apq)##')"-image"
  echo "Build command: ${BUILD_CMD}"
fi
${BUILD_CMD}
exit 0
