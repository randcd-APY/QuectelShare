FILESEXTRAPATHS_append := "${THISDIR}:${THISDIR}/patches:"

SRC_URI += "file://add-userdebug_or_eng-macro.patch \
            file://Assign-proc_t-to-proc-dir.patch \
            file://0004-selinux-update-the-getty.te-for-addressing.patch \
            file://mls-enabling-sysadmin-role-for-the-ssh-login.patch \
            file://Add-lib-busybox-to-subs_dist.patch \
            file://common/ \
            file://${BASEMACHINE}/ \
"

PROVIDES += "sepolicy"
RPROVIDES_${PN} += "sepolicy"

POLICY_MONOLITHIC = "${@base_conditional('BASEMACHINE', 'sdxpoorwills', 'y', 'n', d)}"

EXTRA_OEMAKE += "USER_BUILD=${@base_conditional('USER_BUILD', '1', 'y', 'n', d)}"
EXTRA_OEMAKE += "SETFILES=${RECIPE_SYSROOT_NATIVE}/sbin/setfiles"

# Contrib modules not declared here are turned off
CONTRIB_MODULES ?= "chronyd networkmanager shutdown gnome dhcp avahi apache minidlna mta dbus"

def append_policy_file(src_path, dst_path):
    # Append our policy fragment to the end of the upstream file
    with open(src_path, 'r') as src_file:
        with open(dst_path, 'a') as dst_file:
            dst_file.write('\n')
            for line in src_file.readlines():
                if not line.startswith('#'):
                    dst_file.write(line)
    os.remove(src_path)

def copy_policy_file(src_path, dst_path):
    # Copy the new file into the sepolicy compilation directory
    os.renames(src_path, dst_path)

def install_device_policy(d, src_root):
    src_dir = os.path.join(d.getVar("WORKDIR"), src_root)
    dst_dir = os.path.join(d.getVar("S"), "policy", "modules")
    for root, dirs, files in os.walk(src_dir):
        for file in files:
            policy_file = os.path.relpath(os.path.join(root, file), src_dir)
            dst_path = os.path.join(dst_dir, policy_file)
            if os.path.exists(dst_path):
                append_policy_file(os.path.join(src_dir, policy_file), dst_path)
            else:
                copy_policy_file(os.path.join(src_dir, policy_file), dst_path)

do_patch_append() {
    install_device_policy(d, "common")
    if os.path.exists(os.path.join(d.getVar("WORKDIR"), d.getVar("BASEMACHINE"))):
        install_device_policy(d, d.getVar("BASEMACHINE"))
}

python do_update_modules_conf () {
    contrib_modules = d.getVar("CONTRIB_MODULES").split()
    module_conf_name = os.path.join(d.getVar("S"), "policy", "modules.conf")
    new_file = ""

    with open(module_conf_name, 'r') as modules_file:
        layer = None
        for line in modules_file.readlines():
            if line.startswith('# Layer'):
                layer = line.split(':')[-1].strip()
            elif '=' in line:
                module_name = line.split('=')[0].strip()
                if layer == "contrib" and module_name not in contrib_modules:
                    new_file += module_name + ' = off\n'
                    continue
            new_file += line

    # Save the updated modules.conf
    with open(module_conf_name, 'w') as modules_file:
        modules_file.write(new_file)
}
addtask update_modules_conf after do_patch before do_compile

do_install_sdxpoorwills() {
    oe_runmake 'DESTDIR=${D}' 'prefix=${D}${prefix}' install
    install_misc_files
    install_config
}
