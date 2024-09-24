
# 指定目标系统
set(CMAKE_SYSTEM_NAME Linux)
# 指定目标平台
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_PREFIX_PATH /home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target/usr)

# set(Qt5_DIR /home/cwf/allwinner/panasonic-R528/out/r528-evb3hmi/staging_dir/target/usr/lib/cmake/Qt5)
# 指定交叉编译工具链的根路径
set(CROSS_CHAIN_PATH /home/cwf/allwinner/panasonic-R528/prebuilt/gcc/linux-x86/arm/toolchain-sunxi-glibc/toolchain)
# 指定C编译器
set(CMAKE_C_COMPILER "${CROSS_CHAIN_PATH}/bin/arm-openwrt-linux-gcc")
# 指定C++编译器
set(CMAKE_CXX_COMPILER "${CROSS_CHAIN_PATH}/bin/arm-openwrt-linux-g++")
