cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project(linda-lib CXX)

option(BUILD_CLIENT "Build client" OFF)
option(COMPILE_TESTS "Compile tests" ON)


set(PROJECT_NAME linda-lib)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Og -ggdb -DDEBUG")

set(CMAKE_CXX_FLAGS_SANITIZE "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS_SANITIZE "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()


if(MSVC)
	include_directories("C:/Program Files (x86)/cli/include/")
endif()

include(CTest)
include(CMake/Utils.cmake)
include(CMake/Conan.cmake)

if(COMPILE_TESTS)
	Include(FetchContent)

	FetchContent_Declare(
  		Catch2
  		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  		GIT_TAG        v2.13.1)

	FetchContent_MakeAvailable(Catch2)

  #find_package(Catch2 REQUIRED)
endif()

# Use solution folders feature
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Sub-projects
add_subdirectory(lib)

if(BUILD_CLIENT)
   include_directories("lib")
   add_subdirectory(cli)
 endif()


