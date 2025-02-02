cmake_minimum_required(VERSION 3.8)

project(random)

# set debug compilation flags
get_property(DebugFlags GLOBAL PROPERTY ArkDebugFlags)
if (DebugFlags)
    set(CMAKE_CXX_FLAGS_DEBUG "${DebugFlags}" CACHE STRING "Debug compilation options" FORCE)
endif()

# set release compilation flags
get_property(ReleaseFlags GLOBAL PROPERTY ArkReleaseFlags)
if (ReleaseFlags)
    set(CMAKE_CXX_FLAGS_RELEASE "${ReleaseFlags}" CACHE STRING "Release compilation options" FORCE)
endif()

# set linker flags
get_property(LinkerFlags GLOBAL PROPERTY ArkLinkerFlags)
if (LinkerFlags)
    set(CMAKE_EXE_LINKER_FLAGS "${LinkerFlags}" CACHE STRING "Linker options" FORCE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${PROJECT_SOURCE_DIR}/include
    ${ark_SOURCE_DIR}/thirdparty
    ${ark_SOURCE_DIR}/include
)

file(GLOB_RECURSE SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

list(REMOVE_ITEM SOURCE_FILES "${PROJECT_SOURCE_DIR}/../../src/main.cpp")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE ArkReactor)

if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
endif()

if (UNIX OR LINUX)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DL_LIBS})
endif()

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)

add_custom_command(TARGET ${PROJECT_NAME}
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:${PROJECT_NAME}>" ${PROJECT_SOURCE_DIR}/../../${PROJECT_NAME}.${MODULE_EXTENSION}
)
