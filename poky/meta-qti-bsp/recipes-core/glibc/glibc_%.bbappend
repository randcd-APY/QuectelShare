# glibc expects -fstack-protector optimization passed as a configuration option
# instead of a top level build flag. 

python __anonymous () {
    sel_opt = d.getVar("SELECTED_OPTIMIZATION", True).split()

    for opt in sel_opt:
        if opt in ("-fstack-protector", "-fstack-protector-all", "-fstack-protector-strong"):
            # bb.note("%s can't be built with %s" % (d.getVar('PN'), sel_opt))
            sel_opt.remove(opt)
    d.setVar('SELECTED_OPTIMIZATION', ' '.join(sel_opt))
}

EXTRA_OECONF += "${@bb.utils.contains('FULL_OPTIMIZATION', '-fstack-protector', '--enable-stack-protector=yes', '', d)}"
EXTRA_OECONF += "${@bb.utils.contains('FULL_OPTIMIZATION', '-fstack-protector-all', '--enable-stack-protector=all', '', d)}"
EXTRA_OECONF += "${@bb.utils.contains('FULL_OPTIMIZATION', '-fstack-protector-strong', '--enable-stack-protector=strong', '', d)}"
