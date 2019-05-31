LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://README.md;md5=6f915b762027ee018a62c9ebbd8fc762"
DEPENDS = "openssl"

SRCREV="v14.12"
SRC_URI = " \
	git://git.allseenalliance.org/gerrit/core/ajtcl.git;protocol=https;destsuffix=ajtcl-${PV}.00-src \
	file://001-Connect_to_localhost.patch \
	file://002-Fix_build_on_le.patch \
	"

S = "${WORKDIR}/ajtcl-${PV}.00-src"

inherit scons

EXTRA_OESCONS = "\
	CC="${CC}" \
	CXX="${CXX}" \
	LD="${LD}" \
	AR="${AR}" \
	CFLAGS="${CFLAGS}" \
	CXXFLAGS="${CXXFLAGS}" \
	LINKFLAGS="${LDFLAGS}" \
	STAGING_INCDIR="${STAGING_INCDIR}" \
	TARG=linux \
	WS=off \
	VARIANT=release \
	"

do_install() {
	install -d ${D}${includedir}
	install -m 0644 ${S}/inc/*.h ${D}${includedir}
	install -m 0644 ${S}/target/linux/*.h ${D}${includedir}
	install -m 0644 ${S}/external/sha2/sha2.h ${D}${includedir}

	install -d ${D}${libdir}
	install -m 0755 ${S}/libajtcl.so ${D}${libdir}
}

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
