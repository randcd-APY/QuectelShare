python sectools_builder.py -v -i ./engg_uefi_build/unsigned_input/uefi.elf -t ./engg_uefi_build/builder_output_uefi -g uefi -o ./engg_uefi_build/install_uefi --soc_hw_version=0x60020000 --soc_vers=0x6002 --config=.\config\integration\secimage_eccv3.xml --build_policy_id=USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN

cp ./engg_uefi_build/install_uefi/uefi.elf ./engg_uefi_build/