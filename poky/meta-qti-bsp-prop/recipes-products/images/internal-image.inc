# Additional internal functions to run on images

# Add task to generate sdk image on need basis
python __anonymous () {
    if (d.getVar('SDK_IMAGE', True) == '1'):
        bb.build.addtask('do_populate_sdk', 'do_build', 'do_rootfs', d)
        bb.build.addtask('do_move_sdk', 'do_build', 'do_populate_sdk', d)
}

python do_move_sdk() {
    import subprocess
    cmd = "mv ${SDK_DEPLOY} ${DEPLOY_DIR_IMAGE}"
    subprocess.call(cmd, shell=True)
}
