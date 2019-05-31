DESCRIPTION = "This recipe packages FEAT_BIN_QSF distribution"
AUTHOR = "Dhruv Kumar Chawda (dchawda@qti.qualcomm.com)"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PV = "1.0"
PR = "r1"
#S = "${WORKDIR}"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://vendor/qcom/proprietary/drones/drones-prebuilts/snav-oem_1.2.46.1_8x96.zip"
SRC_URI  += "file://vendor/qcom/proprietary/drones/drones-prebuilts/mv_1.1.3_external.zip"
SRC_URI  += "file://vendor/qcom/proprietary/drones/drones-prebuilts/DG"

SRC_DIR   = "${WORKSPACE}/vendor/qcom/proprietary/drones/drones-prebuilts"
PACKAGES = "${PN}"

ALLOW_EMPTY_${PN} = "1"

INSANE_SKIP_${PN} = "dev-so"
INSANE_SKIP_${PN} += "installed-vs-shipped"

do_install() {
   # Copy the pre-built debian packages from the files directory to the output location
	prebuilt_dest_mv=${COREBASE}/../FEAT-API-LNX-MV/apps_proc
	install -d ${prebuilt_dest_mv}
	cp -rf ${WORKDIR}/app ${prebuilt_dest_mv}
	cp -rf ${WORKDIR}/doc ${prebuilt_dest_mv}
	cp -rf ${WORKDIR}/bin ${prebuilt_dest_mv}
	cp -rf ${WORKDIR}/inc ${prebuilt_dest_mv}
	cp -rf ${WORKDIR}/*.txt ${prebuilt_dest_mv}

	prebuilt_dest_snav=${COREBASE}/../FEAT-API-LNX-QSF/apps_proc
	install -d ${prebuilt_dest_snav}

	install -m 0644 ${WORKDIR}/pkg/snav*.ipk -D ${prebuilt_dest_snav}
	cp -rf ${WORKDIR}/root ${prebuilt_dest_snav}
	cp -rf ${WORKDIR}/supplement ${prebuilt_dest_snav}
	#install -m 0644 ${FILE_DIRNAME}/files/libmv*.ipk -D ${prebuilt_dest}
	#install -m 0644 ${FILE_DIRNAME}/files/*.Linux -D ${prebuilt_dest}

	# Copy the pre-built debian packages from the files directory to the output location
	prebuilt_dest_dg=${COREBASE}/../FEAT-API-LNX-DG/apps_proc/DG
	install -d ${prebuilt_dest_dg}
	cp -rf ${WORKDIR}/vendor/qcom/proprietary/drones/DG/API ${prebuilt_dest_dg}
	cp -rf ${WORKDIR}/vendor/qcom/proprietary/drones/DG/lib ${prebuilt_dest_dg}

}

INSANE_SKIP_${PN} += "arch"
INSANE_SKIP_${PN} += "textrel"
INSANE_SKIP_${PN} += "libdir"
INSANE_SKIP_${PN} += "ldflags"
