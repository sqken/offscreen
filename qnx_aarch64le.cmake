set(output_type $ENV{CMAKE_OUTPUT_TYPE})

set(CMAKE_SYSTEM_NAME QNX)

if ("$ENV{G_CONFIG_QNX_TARGET_ARCH}" STREQUAL "aarch64")
    set(CMAKE_SYSTEM_PROCESSOR ntoaarch64)
else ()
    set(CMAKE_SYSTEM_PROCESSOR ntox86_64)
endif ()


set(CMAKE_PREFIX_PATH "/data/work/atlas/qt/prebuilt")
set(CMAKE_FIND_ROOT_PATH   $ENV{QNX_TARGET} $ENV{QNX_TARGET}/aarch64le/)

set(CMAKE_C_COMPILER       $ENV{CC})
set(CMAKE_CXX_COMPILER     $ENV{CXX})
set(CMAKE_AS               $ENV{AS})
set(CMAKE_LD               $ENV{LD})
set(CMAKE_GDB              $ENV{GDB})
set(CMAKE_STRIP            $ENV{STRIP})
set(CMAKE_RANLIB           $ENV{RANLIB})
set(CMAKE_OBJCOPY          $ENV{OBJCOPY})
set(CMAKE_OBJDUMP          $ENV{OBJDUMP})
set(CMAKE_AR               $ENV{AR})
set(CMAKE_NM               $ENV{NM})

set(COMPILE_FLAGS          "${COMPILE_FLAGS} $ENV{CFLAGS} ")
# set(COMPILE_FLAGS          "${COMPILE_FLAGS} -Wall -Wno-unused-function -Wno-deprecated-declarations -Werror --sysroot=${SYSROOTS_PATH}")
set(COMPILE_FLAGS          "${COMPILE_FLAGS} -D__FILENAME__='\"$$(subst .o,,$$(notdir $$(abspath $$@)))\"'")
set(COMPILE_FLAGS          "${COMPILE_FLAGS} $ENV{CMAKE_COMPILE_FLAG}")

set(CMAKE_C_FLAGS          "${CMAKE_C_COMPILER_ARG1} ${COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_COMPILER_ARG1} ${COMPILE_FLAGS} -std=gnu++14 -fPIC")

set(CMAKE_SHARED_LINKER_FLAGS "$ENV{LDFLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${SYSROOTS_PATH}")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L$ENV{QT_INSTALL_DIR}/lib")
set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_SHARED_LINKER_FLAGS}")

set(QT_QMAKE_TARGET_MKSPEC qnx-aarch64le-qcc)