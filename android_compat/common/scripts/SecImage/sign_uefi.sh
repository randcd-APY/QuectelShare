#!/bin/bash


dir="$(date +"%Y%m%d-%H%M%S")"

python sectools_builder.py \
	-i ./engg_uefi_build/unsigned_input/uefi.elf \
	-t ./engg_uefi_build/builder_output_uefi \
	-g uefi \
	-o ./engg_uefi_build/install_uefi \
	--soc_hw_version=0x60020000 \
	--soc_vers=0x6002 \
	--config=./config/integration/secimage_eccv3.xml \
	--build_policy_id=USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN

mkdir -p ./engg_uefi_build/"$dir"
cp ./engg_uefi_build/install_uefi/uefi.elf ./engg_uefi_build/"$dir"
