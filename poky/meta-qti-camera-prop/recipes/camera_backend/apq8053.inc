#only add global config options which are suitable for all camera modules

EXTRA_OECONF += "DISABLE_FASTCV=false"
EXTRA_OECONF += "ENABLE_C2D=true"
#depends on adreno module
EXTRA_OECONF += "DISABLE_ADRENO_GPU=false"
EXTRA_OECONF += "DISABLE_MEDIA_OMX=false"
EXTRA_OECONF += "CHROMATIX_VERSION=0309"