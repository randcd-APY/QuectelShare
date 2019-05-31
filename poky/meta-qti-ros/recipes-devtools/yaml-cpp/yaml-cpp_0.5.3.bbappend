# This append file updates the md5 and sha for the yaml-cpp download
S = "${WORKDIR}/yaml-cpp-release-${PV}"

SRC_URI = "https://github.com/jbeder/yaml-cpp/archive/release-${PV}.tar.gz"
SRC_URI[md5sum] = "e2507c3645fc2bec29ba9a1838fb3951"
SRC_URI[sha256sum] = "3492d9c1f4319dfd5588f60caed7cec3f030f7984386c11ed4b39f8e3316d763"
