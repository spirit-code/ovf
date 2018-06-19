cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# file(STRINGS "Version.txt" VFRENDERING_VERSION)
project(ovf)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option( OVF_BUILD_PYTHON_BINDINGS  "Build the shared library for Python."           ON  )

set(SOURCE_FILES
    src/ovf.cpp
    src/detail/OVF_File.cpp
    src/detail/Filter_File_Handle.cpp
)

set(HEADER_FILES
    src/ovf.h
    src/detail/OVF_File.hpp
    src/detail/Filter_File_Handle.hpp
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_definitions( -DNOMINMAX )
endif()

### flag for fmt to be header-only
add_definitions( -DFMT_HEADER_ONLY )

### Object library
add_library( ${PROJECT_NAME} OBJECT ${SOURCE_FILES} )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

### Static library
add_library(${PROJECT_NAME}_static $<TARGET_OBJECTS:${PROJECT_NAME}>)

install(TARGETS ${PROJECT_NAME}_static DESTINATION lib)
install(DIRECTORY include/ovf DESTINATION include)


### Build Python bindings
if (OVF_BUILD_PYTHON_BINDINGS)
    add_library(${PROJECT_NAME}_python SHARED $<TARGET_OBJECTS:${PROJECT_NAME}>)

    set_property(TARGET ${PROJECT_NAME}_python PROPERTY OUTPUT_NAME "${PROJECT_NAME}")
    set_property(TARGET ${PROJECT_NAME}_python PROPERTY LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/python/ovf)
endif()


set(OVF_LIBRARIES ${PROJECT_NAME} PARENT_SCOPE)