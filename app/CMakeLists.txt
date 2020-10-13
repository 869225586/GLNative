cmake_minimum_required(VERSION 3.4.1)
#！！！！！添加 第三方so  请把头文件 放到cpp 目录下 要不然 怎么可能找到头文件呢。。。。！！！！！
include_directories(src/main/cpp/include)

add_library( # Sets the name of the library.
        native-lib
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        src/main/cpp/native-lib.cpp
        src/main/cpp/egl/EglHelper.cpp
        src/main/cpp/egl/EglThread.cpp
        src/main/cpp/shader/ShaderUtil.h
        src/main/cpp/matrix/MatrixUtil.h
        src/main/cpp/opengl/BaseRender.cpp
        src/main/cpp/filter/FilterOne.cpp
        src/main/cpp/opengl/Opengl.cpp
        src/main/cpp/filter/WlFilterTwo.cpp
        src/main/cpp/filter/FilterYuv.cpp
        src/main/cpp/vbo/FilterVboYuv.cpp
        src/main/cpp/vbo/CameraRender.cpp
        src/main/cpp/vbo/FboRender.cpp
        #ffmpeg
        src/main/cpp/ffmpeg/FFmpeg.cpp
        src/main/cpp/ffmpeg/AudioPlayer.cpp
        src/main/cpp/ffmpeg/StreamQueue.cpp
        src/main/cpp/ffmpeg/PlayStatus.cpp
        src/main/cpp/ffmpeg/VideoPlayer.cpp
        src/main/cpp/calljava/CallJava.cpp
        )

add_library(avcodec-57 STATIC IMPORTED)
set_target_properties(avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libavcodec-57.so)

add_library(avdevice-57 SHARED IMPORTED)
set_target_properties(avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libavdevice-57.so)


add_library(avfilter-6 SHARED IMPORTED)
set_target_properties(avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libavfilter-6.so)


add_library(avformat-57 SHARED IMPORTED)
set_target_properties(avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libavformat-57.so)


add_library(avutil-55 SHARED IMPORTED)
set_target_properties(avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libavutil-55.so)

add_library(postproc-54 SHARED IMPORTED)
set_target_properties(postproc-54
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libpostproc-54.so)

add_library(swresample-2 SHARED IMPORTED)
set_target_properties(swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libswresample-2.so)


add_library(swscale-4 SHARED IMPORTED)
set_target_properties(swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libswscale-4.so)


find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)
target_link_libraries( # Specifies the target library.
        native-lib
        GLESv2
        EGL
        android
        #ffmpeg
        avcodec-57
        avdevice-57
        avfilter-6
        avformat-57
        avutil-55
        postproc-54
        swresample-2
        swscale-4

        OpenSLES
        # Links the target library to the log library
        # included in the NDK.
        log)