#!/bin/bash
# update $PKG.dsc and put the rest of this directory into debian.tar.gz.
# Those files are used by build.opensuse.org's Open Build Service.
# After the files are updated, they need to be separately committed to git.
# Running this script should be part of the cvmfs release procedure.

HERE="`dirname $0`"
cd $HERE
ME="`basename $0`"
PKG="`sed -n 's/^Source: //p' $HERE/control.in`"
SPECFILE="../../rpm/${PKG}-universal.spec"
VERSION="$(grep ^Version: $SPECFILE | awk '{print $2}')"
RPMREL="$(grep '^%define release_prefix' $SPECFILE | awk '{print $3}')"
if [ -z "$RPMREL" ]; then
    RPMREL="$(grep '^Release:' $SPECFILE | awk '{print $2}' | cut -d% -f1)"
fi
(
echo "# created by $ME, do not edit by hand"
# The following two lines are OBS "magic" to use the tarball from the rpm
echo "Debtransform-Tar: ${PKG}-${VERSION}.tar.gz"
#echo "Debtransform-Files-Tar: "
echo "Format: 1.0"
echo "Version: ${VERSION}-${RPMREL}"
echo "Binary: $PKG"
# this ignores more customization done in ../../../ci/cvmfs/deb.sh
cat control.in|awk '/#FUSE3-BEGIN/{flag=1;next}/#FUSE3-END/{flag=0;next} !flag'
echo "Files:"
echo "  ffffffffffffffffffffffffffffffff 99999 file1"
echo "  ffffffffffffffffffffffffffffffff 99999 file2"
) > $PKG.dsc
#
tar czf debian.tar.gz --exclude $PKG.dsc --exclude debian.tar.gz --exclude obsupdate.sh --exclude source *
echo "Updated $HERE/$PKG.dsc and $HERE/debian.tar.gz; check them into git"
