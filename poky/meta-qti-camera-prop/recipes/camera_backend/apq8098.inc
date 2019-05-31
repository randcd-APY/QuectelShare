#only add global config options which are suitable for all camera modules

EXTRA_OECONF += "DISABLE_FASTCV=true"
EXTRA_OECONF += "ENABLE_C2D=false"
#depends on adreno module
EXTRA_OECONF += "DISABLE_ADRENO_GPU=true"
EXTRA_OECONF += "DISABLE_MEDIA_OMX=true"
EXTRA_OECONF += "CHROMATIX_VERSION=0310"