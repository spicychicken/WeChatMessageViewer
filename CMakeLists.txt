cmake_minimum_required(VERSION 3.1.0)

# Project
project(WeChatMessageViewer VERSION 0.1 LANGUAGES C CXX)

find_package(Qt6 COMPONENTS Core Qml Quick Charts REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(unofficial-libplist CONFIG REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

# OpenAL
find_package(OpenAL CONFIG REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# src
include_directories(src)
include_directories(thirdparty)
include_directories(thirdparty/silk/interface)

add_definitions(-DMMKV_DISABLE_CRYPT)

# UI sources
file(
    GLOB_RECURSE _ui_list 
    LIST_DIRECTORIES false
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qml/components.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/qml/windows.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/icons.qrc"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/images.qrc"
)

# ---------------------------------------------------------------
# Include sources / headers
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${CMAKE_SOURCE_DIR}/src/*.cpp*"
    "${CMAKE_SOURCE_DIR}/src/*.c*"
    "${CMAKE_SOURCE_DIR}/src/*.h*"
    "${CMAKE_SOURCE_DIR}/thirdparty/*.cpp*"
    "${CMAKE_SOURCE_DIR}/thirdparty/*.c*"
    "${CMAKE_SOURCE_DIR}/thirdparty/*.h*"
)

add_executable(WeChatMessageViewer
    ${_source_list}
    ${_ui_list}
)

set_target_properties(WeChatMessageViewer PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(WIN32)
    add_definitions(-DSQLITE_HAS_CODEC)
    find_package(sqlcipher CONFIG REQUIRED)
    target_link_libraries(WeChatMessageViewer PRIVATE Version.lib sqlcipher::sqlcipher)
endif()

target_link_libraries(WeChatMessageViewer PRIVATE 
                Qt6::Core
                Qt6::Qml
                Qt6::Quick
                Qt6::Charts
                protobuf::libprotoc
                protobuf::libprotobuf
                protobuf::libprotobuf-lite
                unofficial::sqlite3::sqlite3
                tinyxml2::tinyxml2
                OpenAL::OpenAL
                JsonCpp::JsonCpp
                unofficial::libplist::libplist
                unofficial::libplist::libplist++)