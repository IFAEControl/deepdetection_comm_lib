set(PROJECT_NAME linda-lib)

option(BUILD_CLIENT "Build client" OFF)

################################################################################
# Source groups
################################################################################
file(GLOB ALL_FILES "*.cpp")
list(FILTER ALL_FILES EXCLUDE REGEX ".*_test.cpp")

file(GLOB TEST_FILES "*_test.cpp")

source_group("Archivos de origen" FILES ${ALL_FILES})


################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME}-temp STATIC ${ALL_FILES})
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-temp ${CONAN_LIBS})

if(NOT MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wcast-qual -Wunreachable-code -Wfatal-errors)
endif()

if(COMPILE_TESTS)
  add_library(Catch INTERFACE)
  target_include_directories(Catch INTERFACE)

  add_executable(tests ${TEST_FILES})
  target_link_libraries(tests PRIVATE Catch2::Catch2 linda-lib)
  add_test(NAME TESTS COMMAND tests)

  enable_testing(true)
endif()

find_program(CLANG_FORMAT clang-format)
if(EXISTS ${CLANG_FORMAT} AND NOT MSVC)
  add_custom_target(clangformat
        COMMAND clang-format
        -style=file
        -i
        ${ALL_FILES}
  )

  add_dependencies(${PROJECT_NAME} clangformat)
else()
  message(AUTHOR_WARNING "clang-format is not installed, auto formating will not be performed")
endif()
