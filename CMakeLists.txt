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

add_library(${PROJECT_NAME} ${SOURCE_FILES})
# target_link_libraries(${PROJECT_NAME} ${qhull_LIBS})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(DIRECTORY include/ovf DESTINATION include)


# Build Python bindings
if (OVF_BUILD_PYTHON_BINDINGS)
    add_library(${PROJECT_NAME}_python SHARED ${SOURCE_FILES})
    set_target_properties( ${PROJECT_NAME}_python PROPERTIES OUTPUT_NAME "${PROJECT_NAME}" )

    set_property(TARGET ${PROJECT_NAME}_python PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${PROJECT_NAME}_python PROPERTY CXX_STANDARD_REQUIRED ON)
    set_property(TARGET ${PROJECT_NAME}_python PROPERTY CXX_EXTENSIONS OFF)
    
    target_include_directories(${PROJECT_NAME}_python PUBLIC ${PROJECT_SOURCE_DIR}/include)

    set( PYLIB_OUTPUT_DIR ${PROJECT_SOURCE_DIR}/python/ovf )
    set_target_properties( ${PROJECT_NAME}_python PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PYLIB_OUTPUT_DIR} )

#     set(MODULE_NAME py${PROJECT_NAME})
#     add_subdirectory(thirdparty/pybind11)
#     pybind11_add_module(${MODULE_NAME} python/vfrendering_bindings.cpp)

#     target_compile_definitions(${MODULE_NAME} PUBLIC "-D${PROJECT_NAME}_VERSION=\"${${PROJECT_NAME}_VERSION}${MODULE_DEV_TAG}\"")

#     target_include_directories(${MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
#     target_include_directories(${MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/pybind11/include)
#     target_include_directories(${MODULE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/glm/include)
#     target_include_directories(${MODULE_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/glad/include)
#     target_include_directories(${MODULE_NAME} PRIVATE ${qhull_INCLUDE_DIRS})

#     target_link_libraries(${MODULE_NAME} PRIVATE ${PROJECT_NAME})
endif()


set(OVF_LIBRARIES ${PROJECT_NAME} PARENT_SCOPE)