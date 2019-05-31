def get_img_inc_file(d):
    product     = d.getVar('PRODUCT', True)
    basemachine = d.getVar('BASEMACHINE', True)
    if product != 'base' or '':
        inc_file_name = basemachine + "-" + product + "-image.inc"
    else:
        inc_file_name = basemachine + "-" + "base-image.inc"
    img_inc_file_path = os.path.join(d.getVar('THISDIR'), basemachine, inc_file_name)
    if os.path.exists(img_inc_file_path):
        img_inc_file = inc_file_name
    else:
        img_inc_file = basemachine + "-base-image.inc"
    return img_inc_file

include ${BASEMACHINE}/${@get_img_inc_file(d)}
