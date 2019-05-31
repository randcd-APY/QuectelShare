SUMMARY = "AllPlay package groups"
LICENSE = "CLOSED"

PR="r2"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit packagegroup

PACKAGES = " \
	packagegroup-allplay \
	"

# Audio HAL and Bluetooth lib are currently added as 32 bits libraries
# so use 32-bits orbplayer and sam-system (moreover the 64-bits versions
# have never been tested)
RDEPENDS_packagegroup-allplay = " \
	${@base_contains('MULTILIB_VARIANTS', 'lib32', 'lib32-orbplayer', 'orbplayer', d)} \
	${@base_contains('MULTILIB_VARIANTS', 'lib32', 'lib32-sam-system', 'sam-system', d)} \
	web-ui \
	iperf \
	${@base_contains('MULTILIB_VARIANTS', 'lib32', 'lib32-nflc', 'nflc', d)} \
	"
