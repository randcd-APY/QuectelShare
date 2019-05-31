SUMMARY = "Lightweight and flexible command-line JSON processor"
HOMEPAGE = "https://stedolan.github.io/jq"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://COPYING;md5=29dd0c35d7e391bb8d515eacf7592e00"

SRC_URI = "\
	http://github.com/stedolan/jq/releases/download/jq-1.5/jq-1.5.tar.gz;name=tarball \
	https://github.com/stedolan/jq/commit/8eb1367ca44e772963e704a700ef72ae2e12babd.patch;name=patch1;striplevel=2 \
	https://github.com/stedolan/jq/commit/83e2cf607f3599d208b6b3129092fa7deb2e5292.patch;name=patch2;striplevel=2 \
	"

SRC_URI[tarball.md5sum] = "0933532b086bd8b6a41c1b162b1731f9"
SRC_URI[tarball.sha256sum] = "c4d2bfec6436341113419debf479d833692cc5cdab7eb0326b5a4d4fbe9f493c"

SRC_URI[patch1.md5sum] = "5453ead1fa3d22411eec64c61e202a2c"
SRC_URI[patch1.sha256sum] = "5235b8bbf6e6939099212ca7d6885d9450abb3602ca725ae3dad4153ab5ff240"

SRC_URI[patch2.md5sum] = "6a42ab30f06d3aab2f6ae58d8c3f6e3e"
SRC_URI[patch2.sha256sum] = "5a894de730b7bda927ada8d08a935f6fc85b6509667dc554b18ea9d8cecb61a8"

inherit autotools
