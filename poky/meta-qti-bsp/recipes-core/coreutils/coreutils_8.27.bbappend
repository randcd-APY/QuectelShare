do_install_append_mdm(){
	mv ${D}${base_bindir}/cp.coreutils ${D}/cp.coreutils;
	mv ${D}${bindir}/chcon ${D}/chcon;
	rm -rf ${D}${base_bindir};
	rm -rf ${D}/usr;
	install -d ${D}${base_bindir};
	install -d ${D}${bindir};
	mv ${D}/cp.coreutils ${D}${base_bindir}/cp.coreutils;
	mv ${D}/chcon ${D}${bindir}/chcon;
}
FILES_${PN}_qcs403-som2 = "${base_bindir}/*  ${bindir}/chcon.coreutils"
