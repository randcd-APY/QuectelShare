# For dependencies that are only needed during compilation and not at runtime,
# i.e. dependencies that do not exist when using prebuilt binaries, e.g. static libraries,
# use SDEPENDS instead of DEPENDS.

inherit qprebuilt

python __anonymous() {
    srcdir = d.getVar('SRC_DIR', True)
    if srcdir:
        if (os.path.isdir(srcdir)):
            # Add build-only dependencies
            depends = [d.getVar("DEPENDS", True), d.getVar("SDEPENDS", True)]
            d.setVar("DEPENDS", " ".join(filter(None, depends)))
}
