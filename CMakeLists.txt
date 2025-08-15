cmake_minimum_required(VERSION 3.10)
project(heartopia-midi2key)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置第三方库路径
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty)
set(RTMIDI_DIR ${CMAKE_SOURCE_DIR}/3rdparty/rtmidi-6.0.0)

# 包含头文件目录
include_directories(${INCLUDE_DIR} ${RTMIDI_DIR})

# 添加源文件
add_executable(heartopia-midi2key 
    src/main.cpp 
    src/midi_handler.cpp 
    src/key_mapper.cpp 
    src/key_emulator.cpp 
    ${RTMIDI_DIR}/RtMidi.cpp)

# 如果在Windows上，定义WINDOWS_MM宏并链接winmm库（RtMidi依赖）
if(WIN32)
    add_definitions(-D__WINDOWS_MM__)
    target_link_libraries(heartopia-midi2key winmm)
endif()

# 复制配置文件到构建目录
add_custom_command(
    TARGET heartopia-midi2key POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/config/heartopia_piano.json
            $<TARGET_FILE_DIR:heartopia-midi2key>/heartopia_piano.json
)