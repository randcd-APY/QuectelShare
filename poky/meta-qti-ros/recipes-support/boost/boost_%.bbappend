# This bbappend is a way to modify poky's default boost recipe in meta/recipe-support
# from version 1.58.0 to 1.59.0, while re-using almost all of the original poky recipe. 
# Luckily, the 1.58.0 recipe works fine for 1.59.0 unmodified, so all we need to do 
# in this file is to set the PV and PR, and associated changes.
# We also print a warning to alert users we're changing the version.
# This change is needed because OMPL doesn't compile because of a known bug in 1.58.0

PV = "1.59.0"
PR = "r0"

BOOST_VER = "${@"_".join(d.getVar("PV",1).split("."))}"
BOOST_MAJ = "${@"_".join(d.getVar("PV",1).split(".")[0:2])}"
BOOST_P = "boost_${BOOST_VER}"

SRC_URI = "${SOURCEFORGE_MIRROR}/boost/${BOOST_P}.tar.bz2"

SRC_URI[md5sum] = "6aa9a5c6a4ca1016edd0ed1178e3cb87"
SRC_URI[sha256sum] = "727a932322d94287b62abb1bd2d41723eec4356a7728909e38adb65ca25241ca"

do_fetch_prepend(){
    bb.warn('NOTE: bbappend overrides boost recipe\'s version and sets it to ' + d.getVar('PV', True))
}
