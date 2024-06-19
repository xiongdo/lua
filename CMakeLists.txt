cmake_minimum_required(VERSION 3.5)

project(lua)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

include_directories(${PROJECT_SOURCE_DIR})

############################################################
# Create libraries
############################################################

add_library(lua STATIC 
    lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c
    lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstate.c 
    lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c ltests.c lauxlib.c lbaselib.c 
    ldblib.c liolib.c lmathlib.c loslib.c ltablib.c lstrlib.c lutf8lib.c
    loadlib.c lcorolib.c linit.c 
)

target_compile_features(lua PRIVATE cxx_std_17)

set_property(TARGET lua PROPERTY OUTPUT_NAME lualib)

############################################################
# Create an executable
############################################################

add_executable(lua_binary lua.c)

target_compile_features(lua_binary PRIVATE cxx_std_17)

target_link_libraries(lua_binary PUBLIC lua)

set_property(TARGET lua_binary PROPERTY OUTPUT_NAME lua)

############################################################
# Create an executable
############################################################

add_executable(luac luac.c)

target_compile_features(luac PRIVATE cxx_std_17)

target_link_libraries(luac PUBLIC lua)