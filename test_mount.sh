#!/bin/sh

set -e

REPO=$1
CVMFS_BUILD_DIR=${CVMFS_BUILD_DIR:-$PWD}
CVMFS_SOURCE_DIR=$(grep CernVM-FS_SOURCE_DIR "${CVMFS_BUILD_DIR}/CMakeCache.txt" | cut -d= -f2)
CVMFS_TEST_WORKSPACE=${CVMFS_TEST_WORKSPACE:-/tmp/cvmfs-test}
FS_OPTIONS=${FS_OPTIONS:-}
CVMFS_ARGS=${CVMFS_ARGS:-}
CVMFS_SERVER_URL=${CVMFS_SERVER_URL:-http://cvmfs-stratum-one.cern.ch/cvmfs/@fqrn@}

export CVMFS_LIBRARY_PATH=${CVMFS_BUILD_DIR}/cvmfs

mkdir -p "${CVMFS_TEST_WORKSPACE}/mnt/$REPO" \
  "${CVMFS_TEST_WORKSPACE}/cache" \
  "${CVMFS_TEST_WORKSPACE}/workspace" \
  "${CVMFS_TEST_WORKSPACE}/conf"

cat << EOF > "${CVMFS_TEST_WORKSPACE}/conf/default.conf"
CVMFS_WORKSPACE=${CVMFS_TEST_WORKSPACE}/workspace
CVMFS_CACHE_BASE=${CVMFS_TEST_WORKSPACE}/cache
CVMFS_CLAIM_OWNERSHIP=yes
CVMFS_KEYS_DIR=${CVMFS_SOURCE_DIR}/mount/keys
CVMFS_USYSLOG=${CVMFS_TEST_WORKSPACE}/cvmfs.log
CVMFS_SERVER_URL=${CVMFS_SERVER_URL}
CVMFS_PAC_URLS=http://cernvm-wpad.cern.ch/wpad.dat
CVMFS_HTTP_PROXY=auto
CVMFS_RELOAD_SOCKETS=${CVMFS_TEST_WORKSPACE}/workspace
CVMFS_INSTRUMENT_FUSE=yes
EOF
touch "${CVMFS_TEST_WORKSPACE}/conf/default.local"

if [ "x$FS_OPTIONS" != "x" ]; then
  FS_OPTIONS="${FS_OPTIONS},"
fi

${CVMFS_BUILD_DIR}/cvmfs/cvmfs2 $CVMFS_ARGS \
  -o ${FS_OPTIONS}config=${CVMFS_TEST_WORKSPACE}/conf/default.conf:${CVMFS_TEST_WORKSPACE}/conf/default.local \
  $REPO "${CVMFS_TEST_WORKSPACE}/mnt/$REPO"