cmake_minimum_required(VERSION 3.22.0)

set(SFML_VERSION "2.6.1")
set(BIN_PATH_NAME "bin")
set(RESOURCES_PATH_NAME "resources")
set(CMAKE_INSTALL_PREFIX "install")
set(CMAKE_INSTALL_RPATH "../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BIN_PATH_NAME})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(game-of-life LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG ${SFML_VERSION})
FetchContent_MakeAvailable(SFML)
  
add_executable(${PROJECT_NAME}
  Cell.hpp
  Controller.hpp
  Controller.cpp
  Model.hpp
  Model.cpp
  RleHelper.hpp
  RleHelper.cpp
  View.hpp
  View.cpp
  Main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
  sfml-graphics
  sfml-window)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion)
elseif(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE -W4)
endif()

if(WIN32)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    COMMENT "Copy OpenAL DLL"
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>VERBATIM)
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_PATH_NAME})
install(DIRECTORY ${RESOURCES_PATH_NAME} DESTINATION .)

include(InstallRequiredSystemLibraries)
set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")
include(CPack)
