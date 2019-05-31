SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=d25939bd242aba3a145910b448c43b6d"

DEPENDS = "catkin ecl-errors ecl-build ecl-formatters ecl-geometry ecl-license ecl-linear-algebra ecl-math ecl-devices ecl-sigslots ecl-command-line"

inherit catkin
require ecl-navigation.inc


