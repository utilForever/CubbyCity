# Target name
set(target CubbyCity)

# Define
set(CUBBYCITY_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries
	${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/glm
	${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/stb
	${CMAKE_CURRENT_SOURCE_DIR}/../../Libraries/json
)

# Sources
file(GLOB CUBBYCITY_HEADER_DIR
    ${CUBBYCITY_ROOT_DIR}/Includes)

file(GLOB_RECURSE CUBBYCITY_HEADERS
    ${CUBBYCITY_HEADER_DIR}/*.hpp)

file(GLOB_RECURSE CUBBYCITY_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Programs/*.cpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(CUBBYCITY_SOURCES ${CUBBYCITY_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/Platform/WinDownloader.cpp)
else()
    find_package(CURL)
    if (CURL_FOUND)
        set(CUBBYCITY_HEADERS ${CUBBYCITY_HEADERS} ${CURL_INCLUDE_DIR})
        set(CUBBYCITY_LIBS ${CUBBYCITY_LIBS} ${CURL_LIBRARIES})
        set(CUBBYCITY_SOURCES ${CUBBYCITY_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/Platform/CurlDownloader.cpp)
    else()
        message(FATAL_ERROR "Could not find the CURL library and development files.")
    endif()
endif()

# Build library
add_library(${target}
    ${CUBBYCITY_SOURCES})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
)

# Compile options
target_compile_options(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

    INTERFACE
)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(${target}
        PRIVATE

        PUBLIC
        ${DEFAULT_LINKER_OPTIONS}
	    ${DEFAULT_LIBRARIES}
		${CUBBYCITY_LIBS}

        INTERFACE
    )
else()
    target_link_libraries(${target}
        PRIVATE

        PUBLIC
        ${DEFAULT_LINKER_OPTIONS}
	    ${DEFAULT_LIBRARIES}
		${CUBBYCITY_LIBS}
		-lcurl

        INTERFACE
    )
endif()

# Install
install(TARGETS ${target} DESTINATION lib)
install(DIRECTORY ${CUBBYCITY_HEADER_DIR} DESTINATION include)