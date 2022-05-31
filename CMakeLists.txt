cmake_minimum_required(VERSION "3.15")
project("zydis-js")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# Register Zydis dependency.
# Disable build of tools and examples.
option(ZYDIS_BUILD_TOOLS "" OFF)
option(ZYDIS_BUILD_EXAMPLES "" OFF)
add_subdirectory("deps/zydis")
include_directories(${CMAKE_JS_INC} ./node_modules/nan/)
# Add our project executable
file(GLOB SOURCE_FILES "src/*.cc" "src/*.h")

# Have CMake link our project executable against Zydis.
# target_link_libraries("MyProject" PRIVATE "Zydis")

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_JS_LIB} "Zydis")
