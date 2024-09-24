#!/bin/sh

export PATH="$PATH:/home/cwf/allwinner/panasonic-R528/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-glibc/toolchain/bin"
export TARGET_CC="arm-openwrt-linux-gcc"
export TARGET_CXX="arm-openwrt-linux-g++"
export TARGET_AR="arm-openwrt-linux-ar"
export STAGING_DIR="/home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target"
export PKG_CONFIG_SYSROOT_DIR="/home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target"
export PKG_CONFIG_PATH="/home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target/usr/lib/pkgconfig"
export PKG_CONFIG_LIBDIR="/home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target/usr/lib"
export TARGET_CFLAGS="-Os -pipe -march=armv7-a -mtune=cortex-a7 -mfpu=neon -fno-caller-saves -Wno-unused-result -mfloat-abi=hard  -Wformat -Werror=format-security -fPIC -D_FORTIFY_SOURCE=2 -Wl,-z,now -Wl,-z,relro -DUSE_SUNXIFB_DOUBLE_BUFFER -DUSE_SUNXIFB_CACHE -DUSE_SUNXIFB_G2D -DUSE_SUNXIFB_G2D_ROTATE -DCONF_G2D_VERSION_NEW -I/home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target/usr/include/freetype2 -I./ -DLV_USE_SUNXIFB_G2D_FILL -DLV_USE_SUNXIFB_G2D_BLEND -DLV_USE_SUNXIFB_G2D_BLIT -DLV_USE_SUNXIFB_G2D_SCALE"
export TARGET_LDFLAGS="-L/home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target/usr/lib -L/home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target/lib -L/home/cwf/allwinner/panasonic-R528/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-glibc/toolchain/usr/lib -L/home/cwf/allwinner/panasonic-R528/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-glibc/toolchain/lib -specs=/home/cwf/allwinner/panasonic-R528/build/hardened-ld-pie.specs -znow -zrelro -luapi -lfreetype"

cmake ../ -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake

make -j8

