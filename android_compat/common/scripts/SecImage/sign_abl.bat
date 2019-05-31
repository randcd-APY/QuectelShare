python sectools_builder.py -v -i ./engg_ABL_build/unsigned_input/abl.elf -t ./engg_ABL_build/builder_output_abl -g abl -o ./engg_ABL_build/install_abl --soc_hw_version=0x60020000 --soc_vers=0x6002 --config=.\config\integration\secimage_eccv3.xml --build_policy_id=USES_SEC_POLICY_MULTIPLE_DEFAULT_SIGN

cp ./engg_ABL_build/install_abl/abl.elf ./engg_ABL_build/