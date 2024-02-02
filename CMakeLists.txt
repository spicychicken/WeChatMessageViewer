cmake_minimum_required(VERSION 3.1.0)

# Project
project(WeChatMessageViewer VERSION 0.1 LANGUAGES C CXX)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

# OpenAL
find_package(OpenAL CONFIG REQUIRED)

# uic文件 搜索路径
set(CMAKE_AUTOUIC_SEARCH_PATHS src/qtui/designer)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# src
include_directories(src)
include_directories(thirdparty)
include_directories(thirdparty/silk/interface)

add_definitions(-DMMKV_DISABLE_CRYPT)

# ---------------------------------------------------------------
# Include ts file
file(
    GLOB_RECURSE _ts_list 
    LIST_DIRECTORIES false
    "${CMAKE_SOURCE_DIR}/src/qtui/designer/translations/*.ts*"
)

set_source_files_properties(${_ts_list} PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations")
configure_file(src/qtui/designer/translations.qrc ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
qt_add_translation(qm_files ${_ts_list})
add_custom_target(translations DEPENDS ${qm_files})

# ---------------------------------------------------------------
# Include ui file
file(
    GLOB_RECURSE _ui_list 
    LIST_DIRECTORIES false
    "${CMAKE_SOURCE_DIR}/src/qtui/designer/*.ui*"
    "${CMAKE_SOURCE_DIR}/src/qtui/designer/resources.qrc*"
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
    ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    )

set_target_properties(WeChatMessageViewer PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

find_library(LIBPLIST plist-2.0)

target_link_libraries(WeChatMessageViewer PRIVATE 
                Qt5::Widgets
                protobuf::libprotoc
                protobuf::libprotobuf
                protobuf::libprotobuf-lite
                unofficial::sqlite3::sqlite3
                tinyxml2::tinyxml2
                fmt::fmt-header-only
                OpenAL::OpenAL
                ${LIBPLIST})