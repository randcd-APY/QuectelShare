# Override the necessary toolchain environment variables.
CC_toolchain-sdllvm  = "${STAGING_BINDIR_NATIVE}/llvm-arm-toolchain/bin/clang -target ${TARGET_SYS} ${HOST_CC_ARCH}${TOOLCHAIN_OPTIONS}"
CXX_toolchain-sdllvm = "${STAGING_BINDIR_NATIVE}/llvm-arm-toolchain/bin/clang++ -target ${TARGET_SYS} ${HOST_CC_ARCH}${TOOLCHAIN_OPTIONS}"
CPP_toolchain-sdllvm = "${STAGING_BINDIR_NATIVE}/llvm-arm-toolchain/bin/clang -target ${TARGET_SYS} -E${TOOLCHAIN_OPTIONS} ${HOST_CC_ARCH}"
CCLD_toolchain-sdllvm = "${STAGING_BINDIR_NATIVE}/llvm-arm-toolchain/bin/clang ${HOST_CC_ARCH}${TOOLCHAIN_OPTIONS}"

# For cmake
OECMAKE_C_COMPILER_toolchain-sdllvm   = "${STAGING_BINDIR_NATIVE}/llvm-arm-toolchain/bin/clang"
OECMAKE_CXX_COMPILER_toolchain-sdllvm = "${STAGING_BINDIR_NATIVE}/llvm-arm-toolchain/bin/clang++"
OECMAKE_C_FLAGS_toolchain-sdllvm     += " -target ${TARGET_SYS} ${HOST_CC_ARCH}${TOOLCHAIN_OPTIONS}"
OECMAKE_CXX_FLAGS_toolchain-sdllvm   += " -target ${TARGET_SYS} ${HOST_CC_ARCH}${TOOLCHAIN_OPTIONS}"
OECMAKE_EXTRA_ROOT_PATH_toolchain-sdllvm = "${STAGING_BINDIR_TOOLCHAIN}"

THUMB_TUNE_CCARGS_remove_toolchain-sdllvm = "-mthumb-interwork"
TUNE_CCARGS_remove_toolchain-sdllvm = "-meb"
TUNE_CCARGS_remove_toolchain-sdllvm = "-mel"
TUNE_CCARGS_append_toolchain-sdllvm = " -D__extern_always_inline=inline -no-integrated-as -Wno-error=unused-command-line-argument -Qunused-arguments"

# Remove unsupported compiler flags
FULL_OPTIMIZATION_remove_toolchain-sdllvm = "-fexpensive-optimizations"
FULL_OPTIMIZATION_remove_toolchain-sdllvm = "-frename-registers"
FULL_OPTIMIZATION_remove_toolchain-sdllvm = "-finline-functions"
FULL_OPTIMIZATION_remove_toolchain-sdllvm = "-finline-limit=64"
FULL_OPTIMIZATION_remove_toolchain-sdllvm = "-Wno-error=maybe-uninitialized"
# Suppress some common build warnings
FULL_OPTIMIZATION_append_toolchain-sdllvm = " -Wno-error=uninitialized"
FULL_OPTIMIZATION_append_toolchain-sdllvm = " -Wno-unused-variable"
FULL_OPTIMIZATION_append_toolchain-sdllvm = " -Wno-unused-private-field"
FULL_OPTIMIZATION_append_toolchain-sdllvm = " -Wno-error=undefined-optimized"

# choose between 'gcc' and 'sdllvm' for toolchain. Default is gcc
TOOLCHAIN ??= "gcc"

TOOLCHAIN_class-native = "gcc"
TOOLCHAIN_class-nativesdk = "gcc"
TOOLCHAIN_class-cross-canadian = "gcc"
TOOLCHAIN_class-crosssdk = "gcc"
TOOLCHAIN_class-cross = "gcc"

# Add toolchain to pkg overrides.
OVERRIDES =. "${@['', 'toolchain-${TOOLCHAIN}:']['${TOOLCHAIN}' != '']}"
OVERRIDES[vardepsexclude] += "TOOLCHAIN"

BASEDEPENDS_append_toolchain-sdllvm_class-target = " llvm-arm-toolchain-native"

# Unavoidable hack to force libstdc++ to always use older gcc4 compatible ABI.
# Without this, compilation may fail with sdllvm using -std=c++11 as llvm having
# its own stdc++ can't reslove the new [abi::cxx11] symbols.
# See below for more info:
# https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html
# https://gcc.gnu.org/onlinedocs/libstdc++/manual/configure.html
# https://bugs.llvm.org/show_bug.cgi?id=23529
# https://wiki.debian.org/GCC5

EXTRA_OECONF_append_pn-gcc-runtime = " --disable-libstdcxx-dual-abi"
