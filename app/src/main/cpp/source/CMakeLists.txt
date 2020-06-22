# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)
add_definitions(-w)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}   -s  -O3 -DSkip_f2c_Undefs -DNO_LONG_LONG -DNO_BLAS_WRAP")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s  -O3 -DSkip_f2c_Undefs -DNO_LONG_LONG -DNO_BLAS_WRAP")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
# 设置native目录
#set(NATIVE_DIR ${CMAKE_SOURCE_DIR}/src/main/cpp)
set(NATIVE_DIR ${CMAKE_SOURCE_DIR})

#set(clapack_dir ${NATIVE_DIR}/lapack)
set(LIBS_DIR ${NATIVE_DIR}/libs)

set(EXTRA_DIR ${NATIVE_DIR}/extra)

if ((${ANDROID_ABI} STREQUAL "arm64-v8a") OR (${ANDROID_ABI} STREQUAL "x86_64"))
    set(FAISS_DIR ${NATIVE_DIR}/faiss)
    add_definitions(-D ANDROID_BIT32)
else ()
    set(FAISS_DIR ${NATIVE_DIR}/faiss_32)
    add_definitions(-D ANDROID_BIT64)
endif ()


FILE(GLOB EXTRA_FILES
        ${EXTRA_DIR}/clapack/SRC/*.c
        ${EXTRA_DIR}/clapack/SRC/*.cpp
        )
FILE(GLOB FAISS_FILES
        ${FAISS_DIR}/*.cpp
        #add faiss source more
        #${FAISS_DIR}/impl/*.cpp
        #${FAISS_DIR}/utils/*.cpp
        )

include_directories(${FAISS_DIR})

add_library( # Sets the name of the library.
        faiss
        SHARED
        #STATIC
        ${EXTRA_FILES}
        ${FAISS_FILES}


        #compile native-lib.cpp if test
        native-lib.cpp
        )


find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

target_link_libraries( # Specifies the target library.
        faiss

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})