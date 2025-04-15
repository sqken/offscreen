# Build for QNX

cd /data/work/atlas/  
source set_env.sh  

cd /data/test/qt/offscreen/  
mkdir qnx_build  
/data/work/atlas/qt/prebuilt/bin/qt-cmake -DQT_CHAINLOAD_TOOLCHAIN_FILE=/data/test/qt/offscreen/qnx_aarch64le.cmake ..  
make  